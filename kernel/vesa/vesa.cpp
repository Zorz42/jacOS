#include "vesa.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"

struct mode_info {
    unsigned short attributes;
    unsigned char windowA, windowB;
    unsigned short granularity;
    unsigned short windowSize;
    unsigned short segmentA, segmentB;
    unsigned int winFuncPtr;
    unsigned short pitch;

    unsigned short resolutionX, resolutionY;
    unsigned char wChar, yChar, planes, bpp, banks;
    unsigned char memoryModel, bankSize, imagePages;
    unsigned char reserved0;

    unsigned char readMask, redPosition;
    unsigned char greenMask, greenPosition;
    unsigned char blueMask, bluePosition;
    unsigned char reservedMask, reservedPosition;
    unsigned char directColorAttributes;

    // linear frame buffer
    unsigned char* buffer;
    unsigned int offScreenMemOff;
    unsigned short offScreenMemSize;
    //unsigned char  reserved1 [206];
};

static mode_info* mode_information = nullptr;
static unsigned int* buffer = nullptr;
static bool* changed_lines = nullptr;
static int total_pixels = -1;

void vesa::init() {
    mode_information = (mode_info*)0x7e00;
    total_pixels = vesa::getScreenWidth() * vesa::getScreenHeight();
    buffer = (unsigned int*)mem::alloc(getTotalPixels() * getBytesPerPixel());
    changed_lines = (bool*)mem::alloc(getScreenHeight());
    
    // map vesa addresses
    for(unsigned int i = (unsigned int)mode_information->buffer; i <= (unsigned int)mode_information->buffer + getTotalPixels() * getBytesPerPixel(); i += 0x1000)
        mem::identityMapPage(i, /*is_kernel*/true, /*is_writable*/true);
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
