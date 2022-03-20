#pragma once

namespace disks {

class Disk {
public:
    unsigned int port_base, h, size;
    void read(unsigned int sector, unsigned int sector_count, void* ptr);
};

void init();
unsigned int getNumDisks();
Disk getDisk(unsigned int n);

}
