#include <lib.hpp>


__attribute__((optimize("O0"))) unsigned int lib::__systemCall(unsigned int func, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    unsigned int return_value = 0;
    asm volatile("pusha");
    asm volatile("" : : "a"(func), "b"(arg1), "c"(arg2), "d"(arg3));
    asm volatile("int $0x40");
    asm volatile("" : "=a"(return_value) : :);
    asm volatile("popa");
    return return_value;
}
