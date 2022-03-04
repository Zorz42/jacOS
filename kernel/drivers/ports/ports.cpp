#include "ports.hpp"

/**
 * Read a byte from the specified port
 */
u8 ports::byteIn(u16 port) {
    u8 result;
    /* Inline assembler syntax
     * !! Notice how the source and destination registers are switched from NASM !!
     *
     * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
     * '"d" (port)': map the C variable '(port)' into e'd'x register
     *
     * Inputs and outputs are separated by colons
     */
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void ports::byteOut(u16 port, u8 data) {
    /* Notice how here both registers are mapped to C variables and
     * nothing is returned, thus, no equals '=' in the asm syntax 
     * However we see a comma since there are two variables in the input area
     * and none in the 'return' area
     */
    asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

u16 ports::wordIn(u16 port) {
    u16 result;
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void ports::wordOut(u16 port, u16 data) {
    asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}
