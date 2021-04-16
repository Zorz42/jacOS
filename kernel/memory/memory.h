#include "cpu/types.h"

void* malloc(u32 size);
void free(void* ptr);

void initMemory();
unsigned int getUsedMemory();
unsigned int getTotalMemory();
unsigned int getFreeMemory();
