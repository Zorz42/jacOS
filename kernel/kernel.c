#include "graphics/gfx.h"

void kernelMain() {
    for(int c = 0; c < 3; c++) {
        for(int i = 0; i < 600; i++) {
            /*drawRect(20, 20, i, i, createColor(255, 100, 255));
            swapBuffers();
            
            drawRect(20, 20, i, i, createColor(100, 255, 100));
            swapBuffers();*/
            
            drawRect(0, 0, getScreenWidth(), getScreenHeight(), createColor(255, 100, 255));
            swapBuffers();
            
            drawRect(0, 0, getScreenWidth(), getScreenHeight(), createColor(100, 255, 100));
            swapBuffers();
            
            /*for(int x = 0; x < getScreenWidth(); x++)
                for(int y = 0; y < getScreenHeight(); y++)
                    putpixel(x, y, 100 + 255 * 256 + 100 * 256 * 256);
            
            swapBuffers();*/
        }
        //drawRect(20, 20, 600, 600, 0);
        //drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
    }
    
    drawRect(0, 0, getScreenWidth(), getScreenHeight(), 0);
    swapBuffers();
    
    while(1);
}
