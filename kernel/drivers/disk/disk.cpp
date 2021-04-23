#include "disk.h"
#include "drivers/ports/ports.h"
#include "text/text.h"
#include "memory/memory.h"
#include "graphics/gfx.h"


void* disk::read(u8 head, u16 cylinder, u8 sector, u8 sector_count) {
    ports::byteOut(0x1f6, (head & 0b00001111) | 0b10100000); // drive & head number
    ports::byteOut(0x1f2, sector_count); // sector count
    ports::byteOut(0x1f3, sector); // sector number
    ports::byteOut(0x1f4, cylinder & 0b00001111); // cylinder low port
    ports::byteOut(0x1f5, (cylinder >> 4) & 0b00001111); // cylinder high port
    ports::byteOut(0x1f7, 0x20); // command port - read with retry
    
    void* result = malloc(sector_count * 512);
    
    asm volatile("mov %0, %%ch" :: "r"(sector) : "%ch");
    asm volatile("mov %0, %%edi" :: "r"(result) : "%edi");

    asm volatile("still_going:");
    asm volatile("in %dx, %al");
    asm volatile("test $8, %al");
    asm volatile("jz still_going");
    
    //asm volatile("mov $256, %eax");
    //asm volatile("xor %bx, %bx");
    //asm volatile("mov %ch, %bl");
    //asm volatile("mul %bx");
    //asm volatile("mov %eax, %ecx");
    asm volatile("mov $0x1f0, %edx");
    asm volatile("rep insw");
    
    return result;
}
