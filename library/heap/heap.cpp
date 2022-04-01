//#include "qemuDebug/debug.hpp"
#include "memory/memory.hpp"
#include <library>
#include <syscalls/__syscalls.hpp>

#define HEAD_ALLOCATED 'A' // something random which is not likely to spontaneously show up in random memory
#define HEAD_FREE 'F'

struct MallocHead {
    unsigned int size;
    char free;
    MallocHead *next, *prev;
    MallocHead* getNext() { if(next != nullptr) return (MallocHead*)((unsigned int)this + (unsigned int)sizeof(MallocHead) + (unsigned int)size); else return nullptr; }
};

static unsigned int heap_base;

void* alloc(unsigned int size) {
    MallocHead* head = (MallocHead*)heap_base;
    // find a free block
    while(true) {
        if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE) {
            //debug::out << DEBUG_ERROR << "Heap corruption" << debug::endl;
            asm("int $0x13");
        }
        
        if(head->free == HEAD_FREE && head->size >= size)
            break;
        
        if(head->next == nullptr) {
            for(int i = 0; i <= size; i += 0x1000) {
                unsigned int address = (unsigned int)head + sizeof(MallocHead) + head->size;
                
                __syscall(__SYSCALL_ALLOCATE_FRAME, address);
                
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
                } else {
                    //debug::out << DEBUG_ERROR << "Heap corruption" << debug::endl;
                    asm("int $0x13");
                }
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

void free(void* ptr) {
    MallocHead* head = (MallocHead*)((unsigned int)ptr - sizeof(MallocHead));
    if(head->free != HEAD_ALLOCATED && head->free != HEAD_FREE) {
        //debug::out << DEBUG_ERROR << "Heap corruption" << debug::endl;
        asm("int $0x13");
    }
    
    if(head->free == HEAD_FREE) {
        //debug::out << DEBUG_ERROR << "Freeing freed memory" << debug::endl;
        asm("int $0x16");
    }
    
    head->free = HEAD_FREE;
    
    if(head->next != nullptr && head->next->free == HEAD_FREE)
        mergeBlocks(head, (MallocHead*)head->next);
    
    if(head->prev != nullptr && head->prev->free == HEAD_FREE)
        mergeBlocks((MallocHead*)head->prev, head);
}

void *operator new(unsigned long size) {
    return alloc(size);
}

void *operator new[](unsigned long size) {
    return alloc(size);
}

void operator delete(void* ptr, unsigned long _) {
    free(ptr);
}

void initHeap(unsigned int base) {
    heap_base = base;
    heap_base = (heap_base - 1) / 0x1000 * 0x1000 + 0x1000;
    
    __syscall(__SYSCALL_ALLOCATE_FRAME, heap_base);
    
    MallocHead* main_head = (MallocHead*)heap_base;
    main_head->free = HEAD_FREE;
    main_head->size = 0x1000 - sizeof(MallocHead); // make it one frame long
    main_head->next = nullptr;
    main_head->prev = nullptr;
}
