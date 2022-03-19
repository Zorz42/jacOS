#include "keyboard.hpp"
#include "ports/ports.hpp"
#include "interrupts/interrupts.hpp"

#define EVENT_QUEUE_SIZE 100

static unsigned char key_events_queue[EVENT_QUEUE_SIZE];
static bool key_states[0x80];
int queue_top = 0, queue_bottom = 0;

static void keyboardCallback(Registers regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    unsigned char scancode = ports::byteIn(0x60);
    key_events_queue[queue_top] = scancode;
    if(scancode > 0x80)
        key_states[scancode - 0x80] = false;
    else
        key_states[scancode] = true;
    queue_top++;
    if(queue_top == EVENT_QUEUE_SIZE)
        queue_top = 0;
}

bool hasKeyEvent() {
    return queue_top != queue_bottom;
}

unsigned int syscallGetKeyEvent(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    if(!hasKeyEvent())
        return 0;
    unsigned char result = key_events_queue[queue_bottom];
    key_events_queue[queue_bottom] = 0;
    queue_bottom++;
    if(queue_bottom == EVENT_QUEUE_SIZE)
        queue_bottom = 0;
    return result;
}

unsigned int syscallGetKeyState(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    return key_states[arg1];
}

void keyboard::init() {
    interrupts::registerIrqHandler(IRQ1, keyboardCallback);
    interrupts::registerSyscallHandler(&syscallGetKeyEvent, "getKeyEvent");
    interrupts::registerSyscallHandler(&syscallGetKeyState, "getKeyState");
}
