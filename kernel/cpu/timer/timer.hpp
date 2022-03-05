#ifndef TIMER_H
#define TIMER_H

#include "types.hpp"

void init_timer(u32 freq);
unsigned int getTicks();
void delay(unsigned int ds);

#endif
