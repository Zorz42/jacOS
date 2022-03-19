#include <lib.hpp>

#define SYSCALL_OUT_TEXT 3
#define SYSCALL_OUT_ENDL 4
#define SYSCALL_OUT_NUM 5
#define SYSCALL_OUT_DEC 6
#define SYSCALL_OUT_HEX 7
#define SYSCALL_OUT_FLUSH 8
#define SYSCALL_GET_CURSOR_X 9
#define SYSCALL_GET_CURSOR_Y 10
#define SYSCALL_SET_CURSOR_POS 11

lib::__out lib::__out::operator<<(const char* text) {
    lib::__systemCall(SYSCALL_OUT_TEXT, (unsigned int)text);
    return *this;
}

lib::__out lib::__out::operator<<(char c) {
    char str[2];
    str[0] = c;
    str[1] = 0;
    lib::__systemCall(SYSCALL_OUT_TEXT, (unsigned int)&str);
    return *this;
}

lib::__out lib::__out::operator<<(unsigned int number) {
    lib::__systemCall(SYSCALL_OUT_NUM, number);
    return *this;
}

lib::__out lib::__out::operator<<(int number) {
    *this << (unsigned int)number;
    return *this;
}

lib::__out lib::__out::operator<<(__hex _) {
    lib::__systemCall(SYSCALL_OUT_HEX);
    return *this;
}

lib::__out lib::__out::operator<<(__dec _) {
    lib::__systemCall(SYSCALL_OUT_DEC);
    return *this;
}

lib::__out lib::__out::operator<<(__endl _) {
    lib::__systemCall(SYSCALL_OUT_ENDL);
    return *this;
}

lib::__out lib::__out::operator<<(__flush _) {
    lib::__systemCall(SYSCALL_OUT_FLUSH);
    return *this;
}

unsigned int lib::getCursorX() {
    return __systemCall(SYSCALL_GET_CURSOR_X);
}

unsigned int lib::getCursorY() {
    return __systemCall(SYSCALL_GET_CURSOR_Y);
}

void lib::moveCursorTo(unsigned int x, unsigned int y) {
    __systemCall(SYSCALL_SET_CURSOR_POS, x, y);
}
