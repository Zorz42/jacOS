#include "memory.hpp"
#include "text/text.hpp"
#include "ports/ports.hpp"
#include "qemuDebug/debug.hpp"

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

struct MallocHead {
    unsigned int size;
    char free;
    MallocHead *next, *prev;
    MallocHead* getNext() { if(next != nullptr) return (MallocHead*)((unsigned int)this + (unsigned int)sizeof(MallocHead) + (unsigned int)size); else return nullptr; }
};

struct MemInfo {
    unsigned int base, base_high;
    unsigned int length, length_high;
    unsigned int type;
} __attribute__((packed));

static unsigned int heap_base, total_memory;
static unsigned int allocated_frames;
static unsigned int *free_frames, free_frames_size;
static mem::PageDirectory *kernel_page_directory = nullptr, *current_page_directory = nullptr;

static void memset(void* pointer, u8 value, unsigned int length) {
    u8* iter = (u8*)pointer;
    while(length--)
        *iter++ = value;
}

static void setFrame(unsigned int address, bool allocated) {
    unsigned int frame = address / 0x1000;
    unsigned int index = frame / 32;
    unsigned int offset = frame % 32;
    if(((free_frames[index] & (1 << offset)) != 0) != allocated) {
        if(address < mem::getTotal()) {
            if(allocated)
                allocated_frames++;
            else
                allocated_frames--;
        }
        
        if(allocated)
            free_frames[index] |= 1 << offset;
        else
            free_frames[index] &= ~(1 << offset);
        
    }
}

static bool getFrame(unsigned int address) {
    unsigned int frame = address / 0x1000;
    unsigned int index = frame / 32;
    unsigned int offset = frame % 32;
    return (free_frames[index] & (1 << offset)) != 0;
}

static unsigned int getFirstFreeFrame() {
    for(unsigned int i = 0; i < free_frames_size / 32; i++)
        if(free_frames[i] != 0xFFFFFFFF) // nothing free, exit early.
            // at least one bit is free here.
            for(unsigned int j = 0; j < 32; j++) {
                if(!(free_frames[i] & (1 << j))) {
                    int result = i * 32 + j;
                    if(result * 0x1000 >= mem::getTotal())
                        asm("int $25");
                    return result;
                }
            }
    asm("int $25");
    return 0;
}

unsigned int mem::getUsed() {
    return allocated_frames * 0x1000;
}

void mem::allocateFrame(PageHead* page_head, bool is_kernel, bool is_writable) {
    if(page_head->frame == 0) {
        unsigned int index = getFirstFreeFrame();
        debug::log("allocateFrame() called on ");
        debug::logHex(index * 0x1000);
        debug::log("\n");
        setFrame(index * 0x1000, true);
        page_head->present = 1;
        page_head->rw = is_writable ? 1 : 0;
        page_head->user = is_kernel ? 0 : 1;
        page_head->frame = index;
    }
}

void mem::freeFrame(PageHead* page_head) {
    if(page_head->frame != 0) {
        setFrame(page_head->frame * 0x1000, false);
        page_head->frame = 0;
    }
}

void mem::switchPageDirectory(PageDirectory* page_directory) {
    current_page_directory = page_directory;
    asm volatile("mov %0, %%cr3":: "r"(&page_directory->physical_table_addresses));
    u32 cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

mem::PageHead* mem::getPage(unsigned int address, PageDirectory* page_directory) {
    if(page_directory == nullptr)
        page_directory = current_page_directory;
    
    address /= 0x1000;
    
    unsigned int table_index = address / 1024;
    
    if(!page_directory->tables_allocated[table_index]) {
        page_directory->tables_allocated[table_index] = true;
        debug::log("getPage() ");
        debug::logInt(table_index);
        debug::log("\n");
        //identityMapPage((unsigned int)&page_directory->tables[table_index], true, true, page_directory);
        allocateFrame(getPage((unsigned int)&page_directory->tables[table_index], page_directory), true, true);
        int physical_address = virtualToPhysicalAddress((unsigned int)&page_directory->tables[table_index], page_directory);
        page_directory->physical_table_addresses[table_index] = physical_address | 7;
    }
    
    return &page_directory->tables[table_index].pages[address % 1024];
}


void mem::identityMapPage(unsigned int address, bool is_kernel, bool is_writable, PageDirectory* page_directory) {
    if(page_directory == nullptr)
        page_directory = current_page_directory;
    
    debug::log("identityMapPage() called on ");
    debug::logHex(address);
    debug::log("\n");
    
    if(getFrame(address)) {
        debug::log("error: identityMapPage() called on already taken address ");
        debug::logHex(address);
        debug::log("\n");
        asm("int $26");
    }
    
    PageHead* page_head = getPage(address, page_directory);
    
    setFrame(address, true);
    page_head->present = 1;
    page_head->rw = is_writable ? 1 : 0;
    page_head->user = is_kernel ? 0 : 1;
    page_head->frame = address / 0x1000;
}

unsigned int mem::virtualToPhysicalAddress(unsigned int virtual_address, PageDirectory* page_directory) {
    if(page_directory == nullptr)
        page_directory = current_page_directory;
    
    int page_index = virtual_address / 0x1000;
    int table_index = page_index / 1024;
    
    if(page_directory->tables[table_index].pages[page_index % 1024].frame == 0) {
        debug::log("virtualToPhysicalAddress() ");
        debug::logHex(virtual_address);
        debug::log("\n");
    }
    
    if(page_directory == nullptr)
        return virtual_address;
    else
        return page_directory->tables[table_index].pages[page_index % 1024].frame * 0x1000 + (virtual_address & 0xFFF);
}


void* mem::alloc(u32 size) {
    MallocHead* head = (MallocHead*)heap_base;
    
    // find a free block
    while(true) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
            asm("int $19");
        
        if(head->free == HEAD_FREE && head->size >= size)
            break;
        
        if(head->next == nullptr) {
            for(int i = 0; i <= size; i += 0x1000) {
                unsigned int address = (unsigned int)head + sizeof(MallocHead) + head->size;
                
                if(current_page_directory != nullptr)
                    allocateFrame(getPage(address), true, true);
                
                if(head->free == HEAD_FREE)
                    head->size += 0x1000;
                else if(head->free == HEAD_ALLOCATED) {
                    MallocHead* new_head = (MallocHead*)address;
                    new_head->free = HEAD_FREE;
                    new_head->size = 0x1000 - sizeof(MallocHead);
                    new_head->next = nullptr;
                    new_head->prev = head;
                    head->next = new_head;
                    head = new_head;
                } else
                    asm("int $19");
            }
            
            return alloc(size);
        }
        
        head = head->next;
    }
    
    if(head->size - size > sizeof(MallocHead)) {
        // split that block into two blocks
        MallocHead* next_head = (MallocHead*)((unsigned int)head + sizeof(MallocHead) + size);
        next_head->free = HEAD_FREE;
        next_head->size = head->size - size - sizeof(MallocHead);
        next_head->next = head->next;
        next_head->prev = head;
        if(next_head->next)
            next_head->next->prev = next_head;
        
        head->size = size;
        head->next = next_head;
    }
    
    head->free = HEAD_ALLOCATED;
    
    return (void*)((unsigned int)head + sizeof(MallocHead));
}

static void mergeBlocks(MallocHead* block1, MallocHead* block2) {
    block1->size += block2->size + sizeof(MallocHead);
    block1->next = block2->next;
}

void mem::free(void* ptr) {
    MallocHead* head = (MallocHead*)((unsigned int)ptr - sizeof(MallocHead));
    if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
        asm("int $22");
    
    if(head->free == HEAD_FREE)
        asm("int $23");
    
    head->free = HEAD_FREE;
    
    if(head->next && head->next->free == HEAD_FREE)
        mergeBlocks(head, (MallocHead*)head->next);
    if(head->prev && head->prev->free == HEAD_FREE)
        mergeBlocks((MallocHead*)head->prev, head);
}

static MemInfo* getFreeRegion() {
    for(int i = 0x7e00; ; i += 24) {
        MemInfo* curr_info = (MemInfo*)i;
        if(curr_info->type == 1 && curr_info->base != 0)
            return curr_info;
    }
    
    asm("int $24");
}

void mem::init() {
    MemInfo* target_info = getFreeRegion();
    
    heap_base = 0x400000;
    total_memory = target_info->base + target_info->length;
    
    kernel_page_directory = (PageDirectory*)heap_base;
    memset(kernel_page_directory, 0, sizeof(PageDirectory));
    heap_base += sizeof(PageDirectory);
    
    heap_base = (heap_base - 1) / 0x1000 * 0x1000 + 0x1000;
    
    free_frames_size = 1024 * 1024;
    free_frames = (unsigned int*)heap_base;
    memset(free_frames, 0, free_frames_size / 8);
    heap_base += free_frames_size / 8;
    allocated_frames = 0;
    
    heap_base = (heap_base - 1) / 0x1000 * 0x1000 + 0x1000;
    
    MallocHead* main_head = (MallocHead*)heap_base;
    main_head->free = HEAD_FREE;
    main_head->size = 0x2000 - sizeof(MallocHead);
    main_head->next = nullptr;
    main_head->prev = nullptr;
    
    int to_alloc[] = {1, 2, 0};
    for(int i = 0; i < 3; i++) {
        int table_index = to_alloc[i];
        kernel_page_directory->tables_allocated[table_index] = true;
        kernel_page_directory->physical_table_addresses[table_index] = (unsigned int)&kernel_page_directory->tables[table_index] | 7;
        identityMapPage((unsigned int)kernel_page_directory + i * 0x1000, true, true, kernel_page_directory);
    }
    
    for(int i = 0; i < 0x100000; i += 0x1000)
        identityMapPage(i, true, true, kernel_page_directory);
    
    for(int i = (unsigned int)kernel_page_directory + 1024 * 1024 * 4; i < (unsigned int)kernel_page_directory + sizeof(PageDirectory); i += 0x1000)
        identityMapPage(i, true, true, kernel_page_directory);
    
    for(int i = (unsigned int)free_frames; i < (unsigned int)free_frames + free_frames_size / 8; i += 0x1000)
        identityMapPage(i, true, true, kernel_page_directory);
    
    MallocHead* curr_head = main_head;
    while(curr_head->next != nullptr)
        curr_head = curr_head->next;
    for(int i = heap_base; i <= (unsigned int)curr_head + sizeof(MallocHead) + curr_head->size; i += 0x1000)
        identityMapPage(i, true, true, kernel_page_directory);
    
    switchPageDirectory(kernel_page_directory);
    
    mem::alloc(0x10000000);
    
    debug::log("init() end\n");
}

unsigned int mem::getTotal() {
    return total_memory;
}

unsigned int mem::getFree() {
    return total_memory - mem::getUsed();
}
