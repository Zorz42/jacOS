#pragma once

namespace disk {

void init();
unsigned int getDiskSize();
void* read(unsigned int sector, unsigned int sector_count);

}
