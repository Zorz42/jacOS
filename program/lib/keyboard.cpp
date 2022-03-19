#include <lib.hpp>

#define SYSCALL_GET_KEY_EVENT 7

static lib::Key scancodeToKey(unsigned char scancode);

lib::KeyEvent lib::getKeyEvent() {
    unsigned char scancode = lib::__systemCall(SYSCALL_GET_KEY_EVENT);
    KeyEvent event;
    if(scancode == 0) {
        event.key = NO_KEY_EVENT;
        return event;
    }
    
    event.up = false;
    if(scancode > 0x80) {
        event.up = true;
        scancode -= 0x80;
    }
    
    event.key = scancodeToKey(scancode);
    
    return event;
}

char lib::keyToAscii(Key key) {
    if(key >= KEY_A && key <= KEY_Z)
        return (/*getKeyState(KEY_LSHIFT) || getKeyState(KEY_RSHIFT)*/false ? 'A' : 'a') + key - KEY_A;
    if(key == KEY_SPACE)
        return ' ';
    if(key >= KEY_0 && key <= KEY_9)
        return '0' + key - KEY_0;
    return 0;
}

static lib::Key scancodeToKey(unsigned char scancode) {
    switch (scancode) {
        case 0x1: return lib::KEY_ESCAPE;
        case 0x2: return lib::KEY_1;
        case 0x3: return lib::KEY_2;
        case 0x4: return lib::KEY_3;
        case 0x5: return lib::KEY_4;
        case 0x6: return lib::KEY_5;
        case 0x7: return lib::KEY_6;
        case 0x8: return lib::KEY_7;
        case 0x9: return lib::KEY_8;
        case 0x0A: return lib::KEY_9;
        case 0x0B: return lib::KEY_0;
        case 0x0C: return lib::KEY_MINUS;
        case 0x0D: return lib::KEY_PLUS;
        case 0x0E: return lib::KEY_BACKSPACE;
        case 0x0F: return lib::KEY_TAB;
        case 0x10: return lib::KEY_Q;
        case 0x11: return lib::KEY_W;
        case 0x12: return lib::KEY_E;
        case 0x13: return lib::KEY_R;
        case 0x14: return lib::KEY_T;
        case 0x15: return lib::KEY_Y;
        case 0x16: return lib::KEY_U;
        case 0x17: return lib::KEY_I;
        case 0x18: return lib::KEY_O;
        case 0x19: return lib::KEY_P;
        case 0x1A: return lib::KEY_RIGHTBRACKET;
        case 0x1B: return lib::KEY_LEFTBRACKET;
        case 0x1C: return lib::KEY_ENTER;
        case 0x1D: return lib::KEY_LCTRL;
        case 0x1E: return lib::KEY_A;
        case 0x1F: return lib::KEY_S;
        case 0x20: return lib::KEY_D;
        case 0x21: return lib::KEY_F;
        case 0x22: return lib::KEY_G;
        case 0x23: return lib::KEY_H;
        case 0x24: return lib::KEY_J;
        case 0x25: return lib::KEY_K;
        case 0x26: return lib::KEY_L;
        case 0x27: return lib::KEY_SEMICOLON;
        case 0x28: return lib::KEY_QUOTE;
        case 0x29: return lib::KEY_BACKQUOTE;
        case 0x2A: return lib::KEY_LSHIFT;
        case 0x2B: return lib::KEY_BACKSLASH;
        case 0x2C: return lib::KEY_Z;
        case 0x2D: return lib::KEY_X;
        case 0x2E: return lib::KEY_C;
        case 0x2F: return lib::KEY_V;
        case 0x30: return lib::KEY_B;
        case 0x31: return lib::KEY_N;
        case 0x32: return lib::KEY_M;
        case 0x33: return lib::KEY_COMMA;
        case 0x34: return lib::KEY_PERIOD;
        case 0x35: return lib::KEY_SLASH;
        case 0x36: return lib::KEY_RSHIFT;
        //case 0x37: return lib::KEY_1;
            //kprint("Keypad *");
        case 0x38: return lib::KEY_LALT;
        case 0x39: return lib::KEY_SPACE;
        default: return lib::KEY_UNKNOWN;
    }
}
