/*
 * BACnetDevice.cpp - BACnet Device Object implementation
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 */

#include "BACnetDevice.h"
#include "BACnetConfig.h"

// Include bacnet-stack C headers
extern "C" {
    #include "bacnet/basic/sys/debug.h"
    #include "bacnet/datalink/datalink.h"
    #include "bacnet/datalink/dlmstp.h"
    #include "bacnet/basic/services.h"
    #include "bacnet/basic/tsm/tsm.h"
    #include "bacnet/basic/object/device.h"
}

// Constructor
BACnetDevice::BACnetDevice(uint8_t mac_address, uint32_t device_instance, uint32_t baud_rate)
    : _mac_address(mac_address)
    , _device_instance(device_instance)
    , _baud_rate(baud_rate)
    , _initialized(false)
    , _object_count(0)
{
    // Initialize object list
    for (uint8_t i = 0; i < MAX_BACNET_OBJECTS; i++) {
        _objects[i] = nullptr;
    }
    
    // Set default device properties
    snprintf(_device_name, sizeof(_device_name), "Arduino BACnet Device");
    snprintf(_location, sizeof(_location), "Arduino");
    snprintf(_description, sizeof(_description), BOARD_NAME);
}

void BACnetDevice::begin() {
    if (_initialized) {
        return;  // Already initialized
    }
    
    #if BACNET_DEBUG
    Serial.println(F("Initializing BACnet Device..."));
    #endif
    
    // Initialize the BACnet protocol stack
    initializeStack();
    initializeDatalink();
    initializeDevice();
    
    _initialized = true;
    
    #if BACNET_DEBUG
    printConfig();
    Serial.println(F("BACnet Device Ready!"));
    #endif
}

void BACnetDevice::task() {
    if (!_initialized) {
        return;
    }
    
    // Process datalink layer (MS/TP)
    datalink_receive(NULL);
    datalink_maintenance_timer(1);  // 1ms tick
    
    // Process TSM (Transaction State Machine)
    tsm_timer_milliseconds(1);
    
    // Update all objects
    for (uint8_t i = 0; i < _object_count; i++) {
        if (_objects[i] != nullptr) {
            _objects[i]->update();
        }
    }
}

bool BACnetDevice::addObject(BACnetObject* object) {
    if (object == nullptr) {
        return false;
    }
    
    if (_object_count >= MAX_BACNET_OBJECTS) {
        #if BACNET_DEBUG
        Serial.println(F("ERROR: Maximum object count reached"));
        #endif
        return false;
    }
    
    _objects[_object_count++] = object;
    
    #if BACNET_DEBUG
    Serial.print(F("Added object: "));
    Serial.print(object->getName());
    Serial.print(F(" (Instance: "));
    Serial.print(object->getInstance());
    Serial.println(F(")"));
    #endif
    
    return true;
}

void BACnetDevice::removeObject(BACnetObject* object) {
    if (object == nullptr) {
        return;
    }
    
    // Find and remove object
    for (uint8_t i = 0; i < _object_count; i++) {
        if (_objects[i] == object) {
            // Shift remaining objects
            for (uint8_t j = i; j < _object_count - 1; j++) {
                _objects[j] = _objects[j + 1];
            }
            _objects[_object_count - 1] = nullptr;
            _object_count--;
            break;
        }
    }
}

void BACnetDevice::setDeviceName(const char* name) {
    if (name != nullptr) {
        strncpy(_device_name, name, sizeof(_device_name) - 1);
        _device_name[sizeof(_device_name) - 1] = '\0';
    }
}

void BACnetDevice::setLocation(const char* location) {
    if (location != nullptr) {
        strncpy(_location, location, sizeof(_location) - 1);
        _location[sizeof(_location) - 1] = '\0';
    }
}

void BACnetDevice::setDescription(const char* description) {
    if (description != nullptr) {
        strncpy(_description, description, sizeof(_description) - 1);
        _description[sizeof(_description) - 1] = '\0';
    }
}

void BACnetDevice::printConfig() {
    printBACnetConfig();  // From BACnetConfig.h
    
    Serial.println(F("Device Configuration:"));
    Serial.print(F("  MAC Address: "));
    Serial.println(_mac_address);
    Serial.print(F("  Device Instance: "));
    Serial.println(_device_instance);
    Serial.print(F("  Baud Rate: "));
    Serial.println(_baud_rate);
    Serial.print(F("  Objects: "));
    Serial.print(_object_count);
    Serial.print(F("/"));
    Serial.println(MAX_BACNET_OBJECTS);
}

bool BACnetDevice::isObjectTypeAvailable(uint16_t object_type) {
    // Check based on BACnetConfig.h definitions
    switch (object_type) {
        case 1:  // OBJECT_ANALOG_INPUT
            return BACNET_OBJECT_ANALOG_INPUT;
        case 2:  // OBJECT_ANALOG_OUTPUT
            return BACNET_OBJECT_ANALOG_OUTPUT;
        case 3:  // OBJECT_ANALOG_VALUE
            return BACNET_OBJECT_ANALOG_VALUE;
        case 4:  // OBJECT_BINARY_INPUT
            return BACNET_OBJECT_BINARY_INPUT;
        case 5:  // OBJECT_BINARY_OUTPUT
            return BACNET_OBJECT_BINARY_OUTPUT;
        case 19: // OBJECT_BINARY_VALUE
            return BACNET_OBJECT_BINARY_VALUE;
        case 13: // OBJECT_MULTI_STATE_INPUT
        case 14: // OBJECT_MULTI_STATE_OUTPUT
        case 19: // OBJECT_MULTI_STATE_VALUE
            return BACNET_OBJECT_MULTI_STATE_VALUE;
        default:
            return false;
    }
}

// Private initialization functions
void BACnetDevice::initializeStack() {
    // Initialize datalink layer
    datalink_set_interface(DATALINK_MSTP);
    
    // Initialize MS/TP parameters
    dlmstp_set_mac_address(_mac_address);
    dlmstp_set_baud_rate(_baud_rate);
    dlmstp_set_max_info_frames(1);
    dlmstp_set_max_master(127);
    
    // Initialize TSM
    tsm_init();
}

void BACnetDevice::initializeDatalink() {
    // Initialize the datalink
    datalink_init(nullptr);
}

void BACnetDevice::initializeDevice() {
    // Set device properties
    Device_Set_Object_Instance_Number(_device_instance);
    
    // Note: Full device initialization will connect to
    // the bacnet-stack device.c functions in a future update
}

//=============================================================================
// BACnetObject base class implementation
//=============================================================================

BACnetObject::BACnetObject(uint32_t instance, const char* name)
    : _instance(instance)
{
    setName(name);
}

void BACnetObject::setName(const char* name) {
    if (name != nullptr) {
        strncpy(_name, name, sizeof(_name) - 1);
        _name[sizeof(_name) - 1] = '\0';
    } else {
        _name[0] = '\0';
    }
}
