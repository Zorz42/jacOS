#pragma once
#include <_lib_keys.hpp>

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

//void sleep(unsigned int ms);

unsigned int getTotalMemory();
unsigned int getUsedMemory();
unsigned int getFreeMemory();

};
