#include "../graphics/gfx.h"

void kernelMain();

void kernelEntry(void* data) {
    initGraphics(data);
    
    kernelMain();
}
