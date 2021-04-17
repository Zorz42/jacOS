#include "kernel.h"
#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "cpu/timer.h"

void onKeyEvent(Key key, char up) {
    printl("Key pressed!");
}

void kernelMain() {
    printl("Kernel initialized!");
    void* prev_alloc = 0;
    
    for(int i = 0; i < 1; i++) {
        printl("");
        printInt(i);
        printl(". time:");
        void* alloc1 = malloc(0x10000);
        void* alloc2 = malloc(0x50000);
        free(alloc1);
        void* alloc3 = malloc(0x20000);
        void* alloc4 = malloc(0x60000);
        free(alloc2);
        free(alloc3);
        free(alloc4);
        
        print("Memory change: ");
        printHex((int)alloc1 - (int)prev_alloc);
        printl("");
        
        prev_alloc = alloc1;
        
        print("Used memory: ");
        printInt(getUsedMemory() / 1024);
        printl(" KB");
        
        print("Free memory: ");
        printInt(getFreeMemory() / 1024);
        printl(" KB");
        
        print("Total memory: ");
        printInt(getTotalMemory() / 1024);
        printl(" KB");
        
        //delay(10);
    }
}
