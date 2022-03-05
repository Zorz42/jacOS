#include "ports.hpp"

u8 ports::byteIn(u16 port) {
    u8 result;
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void ports::byteOut(u16 port, u8 data) {
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
