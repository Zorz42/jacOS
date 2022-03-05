#pragma once
#include "types.hpp"

namespace disk {

void* read(u8 head, u16 cylinder, u8 sector, u8 sector_count);

}
