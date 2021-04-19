#include "text.h"
#include "graphics/gfx.h"
#include "memory/memory.h"

enum printMode {MODE_DEC, MODE_HEX};

static int cursor_x = 0, cursor_y = 0, text_width, text_height, prev_x = cursor_x, prev_y = cursor_y;
static char* text_buffer = nullptr;
static printMode print_mode = MODE_DEC;

static void updateChar(int x, int y) {
    gfx::drawChar(x * 8, y * 16, text_buffer[x + y * text_width]);
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

static void printChar(char c) {
    if(c == '\n')
        newLine();
    else {
        text_buffer[cursor_x + cursor_y * text_width] = c;
        updateChar(cursor_x, cursor_y);
        cursor_x++;
        if(cursor_x >= text_width)
            newLine();
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
        gfx::drawRect(cursor_x * 8, cursor_y * 16, 8, 16, gfx::createColor(255, 255, 255));
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
            printChar(*string++);
    return *this;
}

text::_out_stream text::_out_stream::operator<<(char character) {
    if(text_buffer)
        printChar(character);
    return *this;
}

long long div(long long a, long long b) {
    long long result = 0;
    while(a >= b) {
        a -= b;
        result++;
    }
    return result;
}

long long mod(long long a, long long b) {
    while(a >= b)
        a -= b;
    return a;
}

text::_out_stream text::_out_stream::operator<<(long long number) {
    if(text_buffer)
        switch(print_mode) {
            case MODE_DEC: {
                long long x2 = number, length = 0;
                while(x2) {
                    length++;
                    x2 = div(x2, 10);
                }
                
                if(length == 0)
                    length = 1;
                
                for(int i = 0; i < length; i++) {
                    long long x3 = number;
                    for(int i2 = 0; i2 < length - i - 1; i2++)
                        x3 = div(x3, 10);
                    printChar('0' + mod(x3, 10));
                }
                break;
            }
            case MODE_HEX: {
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

text::_out_stream text::_out_stream::operator<<(long number) {
    return operator<<((long long)number);
}

text::_out_stream text::_out_stream::operator<<(int number) {
    return operator<<((long long)number);
}

text::_out_stream text::_out_stream::operator<<(unsigned long long number) {
    return  operator<<((long long)number);
}

text::_out_stream text::_out_stream::operator<<(unsigned long number) {
    return operator<<((long long)number);
}

text::_out_stream text::_out_stream::operator<<(unsigned int number) {
    return operator<<((long long)number);
}

text::_out_stream text::_out_stream::operator<<(_hex _) {
    print_mode = MODE_HEX;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(_dec _) {
    print_mode = MODE_DEC;
    return *this;
}

text::_out_stream text::_out_stream::operator<<(_endl _) {
    if(text_buffer) {
        newLine();
        flush();
    }
    print_mode = MODE_DEC;
    return *this;
}

void text::init() {
    text_width = gfx::getScreenWidth() / 8;
    text_height = gfx::getScreenHeight() / 16;
    
    text_buffer = (char*)malloc(text_width * (text_height + 1));
    
    cout << "Text module initialized!" << endl << "Number of characters of screen is: " << text_width << "x" << text_height << endl;
}
