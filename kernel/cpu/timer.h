#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_timer(u32 freq);
unsigned int getTicks();
void delay(unsigned int ds);

#endif
