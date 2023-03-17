#ifndef _DEBUG_
#define _DEBUG_

#include <Arduino.h>

class Debug
{
private:
    static bool initialized;
public:
    static void begin(unsigned long baud);
    static void println();
    static void println(const String &s);
    static void print(const String &s);
    static void print(char c);
};

#endif