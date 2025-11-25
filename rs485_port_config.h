// Optional: define RS485_PORT here to select the HardwareSerial used for RS485.
// Default for Arduino Mega 2560: use Serial1 (TX1=18, RX1=19).
#ifndef RS485_PORT
#define RS485_PORT Serial
#endif

// RS485_PORT defaults to Serial1 if not overridden by build flags or prior definitions.
