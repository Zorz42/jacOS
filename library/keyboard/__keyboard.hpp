#pragma once
#include <keyboard/__keys.hpp>

class KeyEvent {
public:
    Key key;
    bool up;
};

KeyEvent getKeyEvent();
char keyToAscii(Key key);
bool getKeyState(Key key);
