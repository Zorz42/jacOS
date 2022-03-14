#include "ports.hpp"

unsigned char ports::byteIn(unsigned short port) {
    unsigned char result;
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void ports::byteOut(unsigned short port, unsigned char data) {
    asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned short ports::wordIn(unsigned short port) {
    unsigned short result;
    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void ports::wordOut(unsigned short port, unsigned short data) {
    asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port));
}
