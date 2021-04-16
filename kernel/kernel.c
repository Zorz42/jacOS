#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "cpu/timer.h"

void kernelMain() {
    printl("Kernel initialized!");
    
    for(int i = 0; i < 100; i++) {
        malloc(0x100 - 12);
        
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
