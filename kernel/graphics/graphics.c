#include "mode_info.h"
#include "gfx.h"

void kernel_main();

#define DRAW_BUFFER_BYTES_PER_PIXEL sizeof(unsigned int)

mode_info_t* mode_info;
unsigned char* buffer1;
unsigned int* buffer2;
int total_pixels;

void putpixel(int x, int y, int color) {
    buffer2[x + y * mode_info->resolutionX] = color;
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

void graphics_entry(mode_info_t* mode_info_) {
    mode_info = mode_info_;
    
    total_pixels = mode_info->resolutionX * mode_info->resolutionY;
    buffer1 = (unsigned char*)mode_info->buffer;
    buffer2 = (unsigned int*)((unsigned char*)mode_info->buffer + total_pixels * mode_info->bpp / 8);
    
    kernel_main();
}
