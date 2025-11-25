/*
 * BACnetArduino.h - Main header for BACnet-for-Arduino library
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 * 
 * This is the main include file for users of the BACnet-for-Arduino library.
 * Include this in your Arduino sketch to access all BACnet functionality.
 * 
 * Based on BACnet Protocol Stack by Steve Karg
 * https://github.com/bacnet-stack/bacnet-stack
 */

#ifndef BACNET_ARDUINO_H
#define BACNET_ARDUINO_H

#include <Arduino.h>
#include "BACnetConfig.h"
#include "BACnetDevice.h"
#include "BACnetBinaryValue.h"
#include "BACnetAnalogValue.h"

// Optional objects (available on Tier 2+ boards)
#if BACNET_OBJECT_BINARY_OUTPUT
#include "BACnetBinaryOutput.h"
#endif

#if BACNET_OBJECT_ANALOG_INPUT
#include "BACnetAnalogInput.h"
#endif

#if BACNET_OBJECT_MULTI_STATE_VALUE
#include "BACnetMultiStateValue.h"
#endif

// Hardware abstraction
#include "hardware/BACnetRS485.h"
#include "hardware/BACnetTimer.h"

// Version information
#define BACNET_ARDUINO_VERSION "1.0.0"
#define BACNET_ARDUINO_VERSION_MAJOR 1
#define BACNET_ARDUINO_VERSION_MINOR 0
#define BACNET_ARDUINO_VERSION_PATCH 0

#endif // BACNET_ARDUINO_H
