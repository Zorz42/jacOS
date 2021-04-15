#include "memory.h"
#include "text/text.h"

u32 curr_free_mem, heap_base;

#define HEAD_ALLOCATED 0
#define HEAD_FREE 1

typedef struct {
    u32 size;
    char free;
} malloc_head;

void* malloc(u32 size) {
    malloc_head* head = (malloc_head*)curr_free_mem;
    head->size = size;
    curr_free_mem += size + sizeof(malloc_head);
    
    print("Malloc from ");
    printHex(curr_free_mem - size);
    print(" to ");
    printHex(curr_free_mem);
    printl(".");
    
    return (void*)(curr_free_mem - size);
}

void initMemory() {
    heap_base = 0x3d5000;
    curr_free_mem = heap_base;
    /*print("Memory initialized with heap at: ");
    printHex(curr_free_mem);
    printl("");*/
}

u32 getUsedMemory() {
    return curr_free_mem - heap_base;
}
