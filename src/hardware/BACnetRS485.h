/*
 * BACnetRS485.h - RS-485 Hardware Abstraction Layer
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 * 
 * Provides interface for RS-485 transceivers with optional DE/RE control.
 * Supports both modules with DE/RE pins and auto-direction modules.
 */

#ifndef BACNET_RS485_H
#define BACNET_RS485_H

#include <Arduino.h>
#include "../BACnetConfig.h"

class BACnetRS485 {
public:
    /**
     * Initialize RS-485 with optional DE/RE control pin
     * @param serial_port Pointer to HardwareSerial (Serial, Serial1, etc.)
     * @param enable_pin DE/RE control pin (-1 for auto-direction modules)
     * @param baud_rate Serial baud rate (default: 38400)
     */
    static void begin(HardwareSerial* serial_port, 
                     int8_t enable_pin = -1,
                     uint32_t baud_rate = 38400);
    
    /**
     * Set receive mode (DE/RE = LOW)
     */
    static void setReceiveMode();
    
    /**
     * Set transmit mode (DE/RE = HIGH)
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
     * Write a byte
     */
    static size_t write(uint8_t data);
    
    /**
     * Write multiple bytes
     */
    static size_t write(const uint8_t* buffer, size_t size);
    
    /**
     * Flush transmit buffer
     */
    static void flush();
    
    /**
     * Factory methods for common RS-485 modules
     */
    
    /**
     * Configure for MAX485 module with DE/RE control
     * @param serial_port HardwareSerial port
     * @param enable_pin Arduino pin connected to DE and RE pins
     */
    static void beginMAX485(HardwareSerial* serial_port, int8_t enable_pin);
    
    /**
     * Configure for auto-direction RS-485 modules (no DE/RE control needed)
     * @param serial_port HardwareSerial port
     */
    static void beginAutoDirection(HardwareSerial* serial_port);
    
    /**
     * Configure for DFRobot DFR0259 RS-485 Shield
     * Arduino Uno/Mega shield with auto-configuration
     */
    static void beginDFR0259Shield();

private:
    static HardwareSerial* _serial;
    static int8_t _enable_pin;
    static bool _auto_direction;
};

#endif // BACNET_RS485_H
