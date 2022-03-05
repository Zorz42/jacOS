#include "gfx.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"
#include "font.hpp"
#include "vesa/vesa.hpp"
#include "kernel.hpp"

void gfx::setPixel(short x, short y, int color) {
    vesa::lineHasChanged(y);
    vesa::getVideoBuffer()[x + y * vesa::getScreenWidth()] = color;
}

void gfx::drawRect(short x, short y, short w, short h, int color) {
    unsigned int* temp_buffer = vesa::getVideoBuffer() + x + y * vesa::getScreenWidth();
    int step = vesa::getScreenWidth() - w, y2 = y + h, x2 = x + w;
    for(; y < y2; y++) {
        vesa::lineHasChanged(y);
        for(int x_ = x; x_ < x2; x_++)
            *temp_buffer++ = color;
        temp_buffer += step;
    }
}

void gfx::swapBuffers() {
    vesa::swapBuffers();
}

int gfx::getScreenWidth() {
    return vesa::getScreenWidth();
}

int gfx::getScreenHeight() {
    return vesa::getScreenHeight();
}

void gfx::drawChar(int x, int y, char c) {
    unsigned int* temp_buffer = vesa::getVideoBuffer() + x + y * vesa::getScreenWidth();
    for(int y_ = 0; y_ < 16; y_++) {
        vesa::lineHasChanged(y + y_);
        for(int x_ = 0; x_ < 8; x_++)
            *temp_buffer++ = (font[((int)c << 3) + (y_ >> 1)] >> 8 - x_) & 1 ? COLOR(255, 255, 255) : 0;
        temp_buffer += vesa::getScreenWidth() - 8;
    }
}

void gfx::init(void* data) {
    vesa::init(data);
    text::init();
    
    text::cout << "Initializing graphics module..." << text::endl << "VESA buffer is at: " << text::hex << (int)vesa::getVideoBuffer << text::endl << "Screen buffer is at: " << vesa::getScreenWidth() << "x" << vesa::getScreenHeight() << text::endl;
}
