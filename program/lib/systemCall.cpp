#include <lib.hpp>

unsigned int lib::__systemCall(unsigned int func, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    unsigned int return_value = 0;
    asm volatile("int $0x40" : "=a"(return_value) : "a"(func), "b"(arg1), "c"(arg2), "d"(arg3));
    return return_value;
}
