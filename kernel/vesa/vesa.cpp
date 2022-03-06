#include "vesa.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"

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

static mode_info* mode_information = nullptr;
static unsigned int* buffer = nullptr;
static bool* changed_lines = nullptr;
static int total_pixels = -1;

void vesa::init() {
    mode_information = (mode_info*)0x7d00;
    total_pixels = vesa::getScreenWidth() * vesa::getScreenHeight();
    buffer = (unsigned int*)mem::alloc(getTotalPixels() * getBytesPerPixel());
    changed_lines = (bool*)mem::alloc(getScreenHeight());
}

int vesa::getScreenWidth() {
    return mode_information->resolutionX;
}

int vesa::getScreenHeight() {
    return mode_information->resolutionY;
}

int vesa::getTotalPixels() {
    return total_pixels;
}

int vesa::getBytesPerPixel() {
    return sizeof(unsigned int);
}

unsigned int* vesa::getVideoBuffer() {
    return buffer;
}

void vesa::swapBuffers() {
    unsigned char* curr_pixel = mode_information->buffer;
    int i = 0;
    for(int y = 0; y < getScreenHeight(); y++) {
        if(changed_lines[y]) {
            for(int x = 0; x < getScreenWidth(); x++) {
                *((unsigned int*)curr_pixel) = (buffer[i++] & 0xFFFFFF);
                curr_pixel += 3;
            }
            changed_lines[y] = false;
        } else {
            i += getScreenWidth();
            curr_pixel += getScreenWidth() * 3;
        }
    }
}

void vesa::lineHasChanged(int y) {
    changed_lines[y] = true;
}
