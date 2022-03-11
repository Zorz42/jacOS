#pragma once

enum DebugLevel {
    DEBUG_INFO, DEBUG_WARNING, DEBUG_ERROR
};

namespace debug {

struct _hex {};
struct _dec {};
struct _endl {};

class _out_stream {
public:
    _out_stream operator<<(const char* string);
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

inline _hex hex;
inline _dec dec;
inline _endl endl;
inline _out_stream out;

}
