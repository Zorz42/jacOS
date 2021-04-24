#ifndef memory_h
#define memory_h

#include "cpu/types.h"

namespace mem {

void init();
unsigned int getUsed();
unsigned int getTotal();
unsigned int getFree();

}

void* malloc(u32 size);
void free(void* ptr);

void* operator new(unsigned long size);
void* operator new[](unsigned long size);

#endif
