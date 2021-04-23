#include "memory.h"
#include "text/text.h"

static unsigned int heap_base, total_memory, used_memory;

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

typedef struct {
    u32 size;
    char free;
    void* next, *prev;
} malloc_head;

void* malloc(u32 size) {
    malloc_head* head = (malloc_head*)heap_base;
    
    // find a free block
    while(head->free != HEAD_FREE || head->size <= size) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE) {
            text::cout << "Heap error: heap corruption!" << text::endl;
            while(true)
                asm volatile("hlt");
        }
        
        if(head->next == 0) {
            text::cout << "Heap error: out of heap memory!" << text::endl;
            while(true)
                asm volatile("hlt");
        }
        
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
    
    //text::cout/* << "Malloc from " << text::hex << (int)head + sizeof(malloc_head) << " to " << (int)head + sizeof(malloc_head) + head->size << "."*/ << text::endl;
    
    return (void*)((int)head + (int)sizeof(malloc_head));
}

static void mergeBlocks(malloc_head* block1, malloc_head* block2) {
    block1->size += block2->size + sizeof(malloc_head);
    block1->next = block2->next;
}

void free(void* ptr) {
    malloc_head* head = (malloc_head*)((int)ptr - sizeof(malloc_head));
    if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE) {
        text::cout << "Heap error: freeing invalid memory!" << text::endl;
        while(true)
            asm volatile("hlt");
    }
    
    if(head->free == HEAD_FREE) {
        text::cout << "Heap error: freeing already free memory!" << text::endl;
        while(true)
            asm volatile("hlt");
    }
    
    head->free = HEAD_FREE;
    used_memory -= head->size + sizeof(malloc_head);
    
    if(head->next && ((malloc_head*)head->next)->free == HEAD_FREE)
        mergeBlocks(head, (malloc_head*)head->next);
    if(head->prev && ((malloc_head*)head->prev)->free == HEAD_FREE)
        mergeBlocks((malloc_head*)head->prev, head);
    
    //text::cout << "Free " << text::hex << (int)ptr - sizeof(malloc_head) << "." << text::endl;
}

void initMemory() {
    heap_base = 0x3d5000;
    used_memory = 0;
    total_memory = 0x1000000;
    
    malloc_head* main_head = (malloc_head*)heap_base;
    main_head->free = HEAD_FREE;
    main_head->size = total_memory - sizeof(malloc_head);
    main_head->next = 0;
    main_head->prev = 0;
}

unsigned int getUsedMemory() {
    return used_memory;
}

unsigned int getTotalMemory() {
    return total_memory;
}

unsigned int getFreeMemory() {
    return total_memory - used_memory;
}

void* operator new(unsigned long size) {
    return malloc(size);
}

void* operator new[](unsigned long size) {
    return malloc(size);
}
