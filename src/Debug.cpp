#include "Debug.h"

bool Debug::initialized = false;

void Debug::begin(unsigned long baud)
{
#ifdef DEBUG
    Serial.begin(baud);
    initialized = true;
#endif
}

void Debug::println()
{
#ifdef DEBUG
    if (initialized)
    {
        Serial.println();
    }
#endif
}

void Debug::println(const String &s)
{
#ifdef DEBUG
    if (initialized)
    {
        Serial.println(s);
    }
#endif
}

void Debug::print(const String &s)
{
#ifdef DEBUG
    if (initialized)
    {
        Serial.print(s);
    }
#endif
}

void Debug::print(char c)
{
#ifdef DEBUG
    if (initialized)
    {
        Serial.print(c);
    }
#endif
}