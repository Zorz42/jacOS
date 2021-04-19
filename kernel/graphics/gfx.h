namespace gfx {

#define COLOR(r, b, g) int((int)(b) + ((int)(g) << 8) + ((int)(r) << 16))

void init(void* data);

void setPixel(short x, short y, int color);
void drawRect(short x, short y, short w, short h, int color);

void swapBuffers();
int getScreenWidth();
int getScreenHeight();
void drawChar(int x, int y, char c);

}
