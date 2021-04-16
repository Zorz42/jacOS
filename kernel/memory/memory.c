#include "memory.h"
#include "text/text.h"

unsigned int heap_base, total_memory, used_memory;

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
            printl("Heap error: heap corruption!");
            while(1)
                asm("hlt");
        }
        
        if(head->next == 0) {
            printl("Heap error: out of heap memory!");
            while(1)
                asm("hlt");
        }
        
        head = head->next;
    }
    
    // split that block into two blocks
    malloc_head* next_head = (void*)head + sizeof(malloc_head) + size;
    next_head->free = HEAD_FREE;
    next_head->size = head->size - size - sizeof(malloc_head);
    next_head->next = head->next;
    next_head->prev = head;
    
    head->free = HEAD_ALLOCATED;
    head->size = size;
    head->next = next_head;
    
    used_memory += size + sizeof(malloc_head);
    
    print("Malloc from ");
    printHex((int)head + sizeof(malloc_head));
    print(" to ");
    printHex((int)head + sizeof(malloc_head) + head->size);
    printl(".");
    
    return (void*)head + sizeof(malloc_head);
}

void mergeBlocks(malloc_head* block1, malloc_head* block2) {
    block1->size += block2->size + sizeof(malloc_head);
    block1->next = block2->next;
}

void free(void* ptr) {
    malloc_head* head = ptr - sizeof(malloc_head);
    if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE) {
        printl("Heap error: freeing invalid memory!");
        while(1)
            asm("hlt");
    }
    
    if(head->free == HEAD_FREE) {
        printl("Heap error: freeing already free memory!");
        while(1)
            asm("hlt");
    }
    
    head->free = HEAD_FREE;
    used_memory -= head->size + sizeof(malloc_head);
    
    if(head->next && ((malloc_head*)head->next)->free == HEAD_FREE)
        mergeBlocks(head, head->next);
    if(head->prev && ((malloc_head*)head->prev)->free == HEAD_FREE)
        mergeBlocks(head->prev, head);
    
    print("Free ");
    printHex((int)(ptr - sizeof(malloc_head)));
    printl(".");
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
    /*print("Memory initialized with heap at: ");
    printHex(curr_free_mem);
    printl("");*/
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
