/*
 * BACnetBinaryValue.h - Binary Value Object wrapper
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 * 
 * Binary Value objects represent two-state (on/off, active/inactive) values.
 * They can be read and written by BACnet clients.
 */

#ifndef BACNET_BINARY_VALUE_H
#define BACNET_BINARY_VALUE_H

#include <Arduino.h>
#include "BACnetDevice.h"
#include "BACnetConfig.h"

// Only compile if Binary Value objects are enabled
#if BACNET_OBJECT_BINARY_VALUE

// Binary values
typedef enum {
    BINARY_INACTIVE = 0,
    BINARY_ACTIVE = 1
} BACnetBinaryState;

class BACnetBinaryValue : public BACnetObject {
public:
    /**
     * Create a Binary Value object
     * @param instance Object instance number (0-4194303)
     * @param name Object name (max 31 characters)
     * @param pin Optional: Arduino pin number to control (-1 = no pin)
     */
    BACnetBinaryValue(uint32_t instance, const char* name, int8_t pin = -1);
    
    /**
     * Get the BACnet object type
     */
    uint16_t getObjectType() const override;
    
    /**
     * Set the present value
     * @param value BINARY_ACTIVE or BINARY_INACTIVE
     */
    void setValue(BACnetBinaryState value);
    
    /**
     * Set present value using boolean
     * @param value true = ACTIVE, false = INACTIVE
     */
    void setValue(bool value);
    
    /**
     * Get the present value
     * @return BINARY_ACTIVE or BINARY_INACTIVE
     */
    BACnetBinaryState getValue() const;
    
    /**
     * Get present value as boolean
     * @return true = ACTIVE, false = INACTIVE
     */
    bool getValueBool() const;
    
    /**
     * Set the Arduino pin for this object
     * @param pin Pin number (-1 to disable)
     * @param mode Pin mode: INPUT, OUTPUT, INPUT_PULLUP
     */
    void setPin(int8_t pin, uint8_t mode = OUTPUT);
    
    /**
     * Get the Arduino pin number
     * @return Pin number or -1 if not assigned
     */
    int8_t getPin() const { return _pin; }
    
    /**
     * Read the digital pin value and update Present_Value
     * Only works if pin is configured
     */
    void readPin();
    
    /**
     * Write Present_Value to the digital pin
     * Only works if pin is configured as OUTPUT
     */
    void writePin();
    
#if BACNET_FEATURE_COV
    /**
     * Enable Change of Value (COV) notifications for this object
     * Requires Tier 2+ board (Mega, Due, ESP32)
     */
    void enableCOV();
    
    /**
     * Disable COV notifications
     */
    void disableCOV();
    
    /**
     * Check if COV is enabled
     */
    bool isCOVEnabled() const { return _cov_enabled; }
#endif

    /**
     * Update function called periodically
     */
    void update() override;

private:
    BACnetBinaryState _present_value;
    BACnetBinaryState _previous_value;
    int8_t _pin;
    uint8_t _pin_mode;
    
#if BACNET_FEATURE_COV
    bool _cov_enabled;
    unsigned long _last_cov_time;
#endif
};

#endif // BACNET_OBJECT_BINARY_VALUE

#endif // BACNET_BINARY_VALUE_H
