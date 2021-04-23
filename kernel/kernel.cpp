#include "kernel.h"
#include "graphics/gfx.h"
#include "text/text.h"
#include "memory/memory.h"
#include "cpu/timer.h"
#include "drivers/disk/disk.h"

#define MAX_CMD_LENGTH 100
static char curr_shell_cmd[MAX_CMD_LENGTH];
static int cmd_length;

static void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    text::cout << "> ";
}

static void onCommand() {
    text::cout << &curr_shell_cmd[0] << text::endl;
}

void onKeyEvent(keyboard::Key key, bool up) {
    if(!up) {
        char c = keyToAscii(key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[++cmd_length] = 0;
            text::cout << c;
        } else if(key == keyboard::KEY_BACKSPACE && cmd_length) {
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
            text::cout << " ";
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
        } else if(key == keyboard::KEY_ENTER) {
            text::cout << "\n";
            if(curr_shell_cmd[0])
                onCommand();
            resetCommand();
        }
        text::flush();
    }
}

void kernelMain() {
    text::cout << "Kernel initialized!" << text::endl;
    text::cout << "Reading from disk..." << text::endl;
    void* result = disk::read(0, 1, 0, 1);
    text::cout << *(int*)result << text::endl;
    
    resetCommand();
    text::flush();
    
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
