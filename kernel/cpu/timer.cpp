#include "timer.hpp"
#include "isr.hpp"
#include "drivers/ports/ports.hpp"
#include "text/text.hpp"

static unsigned int tick = 0;

unsigned int getTicks() {
    return tick;
}

void delay(unsigned int ds) {
    unsigned int start = getTicks();
    while(getTicks() < start + ds)
        // waits for next interrupt and does not rapidly loop and use cycles
        asm("hlt");
}

static void timer_callback(registers_t regs) {
    tick++;
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    ports::byteOut(0x43, 0x36); /* Command port */
    ports::byteOut(0x40, low);
    ports::byteOut(0x40, high);
}

