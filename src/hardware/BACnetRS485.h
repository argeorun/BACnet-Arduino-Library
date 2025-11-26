/*
 * BACnetRS485.h - RS-485 Hardware Abstraction Layer
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 * 
 * Provides interface for RS-485 transceivers with optional DE/RE control.
 * Automatically configured from BACnetConfig.h based on detected board.
 * 
 * Usage:
 *   BACnetRS485::begin();  // Auto-configures from BACnetConfig.h
 * 
 * Or for custom configuration:
 *   BACnetRS485::begin(&Serial2, 8, 38400);  // Custom serial port and pin
 */

#ifndef BACNET_RS485_H
#define BACNET_RS485_H

#include <Arduino.h>
#include "../BACnetConfig.h"

class BACnetRS485 {
public:
    /**
     * Initialize RS-485 using automatic configuration from BACnetConfig.h
     * This is the preferred method - works out-of-box for all supported boards
     * 
     * @param baud_rate Serial baud rate (default: 38400 for BACnet MS/TP)
     */
    static void begin(uint32_t baud_rate = 38400);
    
    /**
     * Initialize RS-485 with custom configuration (advanced users)
     * Use this to override BACnetConfig.h defaults
     * 
     * @param serial_port Reference to HardwareSerial (Serial, Serial1, etc.)
     * @param enable_pin DE/RE control pin (-1 for auto-direction modules)
     * @param baud_rate Serial baud rate (default: 38400)
     */
    static void begin(HardwareSerial& serial_port, 
                     int8_t enable_pin,
                     uint32_t baud_rate = 38400);
    
    /**
     * Set receive mode (DE/RE = LOW)
     * Only affects modules with DE/RE control (enable_pin >= 0)
     */
    static void setReceiveMode();
    
    /**
     * Set transmit mode (DE/RE = HIGH)
     * Only affects modules with DE/RE control (enable_pin >= 0)
     */
    static void setTransmitMode();
    
    /**
     * Check if data is available
     */
    static int available();
    
    /**
     * Read a byte
     */
    static int read();
    
    /**
     * Write a byte (automatically handles DE/RE switching)
     */
    static size_t write(uint8_t data);
    
    /**
     * Write multiple bytes (automatically handles DE/RE switching)
     */
    static size_t write(const uint8_t* buffer, size_t size);
    
    /**
     * Flush transmit buffer
     */
    static void flush();
    
    /**
     * Get current configuration info (for debugging)
     */
    static void printConfiguration();

private:
    static HardwareSerial* _serial;
    static int8_t _enable_pin;
    static bool _auto_direction;
    static uint32_t _baud_rate;
};

#endif // BACNET_RS485_H
