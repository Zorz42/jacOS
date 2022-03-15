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

void timer::init(unsigned int freq) {
    interrupts::registerIrqHandler(IRQ0, timerCallback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    unsigned int divisor = 1193180 / freq;
    unsigned char low  = (unsigned char)(divisor & 0xFF);
    unsigned char high = (unsigned char)( (divisor >> 8) & 0xFF);
    
    ports::byteOut(0x43, 0x36);
    ports::byteOut(0x40, low);
    ports::byteOut(0x40, high);
}

