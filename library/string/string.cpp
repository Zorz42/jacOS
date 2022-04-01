#include <library>

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

String::String(int number, NumberMode mode) {
    switch(mode) {
        case NumberMode::DEC: {
            
            if(number < 0) {
                push('-');
                number *= -1;
            }
            long x2 = number, length = 0;
            while(x2) {
                length++;
                x2 = x2 / 10;
            }
            
            if(length == 0)
                length = 1;
            
            for(int i = 0; i < length; i++) {
                long x3 = number;
                for(int i2 = 0; i2 < length - i - 1; i2++)
                    x3 = x3 / 10;
                push(char('0' + x3 % 10));
            }
            break;
        }
        case NumberMode::HEX: {
            int end = 7;
            while(((number >> end * 4) & 0xF) == 0 && end > 0)
                end--;
            insert((String)"0x", 0);
            
            for(int i = end; i >= 0; i--) {
                char result = (number >> i * 4) & 0xF;
                push(char(result + (result < 10 ? '0' : 'A' - 10)));
            }
            break;
        }
    }
}

#define FLOAT_PRECISION 10000

String::String(double number) {
    *this = (int)number;
    float floating = number - (int)number;
    int floating_int = floating * FLOAT_PRECISION;
    while(floating_int != 0 && floating_int % 10 == 0)
        floating_int /= 10;
    if(floating_int != 0) {
        push('.');
        *this += floating_int;
    }
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

String String::operator+(const String& string) const {
    String result = *this;
    result.insert(string, result.getSize());
    return result;
}

void String::operator+=(const String& string) {
    insert(string, getSize());
}
