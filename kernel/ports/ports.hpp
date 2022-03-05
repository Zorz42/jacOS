#pragma once
#include "types.hpp"

namespace ports {

u8 byteIn(u16 port);
void byteOut(u16 port, u8 data);
u16 wordIn(u16 port);
void wordOut(u16 port, u16 data);

}
