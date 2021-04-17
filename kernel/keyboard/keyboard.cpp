#include "keyboard.h"
#include "drivers/ports.h"
#include "cpu/isr.h"
#include "text/text.h"
#include "kernel.h"

Key scancodeToKey(u8 scancode);

static void keyboardCallback(registers_t regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    u8 scancode = port_byte_in(0x60);
    bool up = false;
    if(scancode >= 0x80) {
        up = true;
        scancode -= 0x80;
    }
    Key key = scancodeToKey(scancode);
    onKeyEvent(key, up);
}

char keyToAscii(Key key) {
    if(key >= KEY_A && key <= KEY_Z)
        return 'a' + key - KEY_A;
    if(key == KEY_SPACE)
        return ' ';
    return 0;
}

void initKeyboard() {
    register_interrupt_handler(IRQ1, keyboardCallback);
}

Key scancodeToKey(u8 scancode) {
    switch (scancode) {
        case 0x1: return KEY_ESCAPE;
        case 0x2: return KEY_1;
        case 0x3: return KEY_2;
        case 0x4: return KEY_3;
        case 0x5: return KEY_4;
        case 0x6: return KEY_5;
        case 0x7: return KEY_6;
        case 0x8: return KEY_7;
        case 0x9: return KEY_8;
        case 0x0A: return KEY_9;
        case 0x0B: return KEY_0;
        case 0x0C: return KEY_MINUS;
        case 0x0D: return KEY_PLUS;
        case 0x0E: return KEY_BACKSPACE;
        case 0x0F: return KEY_TAB;
        case 0x10: return KEY_Q;
        case 0x11: return KEY_W;
        case 0x12: return KEY_E;
        case 0x13: return KEY_R;
        case 0x14: return KEY_T;
        case 0x15: return KEY_Y;
        case 0x16: return KEY_U;
        case 0x17: return KEY_I;
        case 0x18: return KEY_O;
        case 0x19: return KEY_P;
        case 0x1A: return KEY_RIGHTBRACKET;
        case 0x1B: return KEY_LEFTBRACKET;
        case 0x1C: return KEY_ENTER;
        case 0x1D: return KEY_LCTRL;
        case 0x1E: return KEY_A;
        case 0x1F: return KEY_S;
        case 0x20: return KEY_D;
        case 0x21: return KEY_F;
        case 0x22: return KEY_G;
        case 0x23: return KEY_H;
        case 0x24: return KEY_J;
        case 0x25: return KEY_K;
        case 0x26: return KEY_L;
        case 0x27: return KEY_SEMICOLON;
        case 0x28: return KEY_QUOTE;
        case 0x29: return KEY_BACKQUOTE;
        case 0x2A: return KEY_LSHIFT;
        case 0x2B: return KEY_BACKSLASH;
        case 0x2C: return KEY_Z;
        case 0x2D: return KEY_X;
        case 0x2E: return KEY_C;
        case 0x2F: return KEY_V;
        case 0x30: return KEY_B;
        case 0x31: return KEY_N;
        case 0x32: return KEY_M;
        case 0x33: return KEY_COMMA;
        case 0x34: return KEY_PERIOD;
        case 0x35: return KEY_SLASH;
        case 0x36: return KEY_RSHIFT;
        //case 0x37: return KEY_1;
            //kprint("Keypad *");
        case 0x38: return KEY_LALT;
        case 0x39: return KEY_SPACE;
        default: return KEY_UNKNOWN;
    }
}
