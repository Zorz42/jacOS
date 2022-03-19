#include <lib.hpp>

#define SYSCALL_GET_TOTAL_MEMORY 1
#define SYSCALL_GET_USED_MEMORY 2

unsigned int lib::getTotalMemory() {
    return __systemCall(SYSCALL_GET_TOTAL_MEMORY);
}

unsigned int lib::getUsedMemory() {
    return __systemCall(SYSCALL_GET_USED_MEMORY);
}

unsigned int lib::getFreeMemory() {
    return getTotalMemory() - getUsedMemory();
}
