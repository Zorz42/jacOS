#pragma once

#include <__array.hpp>

class String : public Array<char> {
public:
    String(const char* string);
    const char* getCharArr();
};
