#include "graphics/gfx.hpp"
#include "interrupts/interrupts.hpp"
#include "memory/memory.hpp"
#include "vesa/vesa.hpp"
#include "text/text.hpp"
#include "timer/timer.hpp"
#include "keyboard/keyboard.hpp"
#include "ports/ports.hpp"

void kernelMain();

extern "C" void kernelEntry(void* data) {
    interrupts::init();
    timer::init(1000);
    keyboard::init();
    
    mem::init();
    gfx::init(data);
    
    kernelMain();
    
    ports::wordOut(0x604, 0x2000);
}
