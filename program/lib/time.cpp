#include <lib.hpp>

#define SYSCALL_SLEEP 12

void lib::sleep(unsigned int ms) {
    __systemCall(SYSCALL_SLEEP, ms);
}
