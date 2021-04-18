#include "graphics/gfx.h"
#include "cpu/isr.h"
#include "memory/memory.h"

void kernelMain();

extern "C" void kernelEntry(void* data) {
    isr_install();
    irq_install();
    
    initMemory();
    initGraphics(data);

    kernelMain();
    
    while(true)
        asm volatile("hlt");
}
