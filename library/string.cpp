#include <library.hpp>

String::String(const char* string) {
    unsigned int strlen = 0;
    while(string[strlen] != 0)
        strlen++;
    
    reserve(strlen);
    for(int i = 0; i < strlen; i++)
        push(string[i]);
}

String::String(const String& string) {
    *this = string;
}

const char* String::getCharArr() {
    reserve(getSize() + 1);
    (*this)[getSize()] = 0;
    return &(*this)[0];
}

bool String::operator==(const String& string) const {
    if(string.getSize() != getSize())
        return false;
    
    for(int i = 0; i < getSize(); i++)
        if(string[i] != (*this)[i])
            return false;
    
    return true;
}
