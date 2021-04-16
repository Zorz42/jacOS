#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "cpu/timer.h"

void kernelMain() {
    printl("Kernel initialized!");
    void* prev_alloc = 0;
    
    for(int i = 0; i < 100; i++) {
        void* alloc1 = malloc(0x10000);
        void* alloc2 = malloc(0x50000);
        free(alloc1);
        void* alloc3 = malloc(0x20000);
        void* alloc4 = malloc(0x60000);
        free(alloc2);
        free(alloc3);
        free(alloc4);
        
        print("Memory change: ");
        printHex(alloc1 - prev_alloc);
        printl("");
        
        prev_alloc = alloc1;
        
        /*print("Used memory: ");
        printInt(getUsedMemory() / 1024);
        printl(" KB");
        
        print("Free memory: ");
        printInt(getFreeMemory() / 1024);
        printl(" KB");
        
        print("Total memory: ");
        printInt(getTotalMemory() / 1024);
        printl(" KB");*/
    }
}
