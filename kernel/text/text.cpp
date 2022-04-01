#include "text.hpp"
#include "graphics/gfx.hpp"
#include "qemuDebug/debug.hpp"
#include "interrupts/interrupts.hpp"

static int cursor_x = 0, cursor_y = 0, text_width, text_height, prev_x = cursor_x, prev_y = cursor_y;
static char* text_buffer = nullptr;

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
        
        prev_x = -1; // force cursor update
    }
}

static void flushText() {
    // update cursor
    if(prev_x != cursor_x || prev_y != cursor_y) {
        updateChar(prev_x, prev_y);
        gfx::drawRect(cursor_x * 8, cursor_y * 16, 8, 16, COLOR(255, 255, 255));
        prev_x = cursor_x;
        prev_y = cursor_y;
    }
    
    gfx::swapBuffers();
}

void putChar(char character) {
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
}

unsigned int syscallTextOutString(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    const char* string = (const char*)arg1;
    if(text_buffer)
        while(*string)
            putChar(*string++);
    return 0;
}

unsigned int syscallTextOutChar(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    putChar((char)arg1);
    return 0;
}

unsigned int syscallTextFlush(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    flushText();
    return 0;
}

unsigned int syscallTextGetCursorX(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    return cursor_x;
}

unsigned int syscallTextGetCursorY(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    return cursor_y;
}

unsigned int syscallTextSetCursorPos(unsigned int arg1, unsigned int arg2, unsigned int arg3) {
    cursor_x = arg1;
    cursor_y = arg2;
    flushText();
    return 0;
}

void initText() {
    interrupts::registerSyscallHandler(&syscallTextOutString, "text::out << string");
    interrupts::registerSyscallHandler(&syscallTextOutChar, "text::out << char");
    interrupts::registerSyscallHandler(&syscallTextFlush, "text::flush");
    interrupts::registerSyscallHandler(&syscallTextGetCursorX, "text::getCursorX");
    interrupts::registerSyscallHandler(&syscallTextGetCursorY, "text::getCursorY");
    interrupts::registerSyscallHandler(&syscallTextSetCursorPos, "text::setCursorPos");
    
    text_width = gfx::getScreenWidth() / 8;
    text_height = gfx::getScreenHeight() / 16;
    
    text_buffer = new char[text_width * (text_height + 1)];
    
    debug::out << "Text module initialized!" << debug::endl << "Number of characters of screen is: " << text_width << "x" << text_height << debug::endl;
}
