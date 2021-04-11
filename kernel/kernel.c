#include "mode_info.h"

mode_info_t* mode_info;

static void putpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned char* screen = (unsigned char*)mode_info->buffer;
    unsigned where = x * mode_info->bpp / 8 + y * mode_info->resolutionX * mode_info->bpp / 8;
    screen[where] = b;
    screen[where + 1] = g;
    screen[where + 2] = r;
}

void main(mode_info_t* mode_info_) {
    mode_info = mode_info_;
    for(int x = 0; x < mode_info->resolutionX; x++)
        for(int y = 0; y < mode_info->resolutionY; y++)
            putpixel(x, y, 255, 100, 255);
    
    while(1);
}
