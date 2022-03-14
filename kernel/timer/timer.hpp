#pragma once

namespace timer {

void init(unsigned int freq);
unsigned int getTicks();
void delay(unsigned int ms);

}
