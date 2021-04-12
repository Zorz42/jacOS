#include "graphics/gfx.h"

void kernel_main() {
    for(int i = 0; i < 30; i++) {
        for(int x = 0; x < getScreenWidth() - 20; x++)
            for(int y = 0; y < getScreenHeight() - 20; y++)
                putpixel(x, y, 255, 100, 255);
        
        swapBuffers();
        
        for(int x = 0; x < getScreenWidth() - 20; x++)
            for(int y = 0; y < getScreenHeight() - 20; y++)
                putpixel(x, y, 100, 255, 100);
        
        swapBuffers();
        
        //swapBuffers(255);
        //swapBuffers(0);
    }
    
    while(1);
}
