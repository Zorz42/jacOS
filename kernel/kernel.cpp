#include "kernel.hpp"
#include "graphics/gfx.hpp"
#include "text/text.hpp"
#include "memory/memory.hpp"
#include "timer/timer.hpp"
#include "disk/disk.hpp"
#include "ports/ports.hpp"
#include "vesa/vesa.hpp"
#include "qemuDebug/debug.hpp"
#include "interrupts/interrupts.hpp"

#define MAX_CMD_LENGTH 100
static char curr_shell_cmd[MAX_CMD_LENGTH];
static int cmd_length;
static bool kernel_running = true;

static bool strcmp(const char* a, const char* b) {
    for(int i = 0; a[i] != 0 || b[i] != 0; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

static void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    text::out << "> ";
}

static void printSize(int bytes) {
    if(bytes < 1024)
        text::out << bytes << "B";
    else if(bytes / 1024 < 1024)
        text::out << bytes / 1024 << "kB";
    else if(bytes / 1024 / 1024 < 1024) {
        float megabytes = float(bytes / 1024) / 1024.f;
        int floating = (megabytes - (int)megabytes) * 100;
        text::out << (int)megabytes << "." << floating << "MB";
    }
    else {
        float gigabytes = float(bytes / 1024 / 1024) / 1024.f;
        int floating = (gigabytes - (int)gigabytes) * 100;
        text::out << (int)gigabytes << "." << floating << "GB";
    }
}

static void switchToUserMode() {
    asm volatile("cli");
    asm volatile("mov $0x23, %ax");
    asm volatile("mov %ax, %ds");
    asm volatile("mov %ax, %es");
    asm volatile("mov %ax, %fs");
    asm volatile("mov %ax, %gs");
    
    asm volatile("mov %esp, %eax");
    asm volatile("pushl $0x23");
    asm volatile("pushl %eax");
    asm volatile("pushf");
    
    asm volatile("pop %eax");
    asm volatile("or $0x200, %eax");
    asm volatile("push %eax");
    
    asm volatile("pushl $0x1B");
    asm volatile("push $1f");
    asm volatile("iret");
    asm volatile("1:");
}

static void onCommand() {
    if(strcmp(&curr_shell_cmd[0], "run")) {
        void* result = disk::read(0, disk::getDiskSize());
        debug::out << "Program loaded into " << debug::hex << mem::virtualToPhysicalAddress((unsigned int)result) << " and virtual address " << (unsigned int)result << debug::endl;
        typedef int (*call_module_t)(void);
        call_module_t program = (call_module_t)result;
        
        int exit_code = program();
        
        mem::free(result);
        text::out << "Program ended with exit code " << exit_code << "! " << text::endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "memstat")) {
        text::out << "Used memory: ";
        printSize(mem::getUsed());
        text::out << text::endl;
        
        text::out << "Free memory: ";
        printSize(mem::getFree());
        text::out << text::endl;
        
        text::out << "Total memory: ";
        printSize(mem::getTotal());
        text::out << text::endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "sleep")) {
        timer::delay(1000);
    
    } else if(strcmp(&curr_shell_cmd[0], "poweroff")) {
        kernel_running = false;
    
    } else {
        text::out << "Unknown command: " << &curr_shell_cmd[0] << text::endl;
    }
}

static void onKeyEvent(keyboard::Key key, bool up) {
    if(!up) {
        char c = keyToAscii(key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[++cmd_length] = 0;
            text::out << c;
        } else if(key == keyboard::KEY_BACKSPACE && cmd_length) {
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
            text::out << " ";
            text::flush();
            text::moveCursorTo(text::getCursorX() - 1, text::getCursorY());
        } else if(key == keyboard::KEY_ENTER) {
            text::out << "\n";
            if(curr_shell_cmd[0])
                onCommand();
            resetCommand();
        }
        text::flush();
    }
}

void kernelMain() {
    switchToUserMode();
    
    text::out << "JacOS has started" << text::endl;
    debug::out << "Kernel initialized" << debug::endl;
    
    resetCommand();
    text::flush();
    
    while(kernel_running) {
        while(keyboard::hasKeyEvent() && kernel_running) {
            keyboard::KeyEvent event = keyboard::getKeyEvent();
            onKeyEvent(event.key, event.up);
        }
        timer::delay(20);
    }
}
