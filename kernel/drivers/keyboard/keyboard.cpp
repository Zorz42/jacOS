#include "keyboard.hpp"
#include "drivers/ports/ports.hpp"
#include "cpu/isr.hpp"
#include "text/text.hpp"
#include "kernel.hpp"

static keyboard::Key scancodeToKey(u8 scancode);
static bool keyStates[keyboard::KEY_COUNT];

static void keyboardCallback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    u8 scancode = ports::byteIn(0x60);
    bool up = false;
    if(scancode >= 0x80) {
        up = true;
        scancode -= 0x80;
    }
    keyboard::Key key = scancodeToKey(scancode);
    if(key != keyboard::KEY_UNKNOWN)
        keyStates[key] = !up;
    onKeyEvent(key, up);
}

char keyboard::keyToAscii(Key key) {
    if(key >= KEY_A && key <= KEY_Z)
        return (getKeyState(KEY_LSHIFT) || getKeyState(KEY_RSHIFT) ? 'A' : 'a') + key - KEY_A;
    if(key == KEY_SPACE)
        return ' ';
    if(key >= KEY_0 && key <= KEY_9)
        return '0' + key - KEY_0;
    return 0;
}

bool keyboard::getKeyState(Key key) {
    return keyStates[key];
}

void keyboard::init() {
    register_interrupt_handler(IRQ1, keyboardCallback);
}

static keyboard::Key scancodeToKey(u8 scancode) {
    switch (scancode) {
        case 0x1: return keyboard::KEY_ESCAPE;
        case 0x2: return keyboard::KEY_1;
        case 0x3: return keyboard::KEY_2;
        case 0x4: return keyboard::KEY_3;
        case 0x5: return keyboard::KEY_4;
        case 0x6: return keyboard::KEY_5;
        case 0x7: return keyboard::KEY_6;
        case 0x8: return keyboard::KEY_7;
        case 0x9: return keyboard::KEY_8;
        case 0x0A: return keyboard::KEY_9;
        case 0x0B: return keyboard::KEY_0;
        case 0x0C: return keyboard::KEY_MINUS;
        case 0x0D: return keyboard::KEY_PLUS;
        case 0x0E: return keyboard::KEY_BACKSPACE;
        case 0x0F: return keyboard::KEY_TAB;
        case 0x10: return keyboard::KEY_Q;
        case 0x11: return keyboard::KEY_W;
        case 0x12: return keyboard::KEY_E;
        case 0x13: return keyboard::KEY_R;
        case 0x14: return keyboard::KEY_T;
        case 0x15: return keyboard::KEY_Y;
        case 0x16: return keyboard::KEY_U;
        case 0x17: return keyboard::KEY_I;
        case 0x18: return keyboard::KEY_O;
        case 0x19: return keyboard::KEY_P;
        case 0x1A: return keyboard::KEY_RIGHTBRACKET;
        case 0x1B: return keyboard::KEY_LEFTBRACKET;
        case 0x1C: return keyboard::KEY_ENTER;
        case 0x1D: return keyboard::KEY_LCTRL;
        case 0x1E: return keyboard::KEY_A;
        case 0x1F: return keyboard::KEY_S;
        case 0x20: return keyboard::KEY_D;
        case 0x21: return keyboard::KEY_F;
        case 0x22: return keyboard::KEY_G;
        case 0x23: return keyboard::KEY_H;
        case 0x24: return keyboard::KEY_J;
        case 0x25: return keyboard::KEY_K;
        case 0x26: return keyboard::KEY_L;
        case 0x27: return keyboard::KEY_SEMICOLON;
        case 0x28: return keyboard::KEY_QUOTE;
        case 0x29: return keyboard::KEY_BACKQUOTE;
        case 0x2A: return keyboard::KEY_LSHIFT;
        case 0x2B: return keyboard::KEY_BACKSLASH;
        case 0x2C: return keyboard::KEY_Z;
        case 0x2D: return keyboard::KEY_X;
        case 0x2E: return keyboard::KEY_C;
        case 0x2F: return keyboard::KEY_V;
        case 0x30: return keyboard::KEY_B;
        case 0x31: return keyboard::KEY_N;
        case 0x32: return keyboard::KEY_M;
        case 0x33: return keyboard::KEY_COMMA;
        case 0x34: return keyboard::KEY_PERIOD;
        case 0x35: return keyboard::KEY_SLASH;
        case 0x36: return keyboard::KEY_RSHIFT;
        //case 0x37: return keyboard::KEY_1;
            //kprint("Keypad *");
        case 0x38: return keyboard::KEY_LALT;
        case 0x39: return keyboard::KEY_SPACE;
        default: return keyboard::KEY_UNKNOWN;
    }
}
