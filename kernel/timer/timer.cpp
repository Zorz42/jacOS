#include "timer/timer.hpp"
#include "interrupts/interrupts.hpp"
#include "ports/ports.hpp"
#include "text/text.hpp"

static unsigned int tick = 0;

unsigned int timer::getTicks() {
    return tick;
}

void timer::delay(unsigned int ms) {
    unsigned int start = getTicks();
    while(getTicks() < start + ms)
        // waits for next interrupt and does not rapidly loop and use cycles
        asm("hlt");
}

static void timerCallback(Registers regs) {
    tick++;
}

void timer::init(u32 freq) {
    interrupts::registerHandler(IRQ0, timerCallback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    
    ports::byteOut(0x43, 0x36);
    ports::byteOut(0x40, low);
    ports::byteOut(0x40, high);
}

