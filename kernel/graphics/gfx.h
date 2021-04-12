void initGraphics(void* vesa_mode_info);

void setPixel(short x, short y, int color);
void drawRect(short x, short y, short w, short h, int color);
inline int createColor(unsigned char r, unsigned char g, unsigned char b) { return b + (g << 8) + (r << 16); }

void swapBuffers();
int getScreenWidth();
int getScreenHeight();
