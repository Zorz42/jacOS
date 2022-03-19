#pragma once

namespace gfx {

#define COLOR(r, b, g) int((unsigned int)(b) + ((unsigned int)(g) << 8) + ((unsigned int)(r) << 16))

void setPixel(short x, short y, int color);
void drawRect(short x, short y, short w, short h, int color);

void swapBuffers();
int getScreenWidth();
int getScreenHeight();
void drawChar(int x, int y, char c);

}
