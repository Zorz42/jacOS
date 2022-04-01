#include <library>
extern int main();

extern "C" int start() {
    initHeap(0xF00000);
    
    return main();
}
