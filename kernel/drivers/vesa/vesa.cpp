#include "vesa.h"
#include "text/text.h"
#include "memory/memory.h"

struct mode_info {
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
};

mode_info* mode_information = nullptr;
static unsigned int* buffer = 0;

void vesa::init(void* info) {
    mode_information = (mode_info*)info;
    buffer = (unsigned int*)malloc(getTotalPixels() * getBytesPerPixel());
}

int vesa::getScreenWidth() {
    return mode_information->resolutionX;
}

int vesa::getScreenHeight() {
    return mode_information->resolutionY;
}

int vesa::getTotalPixels() {
    return vesa::getScreenWidth() * vesa::getScreenHeight();
}

int vesa::getBytesPerPixel() {
    return sizeof(unsigned int);
}

unsigned int* vesa::getVideoBuffer() {
    return buffer;
}

void vesa::swapBuffers() {
    for(int i = 0; i < getTotalPixels(); i++) {
        unsigned int* curr_pixel = (unsigned int*)(mode_information->buffer + i * 3);
        if(*curr_pixel ^ buffer[i] & 0xFFFFFF)
            *curr_pixel = (buffer[i] & 0xFFFFFF) | (*curr_pixel & 0xFF000000);
    }
}
