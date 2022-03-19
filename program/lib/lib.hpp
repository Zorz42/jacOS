#pragma once

namespace lib {

unsigned int __systemCall(unsigned int func, unsigned int arg1=0, unsigned int arg2=0, unsigned int arg3=0);

class __hex {};
class __dec {};
class __endl {};
class __flush {};

class __out {
public:
    __out operator<<(const char* text);
    __out operator<<(unsigned int number);
    __out operator<<(int number);
    __out operator<<(char c);
    __out operator<<(__hex _);
    __out operator<<(__dec _);
    __out operator<<(__endl _);
    __out operator<<(__flush _);
};

inline __out out;
inline __hex hex;
inline __endl endl;
inline __dec dec;
inline __flush flush;

enum Key : unsigned int;
class KeyEvent {
public:
    Key key;
    bool up;
};

KeyEvent getKeyEvent();
char keyToAscii(Key key);

void sleep(unsigned int ms);


enum Key : unsigned int {
    KEY_UNKNOWN,
    NO_KEY_EVENT,
    
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

};
