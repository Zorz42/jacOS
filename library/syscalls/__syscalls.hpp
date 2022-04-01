#pragma once

#define __SYSCALL_SHUTDOWN                 0
#define __SYSCALL_ALLOCATE_FRAME           1
#define __SYSCALL_GET_TOTAL_MEMORY         2
#define __SYSCALL_GET_USED_MEMORY          3
#define __SYSCALL_TEXT_OUT_STRING          4
#define __SYSCALL_TEXT_OUT_CHAR            5
#define __SYSCALL_TEXT_FLUSH               6
#define __SYSCALL_TEXT_GET_CURSOR_X        7
#define __SYSCALL_TEXT_GET_CURSOR_Y        8
#define __SYSCALL_TEXT_SET_CURSOR_POSITION 9
#define __SYSCALL_SLEEP                    10
#define __SYSCALL_GET_KEY_EVENT            11
#define __SYSCALL_GET_KEY_STATE            12

inline unsigned int __syscall(unsigned int func, unsigned int arg1=0, unsigned int arg2=0, unsigned int arg3=0) {
    unsigned int return_value = 0;
    asm volatile("int $0x40" : "=a"(return_value) : "a"(func), "b"(arg1), "c"(arg2), "d"(arg3));
    return return_value;
}
