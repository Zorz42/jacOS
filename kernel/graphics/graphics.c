#include "mode_info.h"
#include "gfx.h"

void kernel_main();

mode_info_t* mode_info;
unsigned char* buffer1;
unsigned char* buffer2;
unsigned long long* buffer1_long;
unsigned long long* buffer2_long;
int bytes_per_pixel;
int pixels_in_bytes;

void putpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    unsigned where = x * bytes_per_pixel + y * mode_info->resolutionX * bytes_per_pixel;
    buffer2[where] = b;
    buffer2[where + 1] = g;
    buffer2[where + 2] = r;
}

void swapBuffers() {
    for(int i = 0; i < pixels_in_bytes; i++)
        buffer1_long[i] = buffer2_long[i];
}

int getScreenWidth() {
    return mode_info->resolutionX;
}

int getScreenHeight() {
    return mode_info->resolutionY;
}

void graphics_entry(mode_info_t* mode_info_) {
    mode_info = mode_info_;
    
    buffer1 = (unsigned char*)mode_info->buffer;
    buffer2 = (unsigned char*)mode_info->buffer + mode_info->resolutionX * mode_info->resolutionY * mode_info->bpp / 8;
    buffer1_long = (unsigned long*) buffer1;
    buffer2_long = (unsigned long*) buffer2;
    bytes_per_pixel = mode_info->bpp / 8;
    pixels_in_bytes = mode_info->resolutionX * mode_info->resolutionY * bytes_per_pixel / 8;
    
    kernel_main();
}
