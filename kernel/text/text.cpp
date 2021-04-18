#include "text.h"
#include "graphics/gfx.h"
#include "memory/memory.h"

int cursor_x = 0, cursor_y = 0, text_width, text_height, prev_x = cursor_x, prev_y = cursor_y;
char* text_buffer = 0;

void updateChar(int x, int y) {
    drawChar(x * 8, y * 16, text_buffer[x + y * text_width]);
}

int getCursorX() {
    return cursor_x;
}

int getCursorY() {
    return cursor_y;
}

void updateCursor() {
    updateChar(prev_x, prev_y);
    drawRect(cursor_x * 8, cursor_y * 16, 8, 16, createColor(255, 255, 255));
    prev_x = cursor_x;
    prev_y = cursor_y;
}

void moveCursorTo(int x, int y) {
    cursor_x = x;
    cursor_y = y;
    updateCursor();
}

void flush() {
    updateCursor();
    swapBuffers();
}

void newLine() {
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
    flush();
}

void printChar(char c) {
    text_buffer[cursor_x + cursor_y * text_width] = c;
    updateChar(cursor_x, cursor_y);
    cursor_x++;
    if(cursor_x >= text_width) {
        newLine();
    }
}

void print(const char* string) {
    if(text_buffer)
        while(*string)
            printChar(*string++);
}

void printl(const char* string) {
    if(text_buffer) {
        print(string);
        newLine();
    }
}

void printHex(int x) {
    if(text_buffer) {
        int end = 7;
        while(((x >> end * 4) & 0xF) == 0 && end > 0)
            end--;
        print("0x");
        
        for(int i = end; i >= 0; i--) {
            char result = (x >> i * 4) & 0xF;
            printChar(result + (result < 10 ? '0' : 'A' - 10));
        }
    }
}

void printInt(int x) {
    if(text_buffer) {
        int x2 = x, length = 0;
        while(x2) {
            length++;
            x2 /= 10;
        }
        
        if(length == 0)
            length = 1;
        
        for(int i = 0; i < length; i++) {
            int x3 = x;
            for(int i2 = 0; i2 < length - i - 1; i2++)
                x3 /= 10;
            printChar('0' + x3 % 10);
        }
    }
}

void initText() {
    text_width = getScreenWidth() / 8;
    text_height = getScreenHeight() / 16;
    
    text_buffer = (char*)malloc(text_width * (text_height + 1));
    
    printl("Text module initialized!");
    print("Number of characters of screen is: ");
    printInt(text_width);
    print("x");
    printInt(text_height);
    printl("");
}
