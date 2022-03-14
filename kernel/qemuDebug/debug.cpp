#include "debug.hpp"
#include "ports/ports.hpp"
#ifdef DEBUG_ENABLED

enum class DebugPrintMode {decimal, hexadecimal};

static DebugPrintMode debug_print_mode = DebugPrintMode::decimal;
static DebugLevel curr_debug_level = DEBUG_INFO;
static bool has_written_in_line = false;

debug::_out_stream debug::_out_stream::operator<<(const char* string) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    while(*string)
        *this << *string++;
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(char character) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    if(!has_written_in_line) {
        has_written_in_line = true;
        switch(curr_debug_level) {
            case DEBUG_INFO:
                *this << "[INFO] ";
                break;
            case DEBUG_WARNING:
                *this << "[WARN] ";
                break;
            case DEBUG_ERROR:
                *this << "[ERROR] ";
                break;
        }
    }
    if(character == '\n')
        has_written_in_line = false;
    
    ports::byteOut(0x3F8, character);
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(long number) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    switch(debug_print_mode) {
        case DebugPrintMode::decimal: {
            if(number < 0) {
                *this << '-';
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
                *this << char('0' + x3 % 10);
            }
            break;
        }
        case DebugPrintMode::hexadecimal: {
            int end = 7;
            while(((number >> end * 4) & 0xF) == 0 && end > 0)
                end--;
            *this << "0x";
            
            for(int i = end; i >= 0; i--) {
                char result = (number >> i * 4) & 0xF;
                *this << char(result + (result < 10 ? '0' : 'A' - 10));
            }
            break;
        }
    }
    return *this;
}

#define FLOAT_COUT_PRECISION 10000

debug::_out_stream debug::_out_stream::operator<<(float number) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    *this << (long)number;
    float floating = number - (int)number;
    long floating_long = floating * FLOAT_COUT_PRECISION;
    while(floating_long != 0 && floating_long % 10 == 0)
        floating_long /= 10;
    if(floating_long != 0)
        *this << "." << floating_long;
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(int number) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    return *this << (long)number;
}

debug::_out_stream debug::_out_stream::operator<<(unsigned long number) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    return *this << (long)number;
}

debug::_out_stream debug::_out_stream::operator<<(unsigned int number) {
    if(curr_debug_level < DEBUG_LEVEL)
        return *this;
    
    return *this << (long)number;
}

debug::_out_stream debug::_out_stream::operator<<(_hex _) {
    debug_print_mode = DebugPrintMode::hexadecimal;
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(_dec _) {
    debug_print_mode = DebugPrintMode::decimal;
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(_endl _) {
    *this << '\n';
    debug_print_mode = DebugPrintMode::decimal;
    curr_debug_level = DEBUG_INFO;
    return *this;
}

debug::_out_stream debug::_out_stream::operator<<(DebugLevel level) {
    curr_debug_level = level;
    return *this;
}

#endif
