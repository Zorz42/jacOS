#include "memory.h"
#include "text/text.h"

u32 curr_free_mem, heap_base;

extern char* text_buffer;

void* malloc(u32 size) {
    curr_free_mem += size;
    if(text_buffer) {
        print("Malloc from ");
        printHex(curr_free_mem - size);
        print(" to ");
        printHex(curr_free_mem);
        printl(".");
    }
    return (void*)(curr_free_mem - size);
}

void initMemory() {
    heap_base = 0x100000;
    curr_free_mem = heap_base;
    /*print("Memory initialized with heap at: ");
    printHex(curr_free_mem);
    printl("");*/
}

u32 getUsedMemory() {
    return curr_free_mem - heap_base;
}
