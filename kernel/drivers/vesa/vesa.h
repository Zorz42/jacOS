//
//  vesa.hpp
//  jacOS
//
//  Created by Jakob Zorz on 18/04/2021.
//

#ifndef vesa_h
#define vesa_h

#include "cpu/types.h"

namespace vesa {

void init(void* info);
int getScreenWidth();
int getScreenHeight();
int getTotalPixels();
int getBytesPerPixel();

unsigned int* getVideoBuffer();
void swapBuffers();

}

#endif /* vesa_h */
