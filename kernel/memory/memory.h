#include "cpu/types.h"

void* malloc(u32 size);

void initMemory();
unsigned int getUsedMemory();
unsigned int getTotalMemory();
unsigned int getFreeMemory();
