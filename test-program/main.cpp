#include <lib.hpp>

#define MAX_CMD_LENGTH 100
static char curr_shell_cmd[MAX_CMD_LENGTH];
static int cmd_length;
static bool shell_running = true;

static bool strcmp(const char* a, const char* b) {
    for(int i = 0; a[i] != 0 || b[i] != 0; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

static void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    lib::out << "> ";
}

static void printSize(int bytes) {
    if(bytes < 1024)
        lib::out << bytes << "B";
    else if(bytes / 1024 < 1024)
        lib::out << bytes / 1024 << "kB";
    else if(bytes / 1024 / 1024 < 1024) {
        float megabytes = float(bytes / 1024) / 1024.f;
        int floating = (megabytes - (int)megabytes) * 100;
        lib::out << (int)megabytes << "." << floating << "MB";
    }
    else {
        float gigabytes = float(bytes / 1024 / 1024) / 1024.f;
        int floating = (gigabytes - (int)gigabytes) * 100;
        lib::out << (int)gigabytes << "." << floating << "GB";
    }
}

static void onCommand() {
    if(strcmp(&curr_shell_cmd[0], "memstat")) {
        lib::out << "Used memory: ";
        printSize(lib::getUsedMemory());
        lib::out << lib::endl;
        
        lib::out << "Free memory: ";
        printSize(lib::getFreeMemory());
        lib::out << lib::endl;
        
        lib::out << "Total memory: ";
        printSize(lib::getTotalMemory());
        lib::out << lib::endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "poweroff")) {
        shell_running = false;
    
    } else {
        lib::out << "Unknown command: " << &curr_shell_cmd[0] << lib::endl;
    }
}

static void onKeyEvent(lib::KeyEvent key_event) {
    if(!key_event.up) {
        char c = keyToAscii(key_event.key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[++cmd_length] = 0;
            lib::out << c;
        } else if(key_event.key == lib::KEY_BACKSPACE && cmd_length) {
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
            lib::moveCursorTo(lib::getCursorX() - 1, lib::getCursorY());
            lib::out << " " << lib::flush;
            lib::moveCursorTo(lib::getCursorX() - 1, lib::getCursorY());
        } else if(key_event.key == lib::KEY_ENTER) {
            lib::out << "\n";
            if(curr_shell_cmd[0])
                onCommand();
            resetCommand();
        }
        lib::out << lib::flush;
    }
}

int main() {
    resetCommand();
    lib::out << lib::flush;
    
    while(shell_running) {
        while(shell_running) {
            lib::KeyEvent key_event = lib::getKeyEvent();
            if(key_event.key != lib::KEY_UNKNOWN && !key_event.up)
                onKeyEvent(key_event);
            else
                break;
        }
        lib::sleep(20);
    }
    
    return 0;
}
