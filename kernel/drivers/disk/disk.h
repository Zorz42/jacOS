#ifndef disk_h
#define disk_h

#include "cpu/types.h"

namespace disk {

void* read(u8 head, u8 sector, u16 cylinder, u8 sector_count);

}

#endif /* disk_h */
