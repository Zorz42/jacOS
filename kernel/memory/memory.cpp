#include "memory.hpp"

static unsigned int heap_base, total_memory, used_memory;

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

struct MallocHead {
    u32 size;
    char free;
    void* next, *prev;
};

struct MemInfo {
    unsigned int base, base_high;
    unsigned int length, length_high;
    unsigned int type;
} __attribute__((packed));

void* mem::alloc(u32 size, bool page_align) {
    MallocHead* head = (MallocHead*)heap_base;
    
    // find a free block
    while(head->free != HEAD_FREE || head->size <= size) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
            asm("int $20");
        
        if(head->next == 0)
            asm("int $21");
        
        head = (MallocHead*)head->next;
    }
    
    // split that block into two blocks
    MallocHead* next_head = (MallocHead*)((int)head + (int)sizeof(MallocHead) + (int)size);
    next_head->free = HEAD_FREE;
    next_head->size = head->size - size - sizeof(MallocHead);
    next_head->next = head->next;
    next_head->prev = head;
    
    head->free = HEAD_ALLOCATED;
    head->size = size;
    head->next = next_head;
    
    used_memory += size + sizeof(MallocHead);
    
    return (void*)((int)head + (int)sizeof(MallocHead));
}

static void mergeBlocks(MallocHead* block1, MallocHead* block2) {
    block1->size += block2->size + sizeof(MallocHead);
    block1->next = block2->next;
}

void mem::free(void* ptr) {
    MallocHead* head = (MallocHead*)((int)ptr - sizeof(MallocHead));
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
