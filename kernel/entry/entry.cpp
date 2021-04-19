#include "graphics/gfx.h"
#include "cpu/isr.h"
#include "memory/memory.h"
#include "drivers/vesa/vesa.h"

void kernelMain();

extern "C" void kernelEntry(void* data) {
    isr_install();
    irq_install();
    
    initMemory();
    gfx::init(data);
    
    kernelMain();
    
    while(true)
        asm volatile("hlt");
}
