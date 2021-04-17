#include "mode_info.h"
#include "gfx.h"
#include "text/text.h"
#include "memory/memory.h"

void kernel_main();

#define DRAW_BUFFER_BYTES_PER_PIXEL sizeof(unsigned int)

mode_info_t* mode_info;
unsigned int* buffer = 0;
int total_pixels;
extern const unsigned char font[2048];

int createColor(unsigned char r, unsigned char g, unsigned char b) {
    return b + (g << 8) + (r << 16);
}

void setPixel(short x, short y, int color) {
    buffer[x + y * mode_info->resolutionX] = color;
}

void drawRect(short x, short y, short w, short h, int color) {
    unsigned int* temp_buffer = buffer + x + y * mode_info->resolutionX;
    int step = mode_info->resolutionX - w, y2 = y + h, x2 = x + w;
    for(; y < y2; y++) {
        for(int x_ = x; x_ < x2; x_++)
            *temp_buffer++ = color;
        temp_buffer += step;
    }
}

void swapBuffers() {
    for(int i = 0; i < total_pixels; i++) {
        unsigned int* curr_pixel = (unsigned int*)(mode_info->buffer + i * 3);
        if(*curr_pixel ^ buffer[i] & 0xFFFFFF)
            *curr_pixel = buffer[i];
    }
}

int getScreenWidth() {
    return mode_info->resolutionX;
}

int getScreenHeight() {
    return mode_info->resolutionY;
}

void drawChar(int x, int y, char c) {
    unsigned int* temp_buffer = buffer + x + y * mode_info->resolutionX;
    for(int y_ = 0; y_ < 16; y_++) {
        for(int x_ = 0; x_ < 8; x_++)
            *temp_buffer++ = createColor(255, 255, 255) * ((font[((int)c << 3) + (y_ >> 1)] >> 8 - x_) & 1);
        temp_buffer += mode_info->resolutionX - 8;
    }
}

void initGraphics(void* vesa_mode_info) {
    mode_info = vesa_mode_info;
    
    total_pixels = mode_info->resolutionX * mode_info->resolutionY;
    buffer = (unsigned int*)malloc(total_pixels * DRAW_BUFFER_BYTES_PER_PIXEL);
    
    //drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
    
    //drawRect(0, 0, mode_info->resolutionX, mode_info->resolutionY, 255);
    
    initText();
    
    printl("Initializing graphics module...");
    print("VESA buffer is at: ");
    printHex((int)mode_info->buffer);
    printl("");
    print("Screen buffer is at: ");
    printHex((int)buffer);
    printl("");
    print("Screen resulution is: ");
    printInt(mode_info->resolutionX);
    print("x");
    printInt(mode_info->resolutionY);
    printl("");
}
