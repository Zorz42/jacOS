#include "gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "font.h"
#include "drivers/vesa/vesa.h"
#include "kernel.h"

int createColor(unsigned char r, unsigned char g, unsigned char b) {
    return b + (g << 8) + (r << 16);
}

void setPixel(short x, short y, int color) {
    vesa::getVideoBuffer()[x + y * vesa::getScreenWidth()] = color;
}

void drawRect(short x, short y, short w, short h, int color) {
    unsigned int* temp_buffer = vesa::getVideoBuffer() + x + y * vesa::getScreenWidth();
    int step = vesa::getScreenWidth() - w, y2 = y + h, x2 = x + w;
    for(; y < y2; y++) {
        for(int x_ = x; x_ < x2; x_++)
            *temp_buffer++ = color;
        temp_buffer += step;
    }
}

void swapBuffers() {
    vesa::swapBuffers();
}

int getScreenWidth() {
    return vesa::getScreenWidth();
}

int getScreenHeight() {
    return vesa::getScreenHeight();
}

void drawChar(int x, int y, char c) {
    unsigned int* temp_buffer = vesa::getVideoBuffer() + x + y * vesa::getScreenWidth();
    for(int y_ = 0; y_ < 16; y_++) {
        for(int x_ = 0; x_ < 8; x_++)
            *temp_buffer++ = createColor(255, 255, 255) * ((font[((int)c << 3) + (y_ >> 1)] >> 8 - x_) & 1);
        temp_buffer += vesa::getScreenWidth() - 8;
    }
}

void initGraphics() {
    //drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
    
    //drawRect(0, 0, mode_info->resolutionX, mode_info->resolutionY, 255);
    
    initText();
    
    printl("Initializing graphics module...");
    print("VESA buffer is at: ");
    printHex((int)vesa::getVideoBuffer());
    printl("");
    print("Screen buffer is at: ");
    printHex((int)vesa::getVideoBuffer());
    printl("");
    print("Screen resulution is: ");
    printInt(vesa::getScreenWidth());
    print("x");
    printInt(vesa::getScreenHeight());
    printl("");
}
