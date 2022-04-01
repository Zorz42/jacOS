#include <library>

__Out __Out::operator<<(const char* string) {
    return *this;
}

__Out __Out::operator<<(const String& string) {
    return *this;
}

__Out __Out::operator<<(char character) {
    return *this;
}

__Out __Out::operator<<(int number) {
    return *this;
}

__Out __Out::operator<<(float number) {
    return *this;
}

__Out __Out::operator<<(__Hex _) {
    return *this;
}

__Out __Out::operator<<(__Dec _) {
    return *this;
}

__Out __Out::operator<<(__Endl _) {
    return *this;
}

__Out __Out::operator<<(__Flush _) {
    return *this;
}
