#include "disk.h"
#include "drivers/ports/ports.h"
#include "text/text.h"

extern "C" void ata_chs_read();

void disk::read() {
    ports::byteOut(0x1f6, 0b10100000); // drive & head number
    ports::byteOut(0x1f2, 1); // sector count
    ports::byteOut(0x1f3, 1); // sector number
    ports::byteOut(0x1f4, 0); // cylinder low port
    ports::byteOut(0x1f5, 0); // cylinder high port
    ports::byteOut(0x1f7, 0x20); // command port - read with retry
    
    ata_chs_read();
}
