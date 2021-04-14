#include "graphics/gfx.h"
#include "text/text.h"

void kernelMain() {
    printl("Kernel loaded!");
    for(int i = 0; i < 200; i++)
        print("A");
    printl("test");
    printl("test");
}
