#include "graphics/gfx.h"

void kernel_main() {
    for(int i = 0; i < 600; i++) {
        for(int x = 0; x < i; x++)
            for(int y = 0; y < i; y++)
                putpixel(x, y, 255 + 100 * 256 + 255 * 256 * 256);
        
        swapBuffers();
        
        /*for(int x = 0; x < getScreenWidth(); x++)
            for(int y = 0; y < getScreenHeight(); y++)
                putpixel(x, y, 100 + 255 * 256 + 100 * 256 * 256);
        
        swapBuffers();*/
    }
    
    for(int x = 0; x < getScreenWidth(); x++)
        for(int y = 0; y < getScreenHeight(); y++)
            putpixel(x, y, 0);
    
    swapBuffers();
    
    while(1);
}
