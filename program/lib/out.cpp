#include <lib.hpp>

#define SYSCALL_OUT_TEXT 1
#define SYSCALL_OUT_ENDL 2
#define SYSCALL_OUT_NUM 3
#define SYSCALL_OUT_DEC 4
#define SYSCALL_OUT_HEX 5
#define SYSCALL_OUT_FLUSH 6

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
