#include "disk.hpp"
#include "ports/ports.hpp"
#include "memory/memory.hpp"
#include "qemuDebug/debug.hpp"
#include "text/text.hpp"
#include "timer/timer.hpp"

#define ATA_DATA          0
#define ATA_ERROR         1
#define ATA_SECTORCOUNT   2
#define ATA_SECTORNUMBER1 3
#define ATA_SECTORNUMBER2 4
#define ATA_SECTORNUMBER3 5
#define ATA_DRIVEHEAD     6
#define ATA_STATUS        7
#define ATA_COMMAND       8

disks::Disk disks_arr[10];
unsigned int disk_count = 0;

unsigned int disks::getNumDisks() {
    return disk_count;
}

disks::Disk disks::getDisk(unsigned int n) {
    return disks_arr[n];
}

void disks::Disk::read(unsigned int sector, unsigned int sector_count, void* ptr) {
    // set parameters to ports
    ports::byteOut(port_base + ATA_SECTORCOUNT, sector_count);
    ports::byteOut(port_base + ATA_SECTORNUMBER1, sector);
    ports::byteOut(port_base + ATA_SECTORNUMBER2, sector >> 8);
    ports::byteOut(port_base + ATA_SECTORNUMBER3, sector >> 16);
    ports::byteOut(port_base + ATA_DRIVEHEAD, ((sector >> 24) & 0b1111) | 0b11100000 | (h << 4)); // bit 24 - 27 of sector index
    ports::byteOut(port_base + ATA_STATUS, 0x20); // command - read with retry
    
    // copy data to buffer
    for(int i = 0; i < 512 * sector_count; i += 2) {
        if(i % 512 == 0) {
            // wait until it finishes reading
            while((ports::byteIn(port_base + ATA_STATUS) & (1 << 7)) != 0 || (ports::byteIn(port_base + ATA_STATUS) & (1 << 3)) == 0)
                if((ports::byteIn(port_base + ATA_STATUS) & (1 << 0)) != 0 || (ports::byteIn(port_base + ATA_STATUS) & (1 << 5)) != 0) {
                    text::out << "Drive read error on byte " << i << " sector " << sector << " sector count " << sector_count << text::endl;
                    while(true);
                }
        }
        
        *(unsigned short*)((unsigned int)ptr + i) = ports::wordIn(port_base + ATA_DATA);
    }
}

void disks::Disk::write(unsigned int sector, unsigned int sector_count, void* ptr) {
    // set parameters to ports
    ports::byteOut(port_base + ATA_SECTORCOUNT, sector_count);
    ports::byteOut(port_base + ATA_SECTORNUMBER1, sector);
    ports::byteOut(port_base + ATA_SECTORNUMBER2, sector >> 8);
    ports::byteOut(port_base + ATA_SECTORNUMBER3, sector >> 16);
    ports::byteOut(port_base + ATA_DRIVEHEAD, ((sector >> 24) & 0b1111) | 0b11100000 | (h << 4)); // bit 24 - 27 of sector index
    ports::byteOut(port_base + ATA_STATUS, 0x30); // command - write
    
    //while((ports::byteIn(port_base + ATA_STATUS) & 8) == 0);
    
    // copy data to buffer
    for(int i = 0; i < 512 * sector_count; i += 2) {
        if(i % 512 == 0) {
            // wait until it finishes writing
            while((ports::byteIn(port_base + ATA_STATUS) & (1 << 7)) != 0 || (ports::byteIn(port_base + ATA_STATUS) & (1 << 3)) == 0)
                if((ports::byteIn(port_base + ATA_STATUS) & (1 << 0)) != 0 || (ports::byteIn(port_base + ATA_STATUS) & (1 << 5)) != 0) {
                    text::out << "Drive write error on byte " << i << " sector " << sector << " sector count " << sector_count << text::endl;
                    while(true);
                }
        }
        
        ports::wordOut(port_base + ATA_DATA, *(unsigned short*)((unsigned int)ptr + i));
    }
    
    ports::byteOut(port_base + ATA_STATUS, 0xE7); // command - flush write cache
    
    while((ports::byteIn(port_base + ATA_STATUS) & (1 << 7)) != 0)
        if((ports::byteIn(port_base + ATA_STATUS) & (1 << 0)) != 0 || (ports::byteIn(port_base + ATA_STATUS) & (1 << 5)) != 0) {
            text::out << "Drive cache flush error" << text::endl;
            while(true);
        }
}

void disks::init() {
    static unsigned int bases[8] = {0x1f0, 0x3f0, 0x170, 0x370, 0x1e8, 0x3e0, 0x168, 0x360};
    
    for(int i = 0; i < 8; i++)
        for(int h = 0; h < 2; h++) {
            ports::byteOut(bases[i] + ATA_DRIVEHEAD, (0xa0 | (h << 4)));

            if(((ports::byteIn(bases[i] + ATA_STATUS) & (0x40 | 0x10 | 0x20 | 0x01)) == 0x50)) {
                disks::Disk disk;
                disk.port_base = bases[i];
                disk.h = h;
                
                ports::byteOut(bases[i] + 6, (h << 4) + (1 << 6));
                
                while(ports::byteIn(bases[i] + 7) & 0x40 == 0);
                
                ports::byteOut(bases[i] + 7, 0xF8);
                
                unsigned int started_waiting = timer::getTicks();
                bool timed_out = false;
                while(ports::byteIn(bases[i] + 7) & 0x80 != 0)
                    if(timer::getTicks() >= started_waiting + 2) {
                        timed_out = true;
                        break;
                    }
                
                if(!timed_out) {
                    unsigned int sectors_size;
                    sectors_size = (unsigned int)ports::byteIn(bases[i] + 3);
                    sectors_size += (unsigned int)ports::byteIn(bases[i] + 4) << 8;
                    sectors_size += (unsigned int)ports::byteIn(bases[i] + 5) << 16;
                    sectors_size += ((unsigned int)ports::byteIn(bases[i] + 6) & 0xF) << 24;
                    sectors_size++;
                    
                    disk.size = sectors_size;
                    disks_arr[disk_count] = disk;
                    disk_count++;
                }
            }
        }
}
