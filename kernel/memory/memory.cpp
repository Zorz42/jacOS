#include "memory.hpp"

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

struct MallocHead {
    unsigned int size;
    char free;
    MallocHead *next, *prev;
};

struct MemInfo {
    unsigned int base, base_high;
    unsigned int length, length_high;
    unsigned int type;
} __attribute__((packed));

static unsigned int heap_base, total_memory, used_memory;
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
    if(allocated)
        free_frames[index] |= 1 << offset;
    else
        free_frames[index] &= ~(1 << offset);
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
                if(!(free_frames[i] & (1 << j)))
                    return i * 32 + j;
            }
    asm("int $25");
    return 0;
}

void mem::allocateFrame(PageHead* page_head, bool is_kernel, bool is_writable) {
    if(page_head->frame == 0) {
        unsigned int index = getFirstFreeFrame();
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
    
    if(page_directory->tables[table_index] == nullptr) {
        page_directory->tables[table_index] = (PageTable*)alloc(sizeof(PageTable), /*aligned*/true);
        unsigned int physical_address = virtualToPhysicalAddress((unsigned int)page_directory->tables[table_index]);
        memset(page_directory->tables[table_index], 0, 0x1000);
        page_directory->physical_table_addresses[table_index] = physical_address | 7;
    }
    
    return &page_directory->tables[table_index]->pages[address % 1024];
}

void mem::indentityMapPage(unsigned int address, bool is_kernel, bool is_writable, PageDirectory* page_directory) {
    if(page_directory == nullptr)
        page_directory = current_page_directory;
    
    PageHead* page_head = getPage(address, page_directory);
    
    if(getFrame(address))
        asm("int $26");
    
    setFrame(address, true);
    page_head->present = 1;
    page_head->rw = is_writable ? 1 : 0;
    page_head->user = is_kernel ? 0 : 1;
    page_head->frame = address / 0x1000;
}

unsigned int mem::virtualToPhysicalAddress(unsigned virtual_address) {
    return virtual_address;
}

void* mem::alloc(u32 size, bool page_align) {
    MallocHead* head = (MallocHead*)heap_base;
    
    // find a free block
    while(true) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
            asm("int $20");
        
        if(page_align) {
            if(head->free == HEAD_FREE) {
                // align address
                unsigned int address = ((unsigned int)head) + sizeof(MallocHead) - 1;
                address &= 0xFFFFF000;
                address += 0x1000;
                // check if the aligned block fits into the free block
                if((unsigned int)head + sizeof(MallocHead) + head->size >= address + size) {
                    MallocHead* new_head = (MallocHead*)(address - sizeof(MallocHead));
                    new_head->size = (unsigned int)head + sizeof(MallocHead) + head->size - address;
                    new_head->next = head->next;
                    new_head->prev = head;
                    head->next = new_head;
                    head->size = address - (unsigned int)head - sizeof(MallocHead);
                    new_head->free = HEAD_FREE;
                    
                    head = new_head;
                    break;
                }
            }
        } else {
            if(head->free == HEAD_FREE && head->size >= size)
                break;
        }
        if(head->next == 0)
            asm("int $21");
        
        head = (MallocHead*)head->next;
    }
    
    if(head->size - size > sizeof(MallocHead)) {
        // split that block into two blocks
        MallocHead* next_head = (MallocHead*)((unsigned int)head + sizeof(MallocHead) + size);
        next_head->free = HEAD_FREE;
        next_head->size = head->size - size - sizeof(MallocHead);
        next_head->next = head->next;
        next_head->prev = head;
        
        head->size = size;
        head->next = next_head;
    }
    
    head->free = HEAD_ALLOCATED;
    
    used_memory += head->size + sizeof(MallocHead);
    
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
    used_memory -= head->size + sizeof(MallocHead);
    
    if(head->next && ((MallocHead*)head->next)->free == HEAD_FREE)
        mergeBlocks(head, (MallocHead*)head->next);
    if(head->prev && ((MallocHead*)head->prev)->free == HEAD_FREE)
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
    
    heap_base = target_info->base;
    used_memory = 0;
    total_memory = target_info->length;
    
    MallocHead* main_head = (MallocHead*)heap_base;
    main_head->free = HEAD_FREE;
    main_head->size = total_memory - sizeof(MallocHead);
    main_head->next = 0;
    main_head->prev = 0;
    
    free_frames_size = (target_info->base + target_info->length) / 0x1000;
    free_frames = (unsigned int*)alloc((free_frames_size - 1) / 32 + 1);
    memset(free_frames, 0, (free_frames_size - 1) / 32 + 1);
    
    kernel_page_directory = (PageDirectory*)alloc(sizeof(PageDirectory), /*align*/true);
    memset(kernel_page_directory, 0, sizeof(PageDirectory));
    
    for(unsigned int i = 0; i < target_info->base + 0x600000; i += 0x1000)
        indentityMapPage(i, true, true, kernel_page_directory);
    
    for(unsigned int i = 0xFD000000; i < 0xFD000000 + 0x600000; i += 0x1000)
        indentityMapPage(i, true, true, kernel_page_directory);
    
    
    switchPageDirectory(kernel_page_directory);
}

unsigned int mem::getUsed() {
    return used_memory;
}

unsigned int mem::getTotal() {
    return total_memory;
}

unsigned int mem::getFree() {
    return total_memory - used_memory;
}
