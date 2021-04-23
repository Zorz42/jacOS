#include "disk.h"
#include "drivers/ports/ports.h"
#include "text/text.h"
#include "memory/memory.h"

extern "C" void loadToAddress(void* address);

void* disk::read(u8 head, u8 sector, u16 cylinder, u8 sector_count) {
    ports::byteOut(0x1f6, (head & 0b00001111) | 0b10100000); // drive & head number
    ports::byteOut(0x1f2, sector_count); // sector count
    ports::byteOut(0x1f3, sector); // sector number
    ports::byteOut(0x1f4, cylinder & 0b00001111); // cylinder low port
    ports::byteOut(0x1f5, (cylinder >> 4) & 0b00001111); // cylinder high port
    ports::byteOut(0x1f7, 0x20); // command port - read with retry
    
    void* result = malloc(sector_count * 512);
    
    loadToAddress((void*)result);
    
    return result;
}
