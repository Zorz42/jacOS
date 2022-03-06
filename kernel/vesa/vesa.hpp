#pragma once
#include "types.hpp"

namespace vesa {

void init();
int getScreenWidth();
int getScreenHeight();
int getTotalPixels();
int getBytesPerPixel();

void lineHasChanged(int y);

unsigned int* getVideoBuffer();
void swapBuffers();

}
