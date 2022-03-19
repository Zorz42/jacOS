#include <lib.hpp>

#define SYSCALL_GET_KEY_EVENT 12
#define SYSCALL_GET_KEY_STATE 13

static lib::Key scancodeToKey(unsigned char scancode);

lib::KeyEvent lib::getKeyEvent() {
    unsigned char scancode = lib::__systemCall(SYSCALL_GET_KEY_EVENT);
    KeyEvent event;
    if(scancode == 0) {
        event.key = KEY_UNKNOWN;
        return event;
    }
    
    event.up = false;
    if(scancode > 0x80) {
        event.up = true;
        scancode -= 0x80;
    }
    
    event.key = (Key)scancode;
    return event;
}

const char* top_chars = "qwertyuiop";
const char* mid_chars = "asdfghjkl";
const char* low_chars = "zxcvbnm";

char lib::keyToAscii(Key key) {
    if(key >= KEY_Q && key <= KEY_P)
        return top_chars[key - KEY_Q] + (getKeyState(KEY_LSHIFT) || getKeyState(KEY_RSHIFT) ? 'A' - 'a' : 0);
    
    if(key >= KEY_A && key <= KEY_L)
        return mid_chars[key - KEY_A] + (getKeyState(KEY_LSHIFT) || getKeyState(KEY_RSHIFT) ? 'A' - 'a' : 0);
    
    if(key >= KEY_Z && key <= KEY_M)
        return low_chars[key - KEY_Z] + (getKeyState(KEY_LSHIFT) || getKeyState(KEY_RSHIFT) ? 'A' - 'a' : 0);
    
    if(key == KEY_SPACE)
        return ' ';
    
    if(key >= KEY_0 && key <= KEY_9)
        return '0' + key - KEY_0;
    return 0;
}

bool lib::getKeyState(Key key) {
    return __systemCall(SYSCALL_GET_KEY_STATE, (unsigned char)key);
}
