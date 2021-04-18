namespace gfx {

void init();

void setPixel(short x, short y, int color);
void drawRect(short x, short y, short w, short h, int color);
int createColor(unsigned char r, unsigned char g, unsigned char b);

void swapBuffers();
int getScreenWidth();
int getScreenHeight();
void drawChar(int x, int y, char c);

}
