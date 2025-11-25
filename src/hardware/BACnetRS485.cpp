/*
 * BACnetRS485.cpp - RS-485 Hardware Abstraction Layer implementation
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 */

#include "BACnetRS485.h"

// Static member initialization
HardwareSerial* BACnetRS485::_serial = nullptr;
int8_t BACnetRS485::_enable_pin = -1;
bool BACnetRS485::_auto_direction = false;

void BACnetRS485::begin(HardwareSerial* serial_port, int8_t enable_pin, uint32_t baud_rate) {
    _serial = serial_port;
    _enable_pin = enable_pin;
    _auto_direction = (enable_pin == -1);
    
    // Initialize serial port
    if (_serial != nullptr) {
        _serial->begin(baud_rate);
    }
    
    // Configure DE/RE control pin if specified
    if (_enable_pin >= 0) {
        pinMode(_enable_pin, OUTPUT);
        setReceiveMode();  // Default to receive mode
    }
}

void BACnetRS485::setReceiveMode() {
    if (_enable_pin >= 0) {
        digitalWrite(_enable_pin, LOW);  // DE/RE = LOW for receive
    }
}

void BACnetRS485::setTransmitMode() {
    if (_enable_pin >= 0) {
        digitalWrite(_enable_pin, HIGH);  // DE/RE = HIGH for transmit
    }
}

int BACnetRS485::available() {
    if (_serial != nullptr) {
        return _serial->available();
    }
    return 0;
}

int BACnetRS485::read() {
    if (_serial != nullptr) {
        return _serial->read();
    }
    return -1;
}

size_t BACnetRS485::write(uint8_t data) {
    if (_serial != nullptr) {
        if (!_auto_direction) {
            setTransmitMode();
        }
        
        size_t result = _serial->write(data);
        
        if (!_auto_direction) {
            _serial->flush();  // Wait for transmission complete
            setReceiveMode();
        }
        
        return result;
    }
    return 0;
}

size_t BACnetRS485::write(const uint8_t* buffer, size_t size) {
    if (_serial != nullptr && buffer != nullptr) {
        if (!_auto_direction) {
            setTransmitMode();
        }
        
        size_t result = _serial->write(buffer, size);
        
        if (!_auto_direction) {
            _serial->flush();  // Wait for transmission complete
            setReceiveMode();
        }
        
        return result;
    }
    return 0;
}

void BACnetRS485::flush() {
    if (_serial != nullptr) {
        _serial->flush();
    }
}

// Factory methods
void BACnetRS485::beginMAX485(HardwareSerial* serial_port, int8_t enable_pin) {
    begin(serial_port, enable_pin, 38400);
}

void BACnetRS485::beginAutoDirection(HardwareSerial* serial_port) {
    begin(serial_port, -1, 38400);
}

void BACnetRS485::beginDFR0259Shield() {
    // DFRobot DFR0259 shield uses:
    // - Serial1 on Mega (pins 18/19)
    // - Software Serial on Uno (configurable)
    // - DE/RE control on pin 2
    
    #if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
        begin(&Serial1, 2, 38400);
    #else
        // On Uno, use main Serial port
        begin(&Serial, 2, 38400);
    #endif
}
