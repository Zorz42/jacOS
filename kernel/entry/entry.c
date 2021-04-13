#include "graphics/gfx.h"
#include "cpu/isr.h"

void kernelMain();

void kernelEntry(void* data) {
    isr_install();
    irq_install();
    initGraphics(data);
    
    kernelMain();
}
