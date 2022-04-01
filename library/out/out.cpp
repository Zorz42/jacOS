#include <library>
#include <syscalls/__syscalls.hpp>

static NumberMode curr_number_mode = NumberMode::DEC;

__Out __Out::operator<<(const char* string) {
    __syscall(__SYSCALL_TEXT_OUT_STRING, (unsigned int)string);
    return *this;
}

__Out __Out::operator<<(const String& string) {
    char* char_arr = new char[string.getSize() + 1];
    for(int i = 0; i < string.getSize(); i++)
        char_arr[i] = string[i];
    char_arr[string.getSize()] = 0;
    *this << char_arr;
    delete char_arr;
    return *this;
}

__Out __Out::operator<<(char character) {
    __syscall(__SYSCALL_TEXT_OUT_CHAR, (unsigned int)character);
    return *this;
}

__Out __Out::operator<<(int number) {
    String str(number, curr_number_mode);
    *this << str;
    return *this;
}

__Out __Out::operator<<(unsigned int number) {
    *this << (int)number;
    return *this;
}

__Out __Out::operator<<(double number) {
    String str(number);
    *this << str;
    return *this;
}

__Out __Out::operator<<(float number) {
    *this << (double)number;
    return *this;
}

__Out __Out::operator<<(__Hex _) {
    curr_number_mode = NumberMode::HEX;
    return *this;
}

__Out __Out::operator<<(__Dec _) {
    curr_number_mode = NumberMode::DEC;
    return *this;
}

__Out __Out::operator<<(__Endl _) {
    curr_number_mode = NumberMode::DEC;
    *this << '\n' << flush;
    return *this;
}

__Out __Out::operator<<(__Flush _) {
    __syscall(__SYSCALL_TEXT_FLUSH);
    return *this;
}

unsigned int getCursorX() {
    return __syscall(__SYSCALL_TEXT_GET_CURSOR_X);
}

unsigned int getCursorY() {
    return __syscall(__SYSCALL_TEXT_GET_CURSOR_Y);
}

void moveCursorTo(unsigned int x, unsigned int y) {
    __syscall(__SYSCALL_TEXT_SET_CURSOR_POSITION, x, y);
}
