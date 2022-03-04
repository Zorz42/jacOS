#include "kernel.hpp"
#include "graphics/gfx.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"
#include "cpu/timer.hpp"
#include "drivers/disk/disk.hpp"

#define MAX_CMD_LENGTH 100
static char curr_shell_cmd[MAX_CMD_LENGTH];
static int cmd_length;

static bool strcmp(const char* a, const char* b) {
    for(int i = 0; a[i] != 0; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

static void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    text::cout << "> ";
}

static void onCommand() {
    if(strcmp(&curr_shell_cmd[0], "run")) {
        void* result = disk::read(0, 0, 1, 20);
        text::cout << "Running program!" << text::endl;
        typedef int (*call_module_t)(void);
        call_module_t program = (call_module_t)result;
        int exit_code = program();
        free(result);
        text::cout << "Program ended with exit code " << exit_code << "! " << text::endl;
        text::cout << *(int*)0x1000000 << text::endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "memstat")) {
        text::cout << "Used memory: " << mem::getUsed() / 1024 << " KB" << text::endl
        << "Free memory: " << mem::getFree() / 1024 << " KB" << text::endl
        << "Total memory: " << mem::getTotal() / 1024 << " KB" << text::endl;
        
    } else {
        text::cout << "Unknown command: " << &curr_shell_cmd[0] << text::endl;
    }
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
    
    resetCommand();
    text::flush();
    
    /*void* prev_alloc = 0;
    
    for(int i = 0; i < 100; i++) {
        text::cout << text::endl << i << ". time:" << text::endl;
        void* alloc1 = malloc(0x10000);
        void* alloc2 = malloc(0x50000);
        free(alloc1);
        void* alloc3 = malloc(0x20000);
        void* alloc4 = malloc(0x60000);
        free(alloc2);
        free(alloc3);
        free(alloc4);
        
        
        text::cout << "Memory change: " << (int)alloc1 - (int)prev_alloc << text::endl;
        
        prev_alloc = alloc1;
        
        text::cout << "Used memory: " << mem::getUsed() / 1024 << " KB" << text::endl;
        text::cout << "Free memory: " << mem::getFree() / 1024 << " KB" << text::endl;
        text::cout << "Total memory: " << mem::getTotal() / 1024 << " KB" << text::endl;
        
        //delay(10);
    }*/
}
