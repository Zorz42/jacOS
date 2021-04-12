#include "graphics/gfx.h"

void kernel_main() {
    for(int c = 0; c < 3; c++) {
        for(int i = 0; i < 600; i++) {
            drawRect(0, 0, i, i, 255 + 100 * 256 + 255 * 256 * 256);
            swapBuffers();
            
            drawRect(0, 0, i, i, 100 + 255 * 256 + 100 * 256 * 256);
            swapBuffers();
            
            /*for(int x = 0; x < getScreenWidth(); x++)
                for(int y = 0; y < getScreenHeight(); y++)
                    putpixel(x, y, 100 + 255 * 256 + 100 * 256 * 256);
            
            swapBuffers();*/
        }
        drawRect(0, 0, 600, 600, 0);
    }
    
    drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
    swapBuffers();
    
    while(1);
}
