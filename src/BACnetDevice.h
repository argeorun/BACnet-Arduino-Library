/*
 * BACnetDevice.h - BACnet Device Object wrapper class
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 * 
 * This class wraps the BACnet Device Object and provides the main
 * interface for BACnet communication in Arduino sketches.
 */

#ifndef BACNET_DEVICE_H
#define BACNET_DEVICE_H

#include <Arduino.h>
#include "BACnetConfig.h"

// Forward declarations for BACnet objects
class BACnetObject;

class BACnetDevice {
public:
    /**
     * Create a BACnet Device
     * @param mac_address MS/TP MAC address (0-127)
     * @param device_instance Device Instance ID (0-4194303)
     * @param baud_rate Serial baud rate (9600, 19200, 38400, 76800)
     */
    BACnetDevice(uint8_t mac_address, 
                 uint32_t device_instance = 260001,
                 uint32_t baud_rate = 38400);
    
    /**
     * Initialize the BACnet device and protocol stack
     * Call this in setup() before adding objects
     */
    void begin();
    
    /**
     * Process BACnet communications
     * Call this repeatedly in loop() - it's non-blocking
     */
    void task();
    
    // Alternative name for task()
    void process() { task(); }
    
    /**
     * Add a BACnet object to this device
     * @param object Pointer to BACnet object (BV, AV, etc.)
     * @return true if added successfully
     */
    bool addObject(BACnetObject* object);
    
    /**
     * Remove a BACnet object from this device
     * @param object Pointer to BACnet object to remove
     */
    void removeObject(BACnetObject* object);
    
    /**
     * Set the device name (Object_Name property)
     * @param name Device name string (max 32 characters)
     */
    void setDeviceName(const char* name);
    
    /**
     * Set the device location
     * @param location Location string (max 64 characters)
     */
    void setLocation(const char* location);
    
    /**
     * Set the device description
     * @param description Description string (max 64 characters)
     */
    void setDescription(const char* description);
    
    /**
     * Get the MAC address
     */
    uint8_t getMacAddress() const { return _mac_address; }
    
    /**
     * Get the Device Instance ID
     */
    uint32_t getDeviceInstance() const { return _device_instance; }
    
    /**
     * Print configuration to Serial
     * Shows board tier, RAM, MAX_APDU, enabled features
     */
    void printConfig();
    
    /**
     * Check if an object type is available on this board
     * @param object_type BACnet object type (use BACNET_OBJECT_xxx constants)
     * @return true if object type is enabled for this board tier
     */
    static bool isObjectTypeAvailable(uint16_t object_type);

private:
    uint8_t _mac_address;
    uint32_t _device_instance;
    uint32_t _baud_rate;
    bool _initialized;
    
    // Object list
    BACnetObject* _objects[MAX_BACNET_OBJECTS];
    uint8_t _object_count;
    
    // Device properties
    char _device_name[32];
    char _location[64];
    char _description[64];
    
    // Internal initialization functions
    void initializeStack();
    void initializeDatalink();
    void initializeDevice();
};

/**
 * Base class for all BACnet objects
 * Do not instantiate directly - use specific object classes
 */
class BACnetObject {
public:
    virtual ~BACnetObject() {}
    
    /**
     * Get the object instance number
     */
    uint32_t getInstance() const { return _instance; }
    
    /**
     * Get the BACnet object type
     */
    virtual uint16_t getObjectType() const = 0;
    
    /**
     * Get the object name
     */
    const char* getName() const { return _name; }
    
    /**
     * Set the object name
     */
    void setName(const char* name);
    
    /**
     * Update the object (called periodically)
     * Override in derived classes for object-specific updates
     */
    virtual void update() {}

protected:
    BACnetObject(uint32_t instance, const char* name);
    
    uint32_t _instance;
    char _name[32];
};

#endif // BACNET_DEVICE_H
