#include "mode_info.h"
#include "gfx.h"
#include "text/text.h"

void kernel_main();

#define DRAW_BUFFER_BYTES_PER_PIXEL sizeof(unsigned int)

mode_info_t* mode_info;
unsigned char* buffer1;
unsigned int* buffer2;
int total_pixels;
extern const unsigned char font[2048];

void setPixel(short x, short y, int color) {
    buffer2[x + y * mode_info->resolutionX] = color;
}

void drawRect(short x, short y, short w, short h, int color) {
    unsigned int* buffer = buffer2 + x + y * mode_info->resolutionX;
    int step = mode_info->resolutionX - w, y2 = y + h, x2 = x + w;
    for(; y < y2; y++) {
        for(int x_ = x; x_ < x2; x_++)
            *buffer++ = color;
        buffer += step;
    }
}

void swapBuffers() {
    for(int i = 0; i < total_pixels; i++) {
        unsigned int* curr_pixel = (unsigned int*)(buffer1 + i * 3);
        if(*curr_pixel ^ buffer2[i] & 0xFFFFFF)
            *curr_pixel = buffer2[i];
    }
}

int getScreenWidth() {
    return mode_info->resolutionX;
}

int getScreenHeight() {
    return mode_info->resolutionY;
}

void drawChar(int x, int y, char c) {
    for(int x_ = 0; x_ < 8; x_++)
        for(int y_ = 0; y_ < 16; y_++)
            setPixel(x + x_, y + y_, createColor(255, 255, 255) * ((font[((int)c << 3) + (y_ >> 1)] >> 8 - x_) & 1));
}

void initGraphics(void* vesa_mode_info) {
    mode_info = vesa_mode_info;
    
    total_pixels = mode_info->resolutionX * mode_info->resolutionY;
    buffer1 = (unsigned char*)mode_info->buffer;
    buffer2 = (unsigned int*)((unsigned char*)mode_info->buffer + total_pixels * mode_info->bpp / 8 + 4 /* main buffer might slightly overwrite first pixel and its not noticable, but its not nice */);

    printl("Initializing graphics module...");
    print("Screen buffer is at: ");
    printHex((int)buffer1);
    printl("");
    print("Screen resulution is: ");
    printInt(mode_info->resolutionX);
    print("x");
    printInt(mode_info->resolutionY);
    printl("");
}
