#include "memory.hpp"

static unsigned int heap_base, total_memory, used_memory;

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

struct malloc_head {
    u32 size;
    char free;
    void* next, *prev;
};

void* malloc(u32 size) {
    malloc_head* head = (malloc_head*)heap_base;
    
    // find a free block
    while(head->free != HEAD_FREE || head->size <= size) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
            asm("int $20");
        
        if(head->next == 0)
            asm("int $21");
        
        head = (malloc_head*)head->next;
    }
    
    // split that block into two blocks
    malloc_head* next_head = (malloc_head*)((int)head + (int)sizeof(malloc_head) + (int)size);
    next_head->free = HEAD_FREE;
    next_head->size = head->size - size - sizeof(malloc_head);
    next_head->next = head->next;
    next_head->prev = head;
    
    head->free = HEAD_ALLOCATED;
    head->size = size;
    head->next = next_head;
    
    used_memory += size + sizeof(malloc_head);
    
    return (void*)((int)head + (int)sizeof(malloc_head));
}

static void mergeBlocks(malloc_head* block1, malloc_head* block2) {
    block1->size += block2->size + sizeof(malloc_head);
    block1->next = block2->next;
}

void free(void* ptr) {
    malloc_head* head = (malloc_head*)((int)ptr - sizeof(malloc_head));
    if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE)
        asm("int $22");
    
    if(head->free == HEAD_FREE)
        asm("int $23");
    
    head->free = HEAD_FREE;
    used_memory -= head->size + sizeof(malloc_head);
    
    if(head->next && ((malloc_head*)head->next)->free == HEAD_FREE)
        mergeBlocks(head, (malloc_head*)head->next);
    if(head->prev && ((malloc_head*)head->prev)->free == HEAD_FREE)
        mergeBlocks((malloc_head*)head->prev, head);
}

void mem::init() {
    heap_base = 0x3d5000;
    used_memory = 0;
    total_memory = 0x1000000;
    
    malloc_head* main_head = (malloc_head*)heap_base;
    main_head->free = HEAD_FREE;
    main_head->size = total_memory - sizeof(malloc_head);
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
