#include "graphics/gfx.hpp"
#include "cpu/isr/isr.hpp"
#include "drivers/memory/memory.hpp"
#include "drivers/vesa/vesa.hpp"
#include "text/text.hpp"

void kernelMain();

extern "C" void kernelEntry(void* data) {
    isr_install();
    irq_install();
    
    mem::init();
    gfx::init(data);
    
    kernelMain();
    
    while(true)
        asm volatile("hlt");
}
