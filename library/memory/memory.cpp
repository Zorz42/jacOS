#include <library>
#include <syscalls/__syscalls.hpp>

unsigned int getTotalMemory() {
    return __syscall(__SYSCALL_GET_TOTAL_MEMORY);
}

unsigned int getUsedMemory() {
    return __syscall(__SYSCALL_GET_USED_MEMORY);
}

unsigned int getFreeMemory() {
    return __syscall(__SYSCALL_GET_TOTAL_MEMORY) - __syscall(__SYSCALL_GET_USED_MEMORY);
}
