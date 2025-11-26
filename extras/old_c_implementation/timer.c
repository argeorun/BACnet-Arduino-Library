/*
 * Arduino override of BACnet mstimer HAL using built-in millis().
 * This replaces the atmega328 hardware timer implementation for mega2560.
 */
#include <Arduino.h>
#include "src/bacnet/basic/sys/mstimer.h"

/* Nothing required for initialization when using millis(). */
void mstimer_init(void) {
  /* millis() starts automatically after boot */
}

/* Current time in milliseconds for BACnet stack timing. */
unsigned long mstimer_now(void) {
  return millis();
}

/* Legacy callback symbol retained (unused). */
void counter(void) {
  /* Intentionally empty - hardware interrupt not used in Arduino millis() version */
}
