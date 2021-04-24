#include "graphics/gfx.h"
#include "cpu/isr.h"
#include "memory/memory.h"
#include "drivers/vesa/vesa.h"
#include "text/text.h"

void kernelMain();

extern "C" void kernelEntry() {
    void* data = 0;
    asm volatile("mov %%ebx, %0" : "=r"(data) :: "%ebx"); // we get vesa info address in ebx register
    
    isr_install();
    irq_install();
    
    mem::init();
    gfx::init(data);
    
    kernelMain();
    
    while(true)
        asm volatile("hlt");
}
