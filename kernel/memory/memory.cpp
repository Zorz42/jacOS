#include "memory.hpp"

static unsigned int heap_base, total_memory, used_memory;

void* malloc(u32 size) {
    void* result = (void*)(heap_base + used_memory);
    used_memory += size;
    return result;
}

void mem::init() {
    heap_base = 0x3d5000;
    used_memory = 0;
    total_memory = 0x1000000;
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
