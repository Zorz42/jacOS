#pragma once

#define __SYSCALL_GET_TOTAL_MEMORY         1
#define __SYSCALL_GET_USED_MEMORY          2
#define __SYSCALL_TEXT_OUT_STRING          3
#define __SYSCALL_TEXT_ENDL                4
#define __SYSCALL_TEXT_OUT_INT             5
#define __SYSCALL_TEXT_DEC_MODE            6
#define __SYSCALL_TEXT_HEX_MODE            7
#define __SYSCALL_TEXT_FLUSH               8
#define __SYSCALL_TEXT_GET_CURSOR_X        9
#define __SYSCALL_TEXT_GET_CURSOR_Y        10
#define __SYSCALL_TEXT_SET_CURSOR_POSITION 11
#define __SYSCALL_SLEEP                    12
#define __SYSCALL_GET_KEY_EVENT            13
#define __SYSCALL_GET_KEY_STATE            14

inline unsigned int __sysCall(unsigned int func, unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    unsigned int return_value = 0;
    asm volatile("int $0x40" : "=a"(return_value) : "a"(func), "b"(arg1), "c"(arg2), "d"(arg3));
    return return_value;
}
