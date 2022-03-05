#include "graphics/gfx.hpp"
#include "interrupts/interrupts.hpp"
#include "memory/memory.hpp"
#include "vesa/vesa.hpp"
#include "text/text.hpp"
#include "timer/timer.hpp"
#include "keyboard/keyboard.hpp"

void kernelMain();

extern "C" void kernelEntry(void* data) {
    Interrupts::init();
    init_timer(100);
    keyboard::init();
    
    mem::init();
    gfx::init(data);
    
    kernelMain();
    
    while(true)
        asm volatile("hlt");
}
