#include <library>

#define MAX_CMD_LENGTH 100
static char curr_shell_cmd[MAX_CMD_LENGTH];
static int cmd_length;
static bool shell_running = true;

void *operator new(unsigned long size) {
    return nullptr;
}

void *operator new[](unsigned long size) {
    return nullptr;
}

void operator delete(void* ptr, unsigned long _) {
    
}


static bool strcmp(const char* a, const char* b) {
    for(int i = 0; a[i] != 0 || b[i] != 0; i++)
        if(a[i] != b[i])
            return false;
    return true;
}

static void resetCommand() {
    cmd_length = 0;
    curr_shell_cmd[0] = 0;
    out << "> ";
}

static void printSize(int bytes) {
    if(bytes < 1024)
        out << bytes << "B";
    else if(bytes / 1024 < 1024)
        out << bytes / 1024 << "kB";
    else if(bytes / 1024 / 1024 < 1024) {
        float megabytes = float(bytes / 1024) / 1024.f;
        int floating = (megabytes - (int)megabytes) * 100;
        out << (int)megabytes << "." << floating << "MB";
    }
    else {
        float gigabytes = float(bytes / 1024 / 1024) / 1024.f;
        int floating = (gigabytes - (int)gigabytes) * 100;
        out << (int)gigabytes << "." << floating << "GB";
    }
}

static void onCommand() {
    if(strcmp(&curr_shell_cmd[0], "memstat")) {
        out << "Used memory: ";
        printSize(getUsedMemory());
        out << endl;
        
        out << "Free memory: ";
        printSize(getFreeMemory());
        out << endl;
        
        out << "Total memory: ";
        printSize(getTotalMemory());
        out << endl;
        
    } else if(strcmp(&curr_shell_cmd[0], "poweroff")) {
        shell_running = false;
    
    } else {
        out << "Unknown command: " << &curr_shell_cmd[0] << endl;
    }
}

static void onKeyEvent(KeyEvent key_event) {
    if(!key_event.up) {
        char c = keyToAscii(key_event.key);
        if(c) {
            curr_shell_cmd[cmd_length] = c;
            curr_shell_cmd[++cmd_length] = 0;
            out << c;
        } else if(key_event.key == KEY_BACKSPACE && cmd_length) {
            cmd_length--;
            curr_shell_cmd[cmd_length] = 0;
            moveCursorTo(getCursorX() - 1, getCursorY());
            out << " " << flush;
            moveCursorTo(getCursorX() - 1, getCursorY());
        } else if(key_event.key == KEY_ENTER) {
            out << "\n";
            if(curr_shell_cmd[0])
                onCommand();
            resetCommand();
        }
        out << flush;
    }
}

int main() {
    resetCommand();
    out << flush;
    
    while(shell_running) {
        while(shell_running) {
            KeyEvent key_event = getKeyEvent();
            if(key_event.key != KEY_UNKNOWN && !key_event.up)
                onKeyEvent(key_event);
            else
                break;
        }
        //lib::sleep(20);
    }
    
    return 0;
}
