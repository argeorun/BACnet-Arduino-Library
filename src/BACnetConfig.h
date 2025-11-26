/*
 * BACnetConfig.h - Automatic Board Detection & Memory Configuration
 * Part of BACnet-for-Arduino library
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License (see LICENSE file)
 * 
 * This file provides automatic board detection and proportional memory scaling
 * for BACnet protocol stack across different Arduino boards.
 */

#ifndef BACNET_CONFIG_H
#define BACNET_CONFIG_H

#include <Arduino.h>

/*=============================================================================
 * AUTOMATIC BOARD DETECTION
 *============================================================================*/

// Detect board type and assign RAM tier
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
    #define BOARD_NAME "Arduino Uno/Nano"
    #define BOARD_RAM_KB 2
    #define BOARD_TIER 1
    #define BOARD_TIER_NAME "Tier 1 (Minimal)"

#elif defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
    #define BOARD_NAME "Arduino Mega 2560"
    #define BOARD_RAM_KB 8
    #define BOARD_TIER 2
    #define BOARD_TIER_NAME "Tier 2 (Standard)"

#elif defined(ARDUINO_SAM_DUE)
    #define BOARD_NAME "Arduino Due"
    #define BOARD_RAM_KB 96
    #define BOARD_TIER 3
    #define BOARD_TIER_NAME "Tier 3 (Advanced)"

#elif defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_ARCH_SAMD)
    #define BOARD_NAME "Arduino Zero/SAMD"
    #define BOARD_RAM_KB 32
    #define BOARD_TIER 3
    #define BOARD_TIER_NAME "Tier 3 (Advanced)"

#elif defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
    #define BOARD_NAME "ESP32"
    #define BOARD_RAM_KB 520
    #define BOARD_TIER 4
    #define BOARD_TIER_NAME "Tier 4 (Full Featured)"

#elif defined(ARDUINO_ARCH_STM32) || defined(STM32F4)
    #define BOARD_NAME "STM32"
    #define BOARD_RAM_KB 128
    #define BOARD_TIER 4
    #define BOARD_TIER_NAME "Tier 4 (Full Featured)"

#elif defined(TEENSYDUINO)
    #if defined(__MK20DX256__)
        #define BOARD_NAME "Teensy 3.2"
        #define BOARD_RAM_KB 64
        #define BOARD_TIER 3
        #define BOARD_TIER_NAME "Tier 3 (Advanced)"
    #elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
        #define BOARD_NAME "Teensy 3.5/3.6"
        #define BOARD_RAM_KB 256
        #define BOARD_TIER 4
        #define BOARD_TIER_NAME "Tier 4 (Full Featured)"
    #else
        #define BOARD_NAME "Teensy (Unknown)"
        #define BOARD_RAM_KB 32
        #define BOARD_TIER 3
        #define BOARD_TIER_NAME "Tier 3 (Advanced)"
    #endif

#else
    // Unknown board - use conservative defaults
    #warning "Unknown board detected - using Tier 1 (minimal) configuration"
    #define BOARD_NAME "Unknown Board"
    #define BOARD_RAM_KB 2
    #define BOARD_TIER 1
    #define BOARD_TIER_NAME "Tier 1 (Minimal - Unknown Board)"
#endif

/*=============================================================================
 * PROPORTIONAL MEMORY SCALING
 * Formula: Multiplier = BOARD_RAM_KB / 2 (Uno baseline = 2KB)
 *============================================================================*/

#define RAM_MULTIPLIER (BOARD_RAM_KB / 2)

// MAX_APDU: Maximum APDU size (Application Protocol Data Unit)
// Uno: 128, Mega: 512, Due: 1476 (BACnet maximum)
#if BOARD_RAM_KB >= 32
    #define MAX_APDU 1476  // BACnet standard maximum
#elif BOARD_RAM_KB >= 8
    #define MAX_APDU (128 * RAM_MULTIPLIER)
#else
    #define MAX_APDU 128  // Minimum for Uno
#endif

// MAX_TSM_TRANSACTIONS: Maximum concurrent transactions
// Uno: 3, Mega: 12, Due: 48, ESP32: 255
#if BOARD_RAM_KB >= 64
    #define MAX_TSM_TRANSACTIONS 255  // Maximum
#elif BOARD_RAM_KB >= 32
    #define MAX_TSM_TRANSACTIONS (3 * RAM_MULTIPLIER)
#elif BOARD_RAM_KB >= 8
    #define MAX_TSM_TRANSACTIONS (3 * RAM_MULTIPLIER)
#else
    #define MAX_TSM_TRANSACTIONS 3  // Minimum for Uno
#endif

// BACNET_DATALINK_MAX_APDU: Datalink layer buffer
#define BACNET_DATALINK_MAX_APDU MAX_APDU

/*=============================================================================
 * SERIAL PORT CONFIGURATION - Easy Configuration Section
 * 
 * ⚠️ IMPORTANT: Verify pin numbers match YOUR specific board variant!
 * Default values work for most common boards, but some variants differ.
 * 
 * 🔧 TO CUSTOMIZE:
 * 1. Find your board section below
 * 2. Uncomment and edit the #define lines
 * 3. Verify pins against your board pinout diagram
 * 4. Save and recompile
 * 
 * 💡 If unsure, leave defaults - they work for standard boards
 *============================================================================*/

//-----------------------------------------------------------------------------
// ARDUINO UNO / NANO (Tier 1) - Only ONE hardware serial
//-----------------------------------------------------------------------------
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
    
    // ⚠️ UNO LIMITATION: Only Serial (TX=D1, RX=D0) available
    // BACnet MS/TP MUST use Serial - debug output DISABLED during operation
    
    #define BACNET_MSTP_SERIAL Serial
    #define BACNET_DEBUG_ENABLED 0  // Disabled - no second serial port
    
    // 🔧 OPTIONAL: Enable SoftwareSerial for debugging (uses more RAM ~100 bytes)
    // Uncomment to use SoftwareSerial for debug on D10(RX)/D11(TX)
    // #define BACNET_USE_SOFTWARE_SERIAL_DEBUG 1
    // #define BACNET_SOFTSERIAL_RX_PIN 10
    // #define BACNET_SOFTSERIAL_TX_PIN 11
    
    // RS485 Enable Pin (DE/RE control for MAX485)
    // Default: D2 - ⚠️ Verify matches your shield!
    // Common options: D2, D4, D8
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN 2  // Change if your shield uses different pin
    #endif
    
    // For auto-direction modules (no DE/RE control), set to -1:
    // #define BACNET_RS485_ENABLE_PIN -1

//-----------------------------------------------------------------------------
// ARDUINO MEGA 2560 (Tier 2) - Four hardware serials
//-----------------------------------------------------------------------------
#elif defined(ARDUINO_AVR_MEGA2560)
    
    // Default Configuration:
    //   Serial  = USB Debug (TX0/RX0 = pins 1/0)
    //   Serial1 = BACnet MS/TP (TX1/RX1 = pins 18/19) ⚠️ VERIFY YOUR BOARD!
    //   Serial2 = Available (TX2/RX2 = pins 16/17)
    //   Serial3 = Available (TX3/RX3 = pins 14/15)
    
    #ifndef BACNET_DEBUG_SERIAL
        #define BACNET_DEBUG_SERIAL Serial   // USB Debug
    #endif
    #define BACNET_DEBUG_ENABLED 1
    
    #ifndef BACNET_MSTP_SERIAL
        #define BACNET_MSTP_SERIAL Serial1   // BACnet on Serial1 (pins 18/19)
    #endif
    
    // 🔧 TO SWAP: Uncomment to use Serial for BACnet, Serial1 for Debug
    // #define BACNET_DEBUG_SERIAL Serial1
    // #define BACNET_MSTP_SERIAL Serial
    
    // 🔧 TO USE Serial2 or Serial3:
    // #define BACNET_MSTP_SERIAL Serial2  // Pins 16/17
    // #define BACNET_MSTP_SERIAL Serial3  // Pins 14/15
    
    // RS485 Enable Pin Configuration (Applies to ALL Arduino boards)
    // ⚠️ VERIFY: Check your RS485 shield documentation!
    //
    // Set to -1 for auto-direction modules (no DE/RE control needed)
    // Set to pin number (e.g., 2, 4, 8) for modules requiring manual DE/RE control
    //
    // Note: This setting is based on your RS485 module type, not the Arduino board
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN -1  // Auto-direction module (no DE/RE)
    #endif

//-----------------------------------------------------------------------------
// ARDUINO DUE / ZERO (Tier 3) - Multiple serials
//-----------------------------------------------------------------------------
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAMD_ZERO)
    
    // Default Configuration:
    //   SerialUSB = USB Debug (Native USB port)
    //   Serial1   = BACnet MS/TP (TX1/RX1 = pins 18/19) ⚠️ VERIFY!
    
    #ifndef BACNET_DEBUG_SERIAL
        #define BACNET_DEBUG_SERIAL SerialUSB
    #endif
    #define BACNET_DEBUG_ENABLED 1
    
    #ifndef BACNET_MSTP_SERIAL
        #define BACNET_MSTP_SERIAL Serial1
    #endif
    
    // RS485 Enable Pin - Default: D2
    // ⚠️ Due/Zero: 3.3V logic! Ensure RS485 module is 3.3V compatible
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN 2
    #endif

//-----------------------------------------------------------------------------
// ESP32 (Tier 4) - Multiple UARTs with flexible pin assignment
//-----------------------------------------------------------------------------
#elif defined(ARDUINO_ARCH_ESP32)
    
    // Default Configuration:
    //   Serial  = USB Debug (USB CDC)
    //   Serial1 = BACnet MS/TP (default RX=GPIO16, TX=GPIO17)
    //   Serial2 = Available
    
    #ifndef BACNET_DEBUG_SERIAL
        #define BACNET_DEBUG_SERIAL Serial
    #endif
    #define BACNET_DEBUG_ENABLED 1
    
    #ifndef BACNET_MSTP_SERIAL
        #define BACNET_MSTP_SERIAL Serial1
    #endif
    
    // 🔧 ESP32 Custom Serial Pins (optional)
    // Default Serial1: RX=16, TX=17 (can be changed in sketch with Serial1.begin())
    // #define BACNET_ESP32_RX_PIN 16
    // #define BACNET_ESP32_TX_PIN 17
    
    // RS485 Enable Pin - Default: GPIO4
    // ⚠️ ESP32: Verify pin doesn't conflict with strapping pins!
    // Avoid: GPIO0, GPIO2, GPIO5, GPIO12, GPIO15 (boot mode pins)
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN 4  // Safe default for most ESP32 boards
    #endif

//-----------------------------------------------------------------------------
// STM32 (Tier 4) - USART configuration varies by board
//-----------------------------------------------------------------------------
#elif defined(ARDUINO_ARCH_STM32)
    
    // Default Configuration:
    //   Serial  = USB Debug (via ST-Link or USB CDC)
    //   Serial1 = USART1 for BACnet
    // 
    // ⚠️ CRITICAL: STM32 pin mapping varies significantly between boards!
    // Common boards:
    //   - Blue Pill (STM32F103C8):  USART1 = PA9(TX), PA10(RX)
    //   - Black Pill (STM32F411CE): USART1 = PA9(TX), PA10(RX)
    //   - Nucleo boards: Check your specific board pinout!
    
    #ifndef BACNET_DEBUG_SERIAL
        #define BACNET_DEBUG_SERIAL Serial
    #endif
    #define BACNET_DEBUG_ENABLED 1
    
    #ifndef BACNET_MSTP_SERIAL
        #define BACNET_MSTP_SERIAL Serial1  // USART1
    #endif
    
    // 🔧 TO USE USART2 or USART3:
    // #define BACNET_MSTP_SERIAL Serial2  // USART2 (check pins!)
    // #define BACNET_MSTP_SERIAL Serial3  // USART3 (check pins!)
    
    // RS485 Enable Pin - Default: PA1
    // ⚠️ VERIFY: STM32 pin names use PXn format (e.g., PA1, PB5, PC13)
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN PA1  // Change to match your wiring
    #endif

//-----------------------------------------------------------------------------
// Generic AVR fallback
//-----------------------------------------------------------------------------
#elif defined(__AVR__)
    // Assume Uno-like configuration for unknown AVR boards
    #define BACNET_MSTP_SERIAL Serial
    #define BACNET_DEBUG_ENABLED 0
    #ifndef BACNET_RS485_ENABLE_PIN
        #define BACNET_RS485_ENABLE_PIN 2
    #endif
    #warning "Unknown AVR board - using Uno-like serial configuration. Verify pins!"

//-----------------------------------------------------------------------------
// Unknown board
//-----------------------------------------------------------------------------
#else
    #error "Unknown board! Please configure serial ports manually in BACnetConfig.h"
#endif

//-----------------------------------------------------------------------------
// Debug Helper Macros - Automatically handles enabled/disabled state
//-----------------------------------------------------------------------------
#if BACNET_DEBUG_ENABLED
    #define BACNET_DEBUG_BEGIN(baud) BACNET_DEBUG_SERIAL.begin(baud)
    #define BACNET_DEBUG_PRINT(x) BACNET_DEBUG_SERIAL.print(x)
    #define BACNET_DEBUG_PRINTLN(x) BACNET_DEBUG_SERIAL.println(x)
    #if defined(ARDUINO_ARCH_ESP32)
        #define BACNET_DEBUG_PRINTF(...) BACNET_DEBUG_SERIAL.printf(__VA_ARGS__)
    #else
        #define BACNET_DEBUG_PRINTF(...)  // Printf not available on AVR
    #endif
#else
    #define BACNET_DEBUG_BEGIN(baud)
    #define BACNET_DEBUG_PRINT(x)
    #define BACNET_DEBUG_PRINTLN(x)
    #define BACNET_DEBUG_PRINTF(...)
#endif

//-----------------------------------------------------------------------------
// Configuration Validation (Compile-time error checking)
//-----------------------------------------------------------------------------
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
    // Check if user accidentally enabled debug on Serial for Uno
    #if defined(BACNET_DEBUG_SERIAL) && !defined(BACNET_USE_SOFTWARE_SERIAL_DEBUG)
        // This is expected - debug disabled on Uno
    #endif
#endif

// Validate RS485 enable pin is defined
#ifndef BACNET_RS485_ENABLE_PIN
    #error "BACNET_RS485_ENABLE_PIN not defined! Check BACnetConfig.h serial configuration."
#endif

// Validate MS/TP serial is defined
#ifndef BACNET_MSTP_SERIAL
    #error "BACNET_MSTP_SERIAL not defined! Check BACnetConfig.h serial configuration."
#endif

/*=============================================================================
 * TIER-BASED OBJECT TYPE ENABLEMENT
 *============================================================================*/

// Tier 1: Basic objects (Always enabled on all boards)
#define BACNET_OBJECT_DEVICE 1
#define BACNET_OBJECT_BINARY_VALUE 1
#define BACNET_OBJECT_ANALOG_VALUE 1

// Tier 2: Standard I/O objects (Mega 2560 and above)
#if BOARD_TIER >= 2
    #define BACNET_OBJECT_BINARY_OUTPUT 1
    #define BACNET_OBJECT_ANALOG_INPUT 1
    #define BACNET_OBJECT_MULTI_STATE_VALUE 1
    #define BACNET_OBJECT_BINARY_INPUT 1
    #define BACNET_OBJECT_ANALOG_OUTPUT 1
    #define BACNET_OBJECT_MULTI_STATE_OUTPUT 1
#else
    #define BACNET_OBJECT_BINARY_OUTPUT 0
    #define BACNET_OBJECT_ANALOG_INPUT 0
    #define BACNET_OBJECT_MULTI_STATE_VALUE 0
    #define BACNET_OBJECT_BINARY_INPUT 0
    #define BACNET_OBJECT_ANALOG_OUTPUT 0
    #define BACNET_OBJECT_MULTI_STATE_OUTPUT 0
#endif

// Tier 3: Advanced objects (Due, Zero and above)
#if BOARD_TIER >= 3
    #define BACNET_OBJECT_CALENDAR 1
    #define BACNET_OBJECT_SCHEDULE 1
    #define BACNET_OBJECT_TREND_LOG 1
    #define BACNET_OBJECT_NOTIFICATION_CLASS 1
    #define BACNET_OBJECT_FILE 1
#else
    #define BACNET_OBJECT_CALENDAR 0
    #define BACNET_OBJECT_SCHEDULE 0
    #define BACNET_OBJECT_TREND_LOG 0
    #define BACNET_OBJECT_NOTIFICATION_CLASS 0
    #define BACNET_OBJECT_FILE 0
#endif

// Tier 4: Full feature set (ESP32, STM32, large boards)
#if BOARD_TIER >= 4
    #define BACNET_OBJECT_ACCUMULATOR 1
    #define BACNET_OBJECT_LOOP 1
    #define BACNET_OBJECT_AVERAGING 1
    #define BACNET_OBJECT_COMMAND 1
#else
    #define BACNET_OBJECT_ACCUMULATOR 0
    #define BACNET_OBJECT_LOOP 0
    #define BACNET_OBJECT_AVERAGING 0
    #define BACNET_OBJECT_COMMAND 0
#endif

/*=============================================================================
 * TIER-BASED FEATURE ENABLEMENT
 *============================================================================*/

// Tier 1 Features (Uno): Basic Read/Write only
#define BACNET_FEATURE_READ_PROPERTY 1
#define BACNET_FEATURE_WRITE_PROPERTY 1
#define BACNET_FEATURE_WHO_IS 1
#define BACNET_FEATURE_I_AM 1

// Tier 2 Features (Mega): Add COV and Priority
#if BOARD_TIER >= 2
    #define BACNET_FEATURE_COV 1
    #define BACNET_FEATURE_PRIORITY_ARRAY 1
    #define BACNET_FEATURE_READ_PROPERTY_MULTIPLE 1
    #define BACNET_FEATURE_WRITE_PROPERTY_MULTIPLE 1
#else
    #define BACNET_FEATURE_COV 0
    #define BACNET_FEATURE_PRIORITY_ARRAY 0
    #define BACNET_FEATURE_READ_PROPERTY_MULTIPLE 0
    #define BACNET_FEATURE_WRITE_PROPERTY_MULTIPLE 0
#endif

// Tier 3 Features (Due): Add Scheduling and Trending
#if BOARD_TIER >= 3
    #define BACNET_FEATURE_INTRINSIC_REPORTING 1
    #define BACNET_FEATURE_TREND_LOG 1
    #define BACNET_FEATURE_SCHEDULE 1
    #define BACNET_FEATURE_CALENDAR 1
#else
    #define BACNET_FEATURE_INTRINSIC_REPORTING 0
    #define BACNET_FEATURE_TREND_LOG 0
    #define BACNET_FEATURE_SCHEDULE 0
    #define BACNET_FEATURE_CALENDAR 0
#endif

// Tier 4 Features (ESP32): Full BACnet feature set
#if BOARD_TIER >= 4
    #define BACNET_FEATURE_SECURE_CONNECT 1
    #define BACNET_FEATURE_NETWORK_PORT 1
    #define BACNET_FEATURE_ROUTING 1
#else
    #define BACNET_FEATURE_SECURE_CONNECT 0
    #define BACNET_FEATURE_NETWORK_PORT 0
    #define BACNET_FEATURE_ROUTING 0
#endif

/*=============================================================================
 * PROTOCOL CONFIGURATION
 *============================================================================*/

// MS/TP Datalink Layer
#define BACDL_MSTP 1

// Maximum number of objects (scaled by tier)
#if BOARD_TIER >= 4
    #define MAX_BACNET_OBJECTS 128
#elif BOARD_TIER >= 3
    #define MAX_BACNET_OBJECTS 64
#elif BOARD_TIER >= 2
    #define MAX_BACNET_OBJECTS 32
#else
    #define MAX_BACNET_OBJECTS 8  // Uno: Keep it minimal
#endif

// Maximum property list size (per object)
#if BOARD_TIER >= 3
    #define MAX_PROPERTY_LIST 64
#elif BOARD_TIER >= 2
    #define MAX_PROPERTY_LIST 32
#else
    #define MAX_PROPERTY_LIST 16
#endif

// COV Subscription limits (Tier 2+)
#if BOARD_TIER >= 3
    #define MAX_COV_SUBSCRIPTIONS 32
#elif BOARD_TIER >= 2
    #define MAX_COV_SUBSCRIPTIONS 8
#else
    #define MAX_COV_SUBSCRIPTIONS 0
#endif

/*=============================================================================
 * BUFFER SIZES (Proportional Scaling)
 *============================================================================*/

// MS/TP Frame buffer
#if BOARD_TIER >= 3
    #define MSTP_FRAME_COUNT 4
#elif BOARD_TIER >= 2
    #define MSTP_FRAME_COUNT 2
#else
    #define MSTP_FRAME_COUNT 1
#endif

// Receive buffer size
#define MSTP_RECEIVE_BUFFER_SIZE (MAX_APDU + 16)

// Transmit buffer size
#define MSTP_TRANSMIT_BUFFER_SIZE (MAX_APDU + 16)

/*=============================================================================
 * TIMING CONFIGURATION
 *============================================================================*/

// MS/TP Timing (microseconds)
#define MSTP_TURNAROUND_TIME_MS 10
#define MSTP_REPLY_TIMEOUT_MS 255

// COV lifetime (seconds) - Tier 2+
#if BOARD_TIER >= 2
    #define COV_DEFAULT_LIFETIME 300  // 5 minutes
#else
    #define COV_DEFAULT_LIFETIME 0
#endif

/*=============================================================================
 * DEBUG AND DIAGNOSTICS
 *============================================================================*/

// Enable debug output (disable on Uno to save RAM)
#if BOARD_TIER >= 2
    #define BACNET_DEBUG 1
#else
    #define BACNET_DEBUG 0
#endif

// Print configuration at startup
#if BACNET_DEBUG
    #define PRINT_CONFIG() printBACnetConfig()
#else
    #define PRINT_CONFIG() 
#endif

/*=============================================================================
 * HELPER MACROS
 *============================================================================*/

// Compile-time tier requirement check for examples
#define REQUIRE_TIER(n) \
    static_assert(BOARD_TIER >= n, \
        "This example requires Tier " #n " board or higher. " \
        "Your board: " BOARD_NAME " (Tier " TOSTRING(BOARD_TIER) "). " \
        "Please use Arduino Mega (Tier 2+), Due (Tier 3+), or ESP32 (Tier 4).")

#define TOSTRING(x) STRINGIFY(x)
#define STRINGIFY(x) #x

// Check if object type is available
#define IS_OBJECT_TYPE_AVAILABLE(obj_type) (BACNET_OBJECT_##obj_type == 1)

/*=============================================================================
 * CONFIGURATION PRINT FUNCTION
 *============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

// Print current configuration to Serial (for debugging)
static inline void printBACnetConfig(void) {
    Serial.println(F("=== BACnet Configuration ==="));
    Serial.print(F("Board: "));
    Serial.println(F(BOARD_NAME));
    Serial.print(F("RAM: "));
    Serial.print(BOARD_RAM_KB);
    Serial.println(F(" KB"));
    Serial.print(F("Tier: "));
    Serial.print(BOARD_TIER);
    Serial.print(F(" - "));
    Serial.println(F(BOARD_TIER_NAME));
    Serial.print(F("MAX_APDU: "));
    Serial.println(MAX_APDU);
    Serial.print(F("MAX_TSM: "));
    Serial.println(MAX_TSM_TRANSACTIONS);
    Serial.print(F("Max Objects: "));
    Serial.println(MAX_BACNET_OBJECTS);
    
    Serial.println(F("\nEnabled Object Types:"));
    Serial.print(F("  - Binary Value (BV): "));
    Serial.println(BACNET_OBJECT_BINARY_VALUE ? F("Yes") : F("No"));
    Serial.print(F("  - Analog Value (AV): "));
    Serial.println(BACNET_OBJECT_ANALOG_VALUE ? F("Yes") : F("No"));
    Serial.print(F("  - Binary Output (BO): "));
    Serial.println(BACNET_OBJECT_BINARY_OUTPUT ? F("Yes") : F("No"));
    Serial.print(F("  - Analog Input (AI): "));
    Serial.println(BACNET_OBJECT_ANALOG_INPUT ? F("Yes") : F("No"));
    Serial.print(F("  - Multi-State Value (MSV): "));
    Serial.println(BACNET_OBJECT_MULTI_STATE_VALUE ? F("Yes") : F("No"));
    
    #if BOARD_TIER >= 3
    Serial.print(F("  - Calendar: "));
    Serial.println(BACNET_OBJECT_CALENDAR ? F("Yes") : F("No"));
    Serial.print(F("  - Schedule: "));
    Serial.println(BACNET_OBJECT_SCHEDULE ? F("Yes") : F("No"));
    #endif
    
    Serial.println(F("\nEnabled Features:"));
    Serial.print(F("  - COV: "));
    Serial.println(BACNET_FEATURE_COV ? F("Yes") : F("No"));
    Serial.print(F("  - Priority Arrays: "));
    Serial.println(BACNET_FEATURE_PRIORITY_ARRAY ? F("Yes") : F("No"));
    
    Serial.println(F("============================\n"));
}

#ifdef __cplusplus
}
#endif

#endif // BACNET_CONFIG_H
