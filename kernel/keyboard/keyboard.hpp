#pragma once

namespace keyboard {

enum Key {
    KEY_UNKNOWN,
    
    // numbers
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    
    // letters
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    
    // non letter or number characters
    KEY_MINUS,
    KEY_PLUS,
    KEY_LEFTBRACKET,
    KEY_RIGHTBRACKET,
    KEY_TAB,
    KEY_SEMICOLON,
    KEY_QUOTE,
    KEY_BACKQUOTE,
    KEY_PERIOD,
    KEY_COMMA,
    KEY_SLASH,
    KEY_SPACE,
    KEY_BACKSLASH,
    
    // special keys
    KEY_BACKSPACE,
    KEY_ESCAPE,
    KEY_ENTER,
    KEY_LCTRL,
    KEY_LSHIFT,
    KEY_RSHIFT,
    KEY_LALT,
    
    KEY_COUNT,
};

void init();
char keyToAscii(Key key);
bool getKeyState(Key key);

}
