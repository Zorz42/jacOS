#include "debug.hpp"
#include "ports/ports.hpp"

void debug::log(const char* string) {
    while(*string != 0)
        ports::byteOut(0x3F8, *string++);
}

void debug::logInt(int number) {
    int len = 0, num = number;
    while(num) {
        num /= 10;
        len++;
    }
    
    char str[100];
    
    int curr = len;
    str[curr] = 0;
    while(curr) {
        str[--curr] = '0' + number % 10;
        number /= 10;
    }
    
    if(len == 0)
        log("0");
    else
        log(str);
}

void debug::logHex(int number) {
    int len = 0, num = number;
    while(num) {
        num /= 16;
        len++;
    }
    
    char str[100];
    
    int curr = len;
    str[curr] = 0;
    while(curr) {
        if(number % 16 < 10)
            str[--curr] = '0' + number % 16;
        else
            str[--curr] = 'a' + number % 16 - 10;
        
        number /= 16;
    }
    
    log("0x");
    if(len == 0)
        log("0");
    else
        log(str);
}
