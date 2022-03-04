#include "graphics/gfx.hpp"
#include "cpu/isr.hpp"
#include "memory/memory.hpp"
#include "drivers/vesa/vesa.hpp"
#include "text/text.hpp"

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
