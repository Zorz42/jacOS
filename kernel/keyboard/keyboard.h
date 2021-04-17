#ifndef keyboard_h
#define keyboard_h

void initKeyboard();

typedef enum {
    KEY_UNKNOWN = -1,
    
    // numbers
    KEY_0 = 0,
    KEY_1 = 1,
    KEY_2 = 2,
    KEY_3 = 3,
    KEY_4 = 4,
    KEY_5 = 5,
    KEY_6 = 6,
    KEY_7 = 7,
    KEY_8 = 8,
    KEY_9 = 9,
    
    // letters
    KEY_A = 10,
    KEY_B = 11,
    KEY_C = 12,
    KEY_D = 13,
    KEY_E = 14,
    KEY_F = 15,
    KEY_G = 16,
    KEY_H = 17,
    KEY_I = 18,
    KEY_J = 19,
    KEY_K = 20,
    KEY_L = 21,
    KEY_M = 22,
    KEY_N = 23,
    KEY_O = 24,
    KEY_P = 25,
    KEY_Q = 26,
    KEY_R = 27,
    KEY_S = 28,
    KEY_T = 29,
    KEY_U = 30,
    KEY_V = 31,
    KEY_W = 32,
    KEY_X = 33,
    KEY_Y = 34,
    KEY_Z = 35,
    
    // non letter or number characters
    KEY_MINUS = 36,
    KEY_PLUS = 37,
    KEY_LEFTBRACKET = 38,
    KEY_RIGHTBRACKET = 39,
    KEY_TAB = 40,
    KEY_SEMICOLON = 41,
    KEY_QUOTE = 42,
    KEY_BACKQUOTE = 43,
    KEY_PERIOD = 44,
    KEY_COMMA = 45,
    KEY_SLASH = 46,
    KEY_SPACE = 47,
    KEY_BACKSLASH = 48,
    
    // special keys
    KEY_BACKSPACE = 49,
    KEY_ESCAPE = 50,
    KEY_ENTER = 51,
    KEY_LCTRL = 52,
    KEY_LSHIFT = 53,
    KEY_RSHIFT = 54,
    KEY_LALT = 55,
    
    KEY_COUNT = 56,
} Key;

#endif /* keyboard_h */
