#include "text.h"
#include "graphics/gfx.h"
#include "memory/memory.h"

int cursor_x = 0, cursor_y = 0, text_width, text_height;
char* text_buffer = 0;

void moveCursorTo(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void printChar(char c) {
    text_buffer[cursor_x + cursor_y * text_width] = c;
    drawChar(cursor_x * 8, cursor_y * 16, c);
    cursor_x++;
    if(cursor_x >= text_width) {
        cursor_x = 0;
        cursor_y++;
    }
}

void print(char* string) {
    while(*string)
        printChar(*string++);
}

void printl(char* string) {
    print(string);
    cursor_x = 0;
    cursor_y++;
    swapBuffers();
}

void printHex(int x) {
    int end = 7;
    while(((x >> end * 4) & 0xF) == 0)
        end--;
    print("0x");
    for(int i = end; i >= 0; i--) {
        char result = (x >> i * 4) & 0xF;
        printChar(result + (result < 10 ? '0' : 'A' - 10));
    }
}

void printInt(int x) {
    int x2 = x, length = 0;
    while(x2) {
        length++;
        x2 /= 10;
    }
    
    for(int i = 0; i < length; i++) {
        int x3 = x;
        for(int i2 = 0; i2 < length - i - 1; i2++)
            x3 /= 10;
        printChar('0' + x3 % 10);
    }
}

void initText() {
    text_width = getScreenWidth() / 8;
    text_height = getScreenHeight() / 16;
    
    text_buffer = (char*)malloc(text_width * text_height);
    
    printl("Text module initialized!");
    print("Number of characters of screen is: ");
    printInt(text_width);
    print("x");
    printInt(text_height);
    printl("");
}
