#include "kernel.hpp"
#include "graphics/gfx.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"
#include "timer/timer.hpp"
#include "disk/disk.hpp"
#include "ports/ports.hpp"

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

void printSize(int bytes) {
    if(bytes < 1024)
        text::cout << bytes << "B";
    else if(bytes / 1024 < 1024)
        text::cout << bytes / 1024 << "kB";
    else if(bytes / 1024 / 1024 < 1024) {
        float megabytes = float(bytes / 1024) / 1024.f;
        int floating = (megabytes - (int)megabytes) * 100;
        text::cout << (int)megabytes << "." << floating << "MB";
    }
    else {
        float gigabytes = float(bytes / 1024 / 1024) / 1024.f;
        int floating = (gigabytes - (int)gigabytes) * 100;
        text::cout << (int)gigabytes << "." << floating << "GB";
    }
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
        text::cout << "Used memory: ";
        printSize(mem::getUsed());
        text::cout << text::endl;
        
        text::cout << "Free memory: ";
        printSize(mem::getFree());
        text::cout << text::endl;
        
        text::cout << "Total memory: ";
        printSize(mem::getTotal());
        text::cout << text::endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "sleep")) {
        timer::delay(1000);
    
    } else {
        text::cout << "Unknown command: " << &curr_shell_cmd[0] << text::endl;
    }
}

static void onKeyEvent(keyboard::Key key, bool up) {
    if(!up) {
        char c = keyToAscii(key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[++cmd_length] = 0;
            text::cout << c;
        } else if(key == keyboard::KEY_BACKSPACE && cmd_length) {
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
            text::cout << " ";
            text::flush();
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
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
    text::cout << "Kernel initialized!" << 55.123f << text::endl;
    
    resetCommand();
    text::flush();
    
    while(true) {
        while(keyboard::hasKeyEvent()) {
            keyboard::KeyEvent event = keyboard::getKeyEvent();
            onKeyEvent(event.key, event.up);
        }
        timer::delay(20);
    }
}
