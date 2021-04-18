#include "kernel.h"
#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "cpu/timer.h"

#define MAX_CMD_LENGTH 100
char curr_shell_cmd[MAX_CMD_LENGTH];
int cmd_length;

void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    print("> ");
    flush();
}

void onCommand() {
    printl(&curr_shell_cmd[0]);
}

void onKeyEvent(Key key, bool up) {
    if(!up) {
        char c = keyToAscii(key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[cmd_length + 1] = 0;
            cmd_length++;
            
            printChar(c);
        } else if(key == KEY_BACKSPACE && cmd_length) {
            moveCursorTo(getCursorX() - 1, getCursorY());
            printChar(' ');
            moveCursorTo(getCursorX() - 1, getCursorY());
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
        } else if(key == KEY_ENTER) {
            printl("");
            onCommand();
            resetCommand();
        }
        flush();
    }
}

void kernelMain() {
    printl("Kernel initialized!");
    resetCommand();
    
    /*void* prev_alloc = 0;
    
    for(int i = 0; i < 100; i++) {
        printl("");
        printInt(i);
        printl(". time:");
        void* alloc1 = malloc(0x10000);
        void* alloc2 = malloc(0x50000);
        free(alloc1);
        void* alloc3 = malloc(0x20000);
        void* alloc4 = malloc(0x60000);
        free(alloc2);
        free(alloc3);
        free(alloc4);
        
        print("Memory change: ");
        printHex((int)alloc1 - (int)prev_alloc);
        printl("");
        
        prev_alloc = alloc1;
        
        print("Used memory: ");
        printInt(getUsedMemory() / 1024);
        printl(" KB");
        
        print("Free memory: ");
        printInt(getFreeMemory() / 1024);
        printl(" KB");
        
        print("Total memory: ");
        printInt(getTotalMemory() / 1024);
        printl(" KB");
        
        //delay(10);
    }*/
}
