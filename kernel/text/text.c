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


