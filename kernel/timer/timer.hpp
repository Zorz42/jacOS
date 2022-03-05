#pragma once
#include "types.hpp"

namespace timer {

void init(u32 freq);
unsigned int getTicks();
void delay(unsigned int ms);

}
