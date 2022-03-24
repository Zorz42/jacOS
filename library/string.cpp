#include <library.hpp>

String::String(const char* string) {
    unsigned int strlen = 0;
    while(string[strlen] != 0)
        strlen++;
    
    reserve(strlen);
    for(int i = 0; i < strlen; i++)
        push(string[i]);
}

const char* String::getCharArr() {
    reserve(getSize() + 1);
    (*this)[getSize()] = 0;
    return &(*this)[0];
}
