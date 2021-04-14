#include "text.h"
#include "graphics/gfx.h"

int cursor_x = 0, cursor_y = 0;

void setChar(int x, int y, char c) {
    drawChar(x * 8, y * 16, c);
}

void print(char* string) {
    while(*string) {
        drawChar(cursor_x * 8, cursor_y * 16, *string);
        string++;
        cursor_x++;
    }
    swapBuffers();
}

void printl(char* string) {
    print(string);
    cursor_x = 0;
    cursor_y++;
}

void moveCursorTo(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void printHex(int x) {
    int end = 7;
    while(((x >> end * 4) & 0xF) == 0)
        end--;
    print("0x");
    for(int i = end; i >= 0; i--) {
        char c[2] = {' ', 0}, result = ((x >> i * 4) & 0xF);
        c[0] = result + (result < 10 ? '0' : 'A' - 10);
        print((char*)&c);
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
        char c[2] = {'0' + x3 % 10, 0};
        print((char*)&c);
    }
}
