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

enum Key : unsigned char;
class KeyEvent {
public:
    Key key;
    bool up;
};

KeyEvent getKeyEvent();
char keyToAscii(Key key);
bool getKeyState(Key key);
unsigned int getCursorX();
unsigned int getCursorY();
void moveCursorTo(unsigned int x, unsigned int y);

void sleep(unsigned int ms);

unsigned int getTotalMemory();
unsigned int getUsedMemory();
unsigned int getFreeMemory();

enum Key : unsigned char {
    KEY_UNKNOWN = 0x00,
    
    // numbers
    KEY_0             = 0x0B,
    KEY_1             = 0x02,
    KEY_2             = 0x03,
    KEY_3             = 0x04,
    KEY_4             = 0x05,
    KEY_5             = 0x06,
    KEY_6             = 0x07,
    KEY_7             = 0x08,
    KEY_8             = 0x09,
    KEY_9             = 0x0A,
    
    // letters
    KEY_A             = 0x1E,
    KEY_B             = 0x30,
    KEY_C             = 0x2E,
    KEY_D             = 0x20,
    KEY_E             = 0x12,
    KEY_F             = 0x21,
    KEY_G             = 0x22,
    KEY_H             = 0x23,
    KEY_I             = 0x17,
    KEY_J             = 0x24,
    KEY_K             = 0x25,
    KEY_L             = 0x26,
    KEY_M             = 0x32,
    KEY_N             = 0x31,
    KEY_O             = 0x18,
    KEY_P             = 0x19,
    KEY_Q             = 0x10,
    KEY_R             = 0x13,
    KEY_S             = 0x1F,
    KEY_T             = 0x14,
    KEY_U             = 0x16,
    KEY_V             = 0x2F,
    KEY_W             = 0x11,
    KEY_X             = 0x2D,
    KEY_Y             = 0x15,
    KEY_Z             = 0x2C,
    
    // non letter or number characters
    KEY_MINUS         = 0x0C,
    KEY_PLUS          = 0x0D,
    KEY_LEFTBRACKET   = 0x1B,
    KEY_RIGHTBRACKET  = 0x1A,
    KEY_TAB           = 0x0F,
    KEY_SEMICOLON     = 0x27,
    KEY_QUOTE         = 0x28,
    KEY_BACKQUOTE     = 0x29,
    KEY_PERIOD        = 0x34,
    KEY_COMMA         = 0x33,
    KEY_SLASH         = 0x35,
    KEY_SPACE         = 0x39,
    KEY_BACKSLASH     = 0x2B,
    
    // special keys
    KEY_BACKSPACE     = 0x0E,
    KEY_ESCAPE        = 0x01,
    KEY_ENTER         = 0x1C,
    KEY_LCTRL         = 0x1D,
    KEY_LSHIFT        = 0x2A,
    KEY_RSHIFT        = 0x36,
    KEY_LALT          = 0x38,
};

};
