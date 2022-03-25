#pragma once
#include <library.hpp>

#define DEBUG_ENABLED
#define DEBUG_LEVEL DEBUG_INFO

enum DebugLevel {
    DEBUG_INFO, DEBUG_WARNING, DEBUG_ERROR
};

namespace debug {

struct _hex {};
struct _dec {};
struct _endl {};

#ifdef DEBUG_ENABLED
class _out_stream {
public:
    _out_stream operator<<(const char* string);
    _out_stream operator<<(const String& string);
    _out_stream operator<<(char character);
    _out_stream operator<<(long number);
    _out_stream operator<<(int number);
    _out_stream operator<<(float number);
    _out_stream operator<<(unsigned long number);
    _out_stream operator<<(unsigned int number);
    _out_stream operator<<(_hex _);
    _out_stream operator<<(_dec _);
    _out_stream operator<<(_endl _);
    _out_stream operator<<(DebugLevel level);
};
#else
class _out_stream {
public:
    _out_stream operator<<(const char* string) { return *this; }
    _out_stream operator<<(char character) { return *this; }
    _out_stream operator<<(long number) { return *this; }
    _out_stream operator<<(int number) { return *this; }
    _out_stream operator<<(float number) { return *this; }
    _out_stream operator<<(unsigned long number) { return *this; }
    _out_stream operator<<(unsigned int number) { return *this; }
    _out_stream operator<<(_hex _) { return *this; }
    _out_stream operator<<(_dec _) { return *this; }
    _out_stream operator<<(_endl _) { return *this; }
    _out_stream operator<<(DebugLevel level) { return *this; }
};
#endif

inline _hex hex;
inline _dec dec;
inline _endl endl;
inline _out_stream out;

}
