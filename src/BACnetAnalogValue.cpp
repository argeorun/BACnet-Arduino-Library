/*
 * BACnetAnalogValue.cpp - Analog Value Object implementation
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 */

#include "BACnetAnalogValue.h"

// BACnet object type for Analog Value
#define OBJECT_ANALOG_VALUE 2

BACnetAnalogValue::BACnetAnalogValue(uint32_t instance, 
                                     const char* name,
                                     BACnetEngineeringUnits units)
    : BACnetObject(instance, name)
    , _present_value(0.0)
    , _previous_value(0.0)
    , _units(units)
    , _resolution(0.1)
    , _min_value(0.0)
    , _max_value(100.0)
    , _pin(-1)
#if BACNET_FEATURE_COV
    , _cov_enabled(false)
    , _cov_increment(1.0)
    , _last_cov_time(0)
#endif
{
}

uint16_t BACnetAnalogValue::getObjectType() const {
    return OBJECT_ANALOG_VALUE;
}

void BACnetAnalogValue::setValue(float value) {
    // Constrain to min/max
    if (value < _min_value) value = _min_value;
    if (value > _max_value) value = _max_value;
    
    _previous_value = _present_value;
    _present_value = value;
    
#if BACNET_FEATURE_COV
    // Trigger COV notification if enabled and change exceeds increment
    if (_cov_enabled) {
        float change = abs(_present_value - _previous_value);
        if (change >= _cov_increment) {
            _last_cov_time = millis();
            // Note: Full COV implementation will be added when integrating
            // with bacnet-stack COV service handlers
        }
    }
#endif
}

void BACnetAnalogValue::setUnits(BACnetEngineeringUnits units) {
    _units = units;
}

void BACnetAnalogValue::setResolution(float resolution) {
    if (resolution > 0.0) {
        _resolution = resolution;
    }
}

void BACnetAnalogValue::setMinValue(float min) {
    _min_value = min;
    
    // Ensure current value is within range
    if (_present_value < _min_value) {
        setValue(_min_value);
    }
}

void BACnetAnalogValue::setMaxValue(float max) {
    _max_value = max;
    
    // Ensure current value is within range
    if (_present_value > _max_value) {
        setValue(_max_value);
    }
}

void BACnetAnalogValue::setPin(int8_t pin) {
    _pin = pin;
    
    if (_pin >= 0) {
        // Configure as analog input
        pinMode(_pin, INPUT);
    }
}

void BACnetAnalogValue::readPin() {
    if (_pin >= 0) {
        // Read ADC value (0-1023 on most Arduino boards)
        int raw = analogRead(_pin);
        
        // Map to min/max range
        float value = map(raw, 0, 1023, 
                         (long)(_min_value * 100), 
                         (long)(_max_value * 100)) / 100.0;
        
        setValue(value);
    }
}

float BACnetAnalogValue::readPinVoltage() {
    if (_pin >= 0) {
        int raw = analogRead(_pin);
        
        // Convert to voltage (assuming 5V reference)
        // Adjust for 3.3V boards if needed
        #if defined(ARDUINO_ARCH_SAMD) || defined(ESP32)
        return (raw * 3.3) / 1023.0;
        #else
        return (raw * 5.0) / 1023.0;
        #endif
    }
    return 0.0;
}

#if BACNET_FEATURE_COV
void BACnetAnalogValue::enableCOV(float increment) {
    _cov_enabled = true;
    _cov_increment = increment;
}

void BACnetAnalogValue::disableCOV() {
    _cov_enabled = false;
}
#endif

void BACnetAnalogValue::update() {
    // Read pin if configured
    if (_pin >= 0) {
        readPin();
    }
}
