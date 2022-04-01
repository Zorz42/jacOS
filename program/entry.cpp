#include <library>
#include <syscalls/__syscalls.hpp>
extern int main();

extern "C" int start() {
    initHeap(0xF00000);
    
    //return main();
    main();
    
    asm volatile("mov $0, %eax");
    asm volatile("int $0x40");
    return 0;
}
