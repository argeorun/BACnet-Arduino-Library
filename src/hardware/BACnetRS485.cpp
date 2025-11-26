/*
 * BACnetRS485.cpp - RS-485 Hardware Abstraction Layer implementation
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 * 
 * Automatically configured from BACnetConfig.h for each supported board.
 */

#include "BACnetRS485.h"

// Static member initialization
HardwareSerial* BACnetRS485::_serial = nullptr;
int8_t BACnetRS485::_enable_pin = -1;
bool BACnetRS485::_auto_direction = false;
uint32_t BACnetRS485::_baud_rate = 38400;

// Automatic configuration from BACnetConfig.h
void BACnetRS485::begin(uint32_t baud_rate) {
    _serial = &BACNET_MSTP_SERIAL;
    _enable_pin = BACNET_RS485_ENABLE_PIN;
    _auto_direction = (_enable_pin == -1);
    _baud_rate = baud_rate;
    
    // Initialize serial port
    _serial->begin(baud_rate);
    
    // Configure DE/RE control pin if specified
    if (_enable_pin >= 0) {
        pinMode(_enable_pin, OUTPUT);
        setReceiveMode();  // Default to receive mode
    }
    
    // Print configuration (uses debug macros from BACnetConfig.h)
    printConfiguration();
}

// Custom configuration (advanced users)
void BACnetRS485::begin(HardwareSerial& serial_port, int8_t enable_pin, uint32_t baud_rate) {
    _serial = &serial_port;
    _enable_pin = enable_pin;
    _auto_direction = (enable_pin == -1);
    _baud_rate = baud_rate;
    
    // Initialize serial port
    _serial->begin(baud_rate);
    
    // Configure DE/RE control pin if specified
    if (_enable_pin >= 0) {
        pinMode(_enable_pin, OUTPUT);
        setReceiveMode();  // Default to receive mode
    }
    
    BACNET_DEBUG_PRINTLN(F("BACnet RS485: Custom configuration"));
    printConfiguration();
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

void BACnetRS485::printConfiguration() {
    BACNET_DEBUG_PRINTLN(F("=== RS485 Configuration ==="));
    
    // Show which serial port
    #if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
        BACNET_DEBUG_PRINTLN(F("Serial: Serial (TX=D1, RX=D0)"));
        BACNET_DEBUG_PRINTLN(F("⚠️  Uno: Debug output disabled"));
    #elif defined(ARDUINO_AVR_MEGA2560)
        #if defined(BACNET_MSTP_SERIAL)
            if (_serial == &Serial) {
                BACNET_DEBUG_PRINTLN(F("Serial: Serial (TX0/RX0)"));
            } else if (_serial == &Serial1) {
                BACNET_DEBUG_PRINTLN(F("Serial: Serial1 (TX1=D18, RX1=D19)"));
            } else if (_serial == &Serial2) {
                BACNET_DEBUG_PRINTLN(F("Serial: Serial2 (TX2=D16, RX2=D17)"));
            } else if (_serial == &Serial3) {
                BACNET_DEBUG_PRINTLN(F("Serial: Serial3 (TX3=D14, RX3=D15)"));
            }
        #endif
    #elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAMD_ZERO)
        BACNET_DEBUG_PRINTLN(F("Serial: Serial1 (TX1=D18, RX1=D19)"));
    #elif defined(ARDUINO_ARCH_ESP32)
        BACNET_DEBUG_PRINTLN(F("Serial: Serial1 (RX=GPIO16, TX=GPIO17)"));
    #elif defined(ARDUINO_ARCH_STM32)
        BACNET_DEBUG_PRINTLN(F("Serial: Serial1 (USART1)"));
    #endif
    
    // Show baud rate
    BACNET_DEBUG_PRINT(F("Baud Rate: "));
    BACNET_DEBUG_PRINTLN(_baud_rate);
    
    // Show DE/RE pin
    if (_auto_direction) {
        BACNET_DEBUG_PRINTLN(F("DE/RE Pin: Auto-direction (no control)"));
    } else {
        BACNET_DEBUG_PRINT(F("DE/RE Pin: "));
        #if defined(ARDUINO_ARCH_STM32)
            // STM32 uses PXn notation - can't print directly
            BACNET_DEBUG_PRINTLN(F("(see BACnetConfig.h)"));
        #else
            BACNET_DEBUG_PRINTLN(_enable_pin);
        #endif
    }
    
    BACNET_DEBUG_PRINTLN(F("==========================="));
}
