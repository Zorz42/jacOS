#pragma once
#include "types.hpp"

namespace mem {

void init();
unsigned int getUsed();
unsigned int getTotal();
unsigned int getFree();

void* alloc(u32 size, bool page_align=false);
void free(void* ptr);

}
