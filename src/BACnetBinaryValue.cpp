/*
 * BACnetBinaryValue.cpp - Binary Value Object implementation
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 */

#include "BACnetBinaryValue.h"

#if BACNET_OBJECT_BINARY_VALUE

// BACnet object type for Binary Value
#define OBJECT_BINARY_VALUE 5

BACnetBinaryValue::BACnetBinaryValue(uint32_t instance, const char* name, int8_t pin)
    : BACnetObject(instance, name)
    , _present_value(BINARY_INACTIVE)
    , _previous_value(BINARY_INACTIVE)
    , _pin(pin)
    , _pin_mode(OUTPUT)
#if BACNET_FEATURE_COV
    , _cov_enabled(false)
    , _last_cov_time(0)
#endif
{
    // Configure pin if specified
    if (_pin >= 0) {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }
}

uint16_t BACnetBinaryValue::getObjectType() const {
    return OBJECT_BINARY_VALUE;
}

void BACnetBinaryValue::setValue(BACnetBinaryState value) {
    _previous_value = _present_value;
    _present_value = value;
    
    // Update pin if configured
    if (_pin >= 0 && _pin_mode == OUTPUT) {
        writePin();
    }
    
#if BACNET_FEATURE_COV
    // Trigger COV notification if enabled and value changed
    if (_cov_enabled && _present_value != _previous_value) {
        _last_cov_time = millis();
        // Note: Full COV implementation will be added when integrating
        // with bacnet-stack COV service handlers
    }
#endif
}

void BACnetBinaryValue::setValue(bool value) {
    setValue(value ? BINARY_ACTIVE : BINARY_INACTIVE);
}

BACnetBinaryState BACnetBinaryValue::getValue() const {
    return _present_value;
}

bool BACnetBinaryValue::getValueBool() const {
    return (_present_value == BINARY_ACTIVE);
}

void BACnetBinaryValue::setPin(int8_t pin, uint8_t mode) {
    _pin = pin;
    _pin_mode = mode;
    
    if (_pin >= 0) {
        pinMode(_pin, mode);
        
        // Set initial state
        if (mode == OUTPUT) {
            writePin();
        } else {
            readPin();
        }
    }
}

void BACnetBinaryValue::readPin() {
    if (_pin >= 0) {
        bool pin_state = digitalRead(_pin);
        
        // Update value if it changed
        if (pin_state != getValueBool()) {
            setValue(pin_state);
        }
    }
}

void BACnetBinaryValue::writePin() {
    if (_pin >= 0 && _pin_mode == OUTPUT) {
        digitalWrite(_pin, _present_value == BINARY_ACTIVE ? HIGH : LOW);
    }
}

#if BACNET_FEATURE_COV
void BACnetBinaryValue::enableCOV() {
    _cov_enabled = true;
}

void BACnetBinaryValue::disableCOV() {
    _cov_enabled = false;
}
#endif

void BACnetBinaryValue::update() {
    // Read pin if configured as input
    if (_pin >= 0 && _pin_mode != OUTPUT) {
        readPin();
    }
}

#endif // BACNET_OBJECT_BINARY_VALUE
