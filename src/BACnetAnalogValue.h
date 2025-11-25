/*
 * BACnetAnalogValue.h - Analog Value Object wrapper
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 * 
 * Analog Value objects represent numeric values with engineering units.
 * They can be read and written by BACnet clients.
 */

#ifndef BACNET_ANALOG_VALUE_H
#define BACNET_ANALOG_VALUE_H

#include <Arduino.h>
#include "BACnetDevice.h"
#include "BACnetConfig.h"

// Common engineering units
typedef enum {
    UNITS_DEGREES_CELSIUS = 62,
    UNITS_DEGREES_FAHRENHEIT = 64,
    UNITS_KELVIN = 63,
    UNITS_PERCENT = 98,
    UNITS_VOLTS = 5,
    UNITS_AMPERES = 2,
    UNITS_WATTS = 48,
    UNITS_KILOWATTS = 132,
    UNITS_METERS = 47,
    UNITS_FEET = 45,
    UNITS_LITERS = 57,
    UNITS_GALLONS = 89,
    UNITS_NO_UNITS = 95
} BACnetEngineeringUnits;

class BACnetAnalogValue : public BACnetObject {
public:
    /**
     * Create an Analog Value object
     * @param instance Object instance number (0-4194303)
     * @param name Object name (max 31 characters)
     * @param units Engineering units (default: degrees Celsius)
     */
    BACnetAnalogValue(uint32_t instance, 
                      const char* name,
                      BACnetEngineeringUnits units = UNITS_DEGREES_CELSIUS);
    
    /**
     * Get the BACnet object type
     */
    uint16_t getObjectType() const override;
    
    /**
     * Set the present value
     * @param value Floating point value
     */
    void setValue(float value);
    
    /**
     * Get the present value
     * @return Current value
     */
    float getValue() const { return _present_value; }
    
    /**
     * Set engineering units
     * @param units Engineering units enum
     */
    void setUnits(BACnetEngineeringUnits units);
    
    /**
     * Get engineering units
     */
    BACnetEngineeringUnits getUnits() const { return _units; }
    
    /**
     * Set the resolution (smallest change detected)
     * @param resolution Resolution value (e.g., 0.1 for 1 decimal place)
     */
    void setResolution(float resolution);
    
    /**
     * Get the resolution
     */
    float getResolution() const { return _resolution; }
    
    /**
     * Set minimum value
     * @param min Minimum allowed value
     */
    void setMinValue(float min);
    
    /**
     * Get minimum value
     */
    float getMinValue() const { return _min_value; }
    
    /**
     * Set maximum value
     * @param max Maximum allowed value
     */
    void setMaxValue(float max);
    
    /**
     * Get maximum value
     */
    float getMaxValue() const { return _max_value; }
    
    /**
     * Set the Arduino analog input pin for this object
     * @param pin Analog pin number (A0, A1, etc.)
     */
    void setPin(int8_t pin);
    
    /**
     * Get the Arduino pin number
     * @return Pin number or -1 if not assigned
     */
    int8_t getPin() const { return _pin; }
    
    /**
     * Read the analog pin value and update Present_Value
     * Maps ADC reading (0-1023) to min/max range
     * Only works if pin is configured
     */
    void readPin();
    
    /**
     * Read the analog pin as voltage (assuming 5V reference)
     * @return Voltage (0.0 - 5.0V)
     */
    float readPinVoltage();

#if BACNET_FEATURE_COV
    /**
     * Enable Change of Value (COV) notifications for this object
     * @param increment COV increment threshold (default: 1.0)
     * Requires Tier 2+ board (Mega, Due, ESP32)
     */
    void enableCOV(float increment = 1.0);
    
    /**
     * Disable COV notifications
     */
    void disableCOV();
    
    /**
     * Check if COV is enabled
     */
    bool isCOVEnabled() const { return _cov_enabled; }
    
    /**
     * Get COV increment
     */
    float getCOVIncrement() const { return _cov_increment; }
#endif

    /**
     * Update function called periodically
     */
    void update() override;

private:
    float _present_value;
    float _previous_value;
    BACnetEngineeringUnits _units;
    float _resolution;
    float _min_value;
    float _max_value;
    int8_t _pin;
    
#if BACNET_FEATURE_COV
    bool _cov_enabled;
    float _cov_increment;
    unsigned long _last_cov_time;
#endif
};

#endif // BACNET_ANALOG_VALUE_H
