#pragma once
#include <array/__array.hpp>

enum class NumberMode { DEC, HEX };

class String : public Array<char> {
public:
    String(const char* string);
    String(const String& string);
    String(int number, NumberMode mode=NumberMode::DEC);
    String(double number);
    String() = default;
    
    const char* getCharArr();
    bool operator==(const String& string) const;
    String operator+(const String& string) const;
    void operator+=(const String& string);
};
