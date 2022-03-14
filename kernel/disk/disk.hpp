#pragma once

namespace disk {

void* read(unsigned char head, unsigned short cylinder, unsigned char sector, unsigned char sector_count);

}
