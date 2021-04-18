#ifndef memory_h
#define memory_h

#include "cpu/types.h"

void* malloc(u32 size);
void free(void* ptr);

void initMemory();
unsigned int getUsedMemory();
unsigned int getTotalMemory();
unsigned int getFreeMemory();

void* operator new(unsigned long size);
void* operator new[](unsigned long size);

#endif
