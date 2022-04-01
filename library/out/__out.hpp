#pragma once
#include <string/__string.hpp>

struct __Hex {};
struct __Dec {};
struct __Endl {};
struct __Flush {};

class __Out {
public:
    __Out operator<<(const char* string);
    __Out operator<<(const String& string);
    __Out operator<<(char character);
    __Out operator<<(int number);
    __Out operator<<(unsigned int number);
    __Out operator<<(double number);
    __Out operator<<(float number);
    __Out operator<<(__Hex _);
    __Out operator<<(__Dec _);
    __Out operator<<(__Endl _);
    __Out operator<<(__Flush _);
};

inline __Hex hex;
inline __Dec dec;
inline __Endl endl;
inline __Flush flush;
inline __Out out;

unsigned int getCursorX();
unsigned int getCursorY();
void moveCursorTo(unsigned int x, unsigned int y);
