#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* ----------------------------------------------------
   PIN DEFINITIONS (EDIT HERE)
   ---------------------------------------------------- */

// Inputs
#define PIN_D3        3
#define PIN_D4        4
#define PIN_D5        5
#define PIN_D6        6
#define PIN_D7        7

// Outputs
#define PIN_D8        8
#define PIN_D9        9
#define PIN_D10       10
#define PIN_D11       11
#define PIN_D12       12

// Built-in LED
#define PIN_LED       13    // For ATmega328 (Arduino Uno)
// For ESP32 use: #define PIN_LED 2

/* ----------------------------------------------------
   FUNCTION DECLARATIONS
   ---------------------------------------------------- */

void PIN_INIT(uint8_t pin, bool is_output);
void PIN_WRITE(uint8_t pin, bool active);
bool PIN_READ(uint8_t pin);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
