#include "text.hpp"
#include "graphics/gfx.hpp"
#include "memory/memory.hpp"
#include "qemuDebug/debug.hpp"
#include "interrupts/interrupts.hpp"

enum class PrintMode {decimal, hexadecimal};

static int cursor_x = 0, cursor_y = 0, text_width, text_height, prev_x = cursor_x, prev_y = cursor_y;
static char* text_buffer = nullptr;
static PrintMode print_mode = PrintMode::decimal;

static void updateChar(int x, int y) {
    gfx::drawChar(x << 3, y << 4, text_buffer[x + y * text_width]);
}

static void newLine() {
    if(cursor_y < text_height - 1) {
        cursor_x = 0;
        cursor_y++;
    } else {
        //scroll text
        cursor_x = 0;
        char* iter = text_buffer;
        for(int y = 0; y < text_height - 1; y++) {
            for(int x = 0; x < text_width; x++) {
                *iter = *(iter + text_width);
                updateChar(x, y);
                iter++;
            }
        }
        for(int x = 0; x < text_width; x++) {
            *iter = ' ';
            updateChar(x, text_height - 1);
            iter++;
        }
    }
}

int text::getCursorX() {
    return cursor_x;
}

int text::getCursorY() {
    return cursor_y;
}

void text::flush() {
    // update cursor
    if(prev_x != cursor_x || prev_y != cursor_y) {
        updateChar(prev_x, prev_y);
        gfx::drawRect(cursor_x * 8, cursor_y * 16, 8, 16, COLOR(255, 255, 255));
        prev_x = cursor_x;
        prev_y = cursor_y;
    }
    
    gfx::swapBuffers();
}

void text::moveCursorTo(int x, int y) {
    cursor_x = x;
    cursor_y = y;
    flush();
}

text::_out_stream text::_out_stream::operator<<(const char* string) {
    if(text_buffer)
        while(*string)
            *this << *string++;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(char character) {
    if(text_buffer) {
        if(character == '\n')
            newLine();
        else {
            text_buffer[cursor_x + cursor_y * text_width] = character;
            updateChar(cursor_x, cursor_y);
            if(++cursor_x >= text_width)
                newLine();
        }
    }
    return *this;
}

text::_out_stream text::_out_stream::operator<<(long number) {
    if(text_buffer)
        switch(print_mode) {
            case PrintMode::decimal: {
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
            case PrintMode::hexadecimal: {
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

text::_out_stream text::_out_stream::operator<<(float number) {
    *this << (long)number;
    float floating = number - (int)number;
    long floating_long = floating * FLOAT_COUT_PRECISION;
    while(floating_long != 0 && floating_long % 10 == 0)
        floating_long /= 10;
    if(floating_long != 0)
        *this << "." << floating_long;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(int number) {
    return operator<<((long)number);
}

text::_out_stream text::_out_stream::operator<<(unsigned long number) {
    return  operator<<((long)number);
}

text::_out_stream text::_out_stream::operator<<(unsigned int number) {
    return operator<<((long)number);
}

text::_out_stream text::_out_stream::operator<<(_hex _) {
    print_mode = PrintMode::hexadecimal;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(_dec _) {
    print_mode = PrintMode::decimal;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(_endl _) {
    if(text_buffer) {
        newLine();
        flush();
    }
    print_mode = PrintMode::decimal;
    return *this;
}

unsigned int syscallTextOut(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::out << (const char*)arg1;
    return 0;
}

unsigned int syscallTextEndl(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::out << text::endl;
    return 0;
}

unsigned int syscallTextInt(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::out << arg1;
    return 0;
}

unsigned int syscallTextDec(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::out << text::dec;
    return 0;
}

unsigned int syscallTextHex(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::out << text::hex;
    return 0;
}

unsigned int syscallTextFlush(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    text::flush();
    return 0;
}

void text::init() {
    interrupts::registerSyscallHandler(&syscallTextOut, "text::out");
    interrupts::registerSyscallHandler(&syscallTextEndl, "text::endl");
    interrupts::registerSyscallHandler(&syscallTextInt, "text << int");
    interrupts::registerSyscallHandler(&syscallTextDec, "text::dec");
    interrupts::registerSyscallHandler(&syscallTextHex, "text::hex");
    interrupts::registerSyscallHandler(&syscallTextFlush, "text::flush");
    
    text_width = gfx::getScreenWidth() / 8;
    text_height = gfx::getScreenHeight() / 16;
    
    text_buffer = (char*)mem::alloc(text_width * (text_height + 1));
    
    debug::out << "Text module initialized!" << debug::endl << "Number of characters of screen is: " << text_width << "x" << text_height << debug::endl;
}
