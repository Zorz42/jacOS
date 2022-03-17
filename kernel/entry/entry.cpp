#include "graphics/gfx.hpp"
#include "interrupts/interrupts.hpp"
#include "memory/memory.hpp"
#include "vesa/vesa.hpp"
#include "text/text.hpp"
#include "timer/timer.hpp"
#include "keyboard/keyboard.hpp"
#include "ports/ports.hpp"
#include "qemuDebug/debug.hpp"
#include "disk/disk.hpp"
#include "gdt/gdt.hpp"

void kernelMain();

extern "C" void kernelEntry() {
    gdt::init();
    interrupts::init();
    
    mem::init();
    gfx::init();
    
    timer::init(1000);
    keyboard::init();
    
    disk::init();
    
    kernelMain();
    
    ports::wordOut(0x604, 0x2000);
}
