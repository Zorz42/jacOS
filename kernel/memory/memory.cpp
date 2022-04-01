#include "memory.hpp"
#include "text/text.hpp"
#include "qemuDebug/debug.hpp"
#include "interrupts/interrupts.hpp"

struct MemInfo {
    unsigned int base, base_high;
    unsigned int length, length_high;
    unsigned int type;
} __attribute__((packed));

static unsigned int total_memory;
static unsigned int allocated_frames;
static unsigned int *free_frames, free_frames_size;
static mem::PageDirectory *kernel_page_directory = nullptr, *current_page_directory = nullptr;

static void memset(void* pointer, unsigned char value, unsigned int length) {
    unsigned char* iter = (unsigned char*)pointer;
    while(length--)
        *iter++ = value;
}

static void setFrame(unsigned int address, bool allocated) {
    unsigned int frame = address / 0x1000;
    unsigned int index = frame / 32;
    unsigned int offset = frame % 32;
    if(((free_frames[index] & (1 << offset)) != 0) != allocated) {
        if(address < total_memory) {
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
                    if(result * 0x1000 >= total_memory) {
                        debug::out << DEBUG_ERROR << "Ran out of free frames" << debug::endl;
                        asm("int $0x18");
                    }
                    return result;
                }
            }
    debug::out << DEBUG_ERROR << "Ran out of free frames" << debug::endl;
    asm("int $0x18");
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
    unsigned int cr0;
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
        allocateFrame(getPage((unsigned int)&page_directory->tables[table_index], page_directory), /*is_kernel*/true, /*is_writable*/true);
        int physical_address = virtualToPhysicalAddress((unsigned int)&page_directory->tables[table_index], page_directory);
        page_directory->physical_table_addresses[table_index] = physical_address | 0b111;
    }
    
    return &page_directory->tables[table_index].pages[address % 1024];
}


void mem::identityMapPage(unsigned int address, bool is_kernel, bool is_writable, PageDirectory* page_directory) {
    if(page_directory == nullptr)
        page_directory = current_page_directory;
    
    if(getFrame(address)) {
        debug::out << DEBUG_ERROR << "identityMapPage() called on already taken address" << debug::hex << address << debug::endl;
        asm("int $0x14");
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
    
    if(page_directory == nullptr)
        return virtual_address;
    else
        return page_directory->tables[table_index].pages[page_index % 1024].frame * 0x1000 + (virtual_address & 0xFFF);
}

static MemInfo* getFreeRegion() {
    for(int i = 0x7f00; ; i += 24) {
        MemInfo* curr_info = (MemInfo*)i;
        if(curr_info->type == 1 && curr_info->base != 0)
            return curr_info;
    }
    
    debug::out << DEBUG_ERROR << "Cannot find free region" << debug::endl;
    asm("int $0x17");
}

unsigned int syscallGetTotal(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    return total_memory;
}

unsigned int syscallGetUsed(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    return allocated_frames * 0x1000;
}

unsigned int syscallAllocateFrame(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    mem::allocateFrame(mem::getPage(arg1), /*is_kernel*/false, /*is_writable*/true);
    return 0;
}
    

void mem::init() {
    interrupts::registerSyscallHandler(&syscallAllocateFrame, "allocateFrame");
    
    MemInfo* target_info = getFreeRegion();
    
    unsigned int base = 0x400000;
    total_memory = target_info->base + target_info->length;
    debug::out << "Total memory is: " << debug::hex << total_memory << debug::endl;
    
    kernel_page_directory = (PageDirectory*)base;
    current_page_directory = kernel_page_directory;
    memset(kernel_page_directory, 0, sizeof(PageDirectory));
    base += sizeof(PageDirectory);
    debug::out << "Kernel page directory is from: " << debug::hex << (unsigned int)kernel_page_directory << " to: " << (unsigned int)kernel_page_directory + sizeof(PageDirectory) << debug::endl;
    
    base = (base - 1) / 0x1000 * 0x1000 + 0x1000;
    
    free_frames_size = 1024 * 1024;
    free_frames = (unsigned int*)base;
    memset(free_frames, 0, free_frames_size / 8);
    base += free_frames_size / 8;
    allocated_frames = 0;
    
    debug::out << "Setting up first three page tables" << debug::endl;
    int to_alloc[] = {1, 2, 0};
    for(int i = 0; i < 3; i++) {
        int table_index = to_alloc[i];
        kernel_page_directory->tables_allocated[table_index] = true;
        kernel_page_directory->physical_table_addresses[table_index] = (unsigned int)&kernel_page_directory->tables[table_index] | 7;
        identityMapPage((unsigned int)kernel_page_directory + i * 0x1000, /*is_kernel*/true, /*is_writable*/true, kernel_page_directory);
    }
    
    debug::out << "Identity mapping first megabyte of memory" << debug::endl;
    for(int i = 0; i < 0x100000; i += 0x1000)
        identityMapPage(i, /*is_kernel*/false, /*is_writable*/true, kernel_page_directory);
    
    debug::out << "Identity mapping kernel page directory" << debug::endl;
    for(int i = (unsigned int)kernel_page_directory + 1024 * 1024 * 4; i < (unsigned int)kernel_page_directory + sizeof(PageDirectory); i += 0x1000)
        identityMapPage(i, /*is_kernel*/true, /*is_writable*/true, kernel_page_directory);
    
    debug::out << "Identity mapping free frames array" << debug::endl;
    for(int i = (unsigned int)free_frames; i < (unsigned int)free_frames + free_frames_size / 8; i += 0x1000)
        identityMapPage(i, /*is_kernel*/true, /*is_writable*/true, kernel_page_directory);
    
    debug::out << "Enabling paging" << debug::endl;
    switchPageDirectory(kernel_page_directory);
    
    initHeap(base);
    
    interrupts::registerSyscallHandler(syscallGetTotal, "getTotalMemory");
    interrupts::registerSyscallHandler(syscallGetUsed, "getUsedMemory");
}
