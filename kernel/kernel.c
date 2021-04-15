#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"

int test[1000000];

void kernelMain() {
    printl("Kernel initialized!");
    
    //for(int i = 0; i < 200; i++) // for testing speed
        //print("A");
    
    for(int i = 0; i < 100; i++) {
        malloc(200);
        malloc(0x200);
        
        print("Used memory: ");
        printInt(getUsedMemory() / 1024);
        printl(" KB");
    }
}
