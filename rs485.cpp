/*
 * Arduino RS485 API override for BACnet MSTP stack.
 * Implements the rs485.h interface using HardwareSerial and millis().
 */
#include <Arduino.h>
#include "src/bacnet/basic/sys/mstimer.h"
#include "rs485.h"  // existing header with API declarations

/* Select which HardwareSerial to use for RS485.
 * User may define RS485_PORT (e.g., to Serial1/2/3) via a shared header or
 * build flag. If not defined, we fall back to Serial.
 */
#include "rs485_port_config.h"  // optional user config header
#ifndef RS485_PORT
#define RS485_PORT Serial
#endif

/* Default baud rate */
static uint32_t RS485_Baud = 9600;
static struct mstimer Silence_Timer = { 0, 0 };

/* Pin used to enable RS485 transceiver (DE/RE). Adjust if shield differs. */
static const uint8_t RS485_Enable_Pin = 2;  // D2 per DFR0259 manual transceiver mode

unsigned long RS485_Timer_Silence(void) {
  return mstimer_elapsed(&Silence_Timer);
}

void RS485_Timer_Silence_Reset(void) {
  mstimer_set(&Silence_Timer, 0);
}

void RS485_Initialize(void) {
  pinMode(RS485_Enable_Pin, OUTPUT);
  RS485_Transmitter_Enable(false);  // start in receive mode
  RS485_PORT.begin(RS485_Baud);
  RS485_Timer_Silence_Reset();
}

uint32_t RS485_Get_Baud_Rate(void) {
  return RS485_Baud;
}

uint32_t RS485_Baud_Rate_From_Kilo(uint8_t baud_k) {
  uint32_t baud = 0;
  if (baud_k == 255) {
    baud = 38400;
  } else if (baud_k >= 115) {
    baud = 115200;
  } else if (baud_k >= 76) {
    baud = 76800;
  } else if (baud_k >= 57) {
    baud = 57600;
  } else if (baud_k >= 38) {
    baud = 38400;
  } else if (baud_k >= 19) {
    baud = 19200;
  } else if (baud_k >= 9) {
    baud = 9600;
  } else {
    baud = 38400;
  }
  return baud;
}

bool RS485_Set_Baud_Rate(uint32_t baud) {
  switch (baud) {
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 76800:
    case 115200:
      RS485_Baud = baud;
      RS485_PORT.end();
      RS485_PORT.begin(RS485_Baud);
      return true;
    default:
      return false;
  }
}

void RS485_Transmitter_Enable(bool enable) {
  digitalWrite(RS485_Enable_Pin, enable ? HIGH : LOW);
}

void RS485_Turnaround_Delay(void) {
  /* 40 bit times per MSTP spec */
  unsigned long us = (40UL * 1000000UL) / RS485_Baud;
  delayMicroseconds(us);
}

void RS485_Send_Data(const uint8_t *buffer, uint16_t nbytes) {
  RS485_Transmitter_Enable(true);
  RS485_PORT.write(buffer, nbytes);
  RS485_PORT.flush();  // wait for transmit complete
  RS485_Transmitter_Enable(false);
  RS485_Timer_Silence_Reset();
}

bool RS485_ReceiveError(void) {
  /* Arduino HardwareSerial does not expose framing/overrun status. */
  return false;
}

bool RS485_DataAvailable(uint8_t *data) {
  if (RS485_PORT.available() > 0) {
    *data = static_cast<uint8_t>(RS485_PORT.read());
    return true;
  }
  return false;
}

void RS485_LED_Timers(void) {
  /* Optional: implement LED indication of activity if desired. */
}
