#include "graphics/gfx.h"
#include "cpu/isr.h"
#include "memory/memory.h"

void kernelMain();

void kernelEntry(void* data) {
    isr_install();
    irq_install();
    initMemory();
    initGraphics(data);
    
    kernelMain();
    
    while(1)
        asm ("hlt");
}
