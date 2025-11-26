#include "pin_config.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

void PIN_INIT(uint8_t pin, bool is_output)
{
#ifdef ARDUINO
    pinMode(pin, is_output ? OUTPUT : INPUT);
    if (is_output)
        digitalWrite(pin, LOW);
#endif
}

void PIN_WRITE(uint8_t pin, bool active)
{
#ifdef ARDUINO
    digitalWrite(pin, active ? HIGH : LOW);
#endif
}

bool PIN_READ(uint8_t pin)
{
#ifdef ARDUINO
    return (digitalRead(pin) == HIGH);
#else
    return false;
#endif
}
