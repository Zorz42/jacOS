#pragma once
#include <__array.hpp>

class String : public Array<char> {
public:
    String(const char* string);
    String(const String& string);
    String() = default;
    
    const char* getCharArr();
    bool operator==(const String& string) const;
    void operator=(const String& string) {
        for(int i = 0; i < string.getSize(); i++)
            push(string[i]);
    }
};
