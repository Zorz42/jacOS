#pragma once
#include "types.hpp"

namespace ports {

unsigned char byteIn(u16 port);
void byteOut(u16 port, u8 data);
unsigned short wordIn(u16 port);
void wordOut(u16 port, u16 data);

}
