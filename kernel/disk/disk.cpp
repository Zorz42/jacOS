#include "disk.hpp"
#include "ports/ports.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"
#include "qemuDebug/debug.hpp"

static unsigned int sectors_count = 0;

void* disk::read(unsigned int sector, unsigned int sector_count) {
    void* result = mem::alloc(sector_count * 512);
    
    // set parameters to ports
    ports::byteOut(0x1F2, sector_count);
    ports::byteOut(0x1F3, sector);
    ports::byteOut(0x1F4, sector >> 8);
    ports::byteOut(0x1F5, sector >> 16);
    ports::byteOut(0x1F6, ((sector >> 24) & 0b1111) | 0b11100000); // bit 24 - 27 of sector index
    ports::byteOut(0x1F7, 0x20); // command - read with retry
    
    // copy data to buffer
    for(int i = 0; i < 512 * sector_count; i += 2) {
        if(i % 512 == 0)
            // wait until it finishes reading
            while((ports::byteIn(0x1F7) & 8) == 0);
        
        *(unsigned short*)((unsigned int)result + i) = ports::wordIn(0x1F0);
    }
    
    return result;
}

void disk::init() {
    unsigned int d = 0x1F0;
    unsigned int dd = 0x3F4;
    unsigned int Dev = 0;
    
    ports::byteOut(d + 6, (Dev << 4) + (1 << 6));
    
    while (ports::byteIn(d + 7) & 0x40 == 0);
    
    ports::byteOut(d + 7, 0xF8);
    while(ports::byteIn(d + 7) & 0x80 != 0);

    sectors_count = (unsigned int)ports::byteIn(d + 3);
    sectors_count += (unsigned int)ports::byteIn(d + 4) << 8;
    sectors_count += (unsigned int)ports::byteIn(d + 5) << 16;
    sectors_count += ((unsigned int)ports::byteIn(d + 6) & 0xF) << 24;
    sectors_count++;
    
    debug::out << "Size of disk in sectors is: " << sectors_count << debug::endl;
}

unsigned int disk::getDiskSize() {
    return sectors_count;
}
