#ifndef mode_info_h
#define mode_info_h

#include "cpu/types.h"

typedef struct mode_info_block {
    u16 attributes;
    u8 windowA, windowB;
    u16 granularity;
    u16 windowSize;
    u16 segmentA, segmentB;
    u32 winFuncPtr;
    u16 pitch;

    u16 resolutionX, resolutionY;
    u8 wChar, yChar, planes, bpp, banks;
    u8 memoryModel, bankSize, imagePages;
    u8 reserved0;

    u8 readMask, redPosition;
    u8 greenMask, greenPosition;
    u8 blueMask, bluePosition;
    u8 reservedMask, reservedPosition;
    u8 directColorAttributes;

    // linear frame buffer
    unsigned char* buffer;
    u32 offScreenMemOff;
    u16 offScreenMemSize;
    //u8  reserved1 [206];
} mode_info_t;

#endif
