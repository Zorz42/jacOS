#ifndef kernel_h
#define kernel_h

#include "drivers/keyboard/keyboard.hpp"

void onKeyEvent(keyboard::Key key, bool up);
void kernelMain();

#endif /* kernel_h */
