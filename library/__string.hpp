#pragma once
#include <__array.hpp>

class String : public Array<char> {
public:
    String(const char* string);
    String(const String& string);
    String() = default;
    
    const char* getCharArr();
    bool operator==(const String& string) const;
    String operator+(const String& string) const;
};
