/**
 * @file BV_LED_Control.ino
 * @brief Control Arduino LED via BACnet Binary Value WriteProperty
 * 
 * This example demonstrates:
 * - Creating a BACnet device with one Binary Value object
 * - Controlling Arduino's built-in LED (pin 13) via BACnet
 * - Reading and writing Present_Value property
 * - Testing with YABE BACnet Explorer
 * 
 * Hardware Requirements:
 * - Arduino Mega 2560 (or Uno/Due/ESP32)
 * - RS-485 transceiver module (MAX485 or auto-direction)
 * - Built-in LED on pin 13 (no external wiring needed)
 * 
 * RS-485 Wiring (for Mega):
 * - See BACnetConfig.h for default pin assignments
 * - Mega: Serial1 (TX1=D18, RX1=D19) for BACnet MS/TP
 * - RS485 enable pin: D2 (or -1 for auto-direction modules)
 * 
 * BACnet Configuration:
 * - Device Instance: 260001
 * - MAC Address: 10
 * - Object: Binary Value 0 (LED)
 * - Baud Rate: 38400
 * 
 * Testing with YABE:
 * 1. Connect RS-485 bus to PC (USB-RS485 adapter)
 * 2. Launch YABE, add MS/TP device on correct COM port
 * 3. Set baud rate to 38400
 * 4. Discover devices - you should see Device 260001
 * 5. Expand device tree, find Binary Value 0 (LED)
 * 6. Write ACTIVE to Present_Value -> LED turns ON
 * 7. Write INACTIVE to Present_Value -> LED turns OFF
 * 
 * Expected RAM Usage:
 * - Arduino Uno: ~1.5 KB
 * - Arduino Mega: ~1.5 KB
 * 
 * @author George Arun <argeorun@gmail.com>
 * @date 2025-11-26
 * @license MIT
 */

#include <BACnetArduino.h>

// BACnet Device Configuration
// MAC Address must be unique on the MS/TP network (0-127)
#define BACNET_MAC_ADDRESS 10

// Device Instance must be unique on the BACnet network (0-4194303)
#define BACNET_DEVICE_INSTANCE 260001

// Create BACnet device
BACnetDevice device(BACNET_MAC_ADDRESS, BACNET_DEVICE_INSTANCE);

// Create Binary Value object for LED control
// Parameters: instance, name, pin
// - Instance 0: First Binary Value object
// - Name "LED": Descriptive name visible in YABE
// - Pin 13: Arduino's built-in LED
BACnetBinaryValue led(0, "LED", LED_BUILTIN);

void setup() {
    // Initialize debug serial (automatically configured per board)
    // Mega: Serial (USB) for debug
    // Uno: Debug disabled (saves RAM)
    BACNET_DEBUG_BEGIN(9600);
    
    BACNET_DEBUG_PRINTLN(F("================================="));
    BACNET_DEBUG_PRINTLN(F("BACnet LED Control Example"));
    BACNET_DEBUG_PRINTLN(F("================================="));
    
    // Initialize BACnet device
    // This will:
    // - Display board configuration (RAM, tier, enabled objects)
    // - Initialize BACnet protocol stack
    // - Configure RS-485 communication
    // - Start MS/TP datalink layer
    device.begin();
    
    // Configure device properties (optional but recommended)
    device.setDeviceName("Arduino LED Controller");
    device.setDescription("BACnet LED control demonstration");
    device.setLocation("Development Board");
    
    // Register LED object with device
    if (device.addObject(&led)) {
        BACNET_DEBUG_PRINTLN(F("LED object registered successfully"));
    } else {
        BACNET_DEBUG_PRINTLN(F("ERROR: Failed to register LED object"));
    }
    
    BACNET_DEBUG_PRINTLN(F("\nDevice Ready!"));
    BACNET_DEBUG_PRINTLN(F("Waiting for BACnet commands..."));
    BACNET_DEBUG_PRINTLN(F("\nTest with YABE:"));
    BACNET_DEBUG_PRINTLN(F("1. Add MS/TP device (baud 38400)"));
    BACNET_DEBUG_PRINTLN(F("2. Discover devices"));
    BACNET_DEBUG_PRINTLN(F("3. Find Binary Value 0 (LED)"));
    BACNET_DEBUG_PRINTLN(F("4. Write ACTIVE/INACTIVE to Present_Value"));
    BACNET_DEBUG_PRINTLN(F("=================================\n"));
}

void loop() {
    // Process BACnet communications
    // This MUST be called continuously in loop()
    // Handles:
    // - Incoming BACnet messages
    // - ReadProperty requests
    // - WriteProperty requests
    // - Who-Is/I-Am discovery
    // - COV notifications (if enabled)
    device.task();
    
    // Optional: Print LED state changes for debugging
    static bool lastState = false;
    bool currentState = led.getPresentValue();
    
    if (currentState != lastState) {
        lastState = currentState;
        BACNET_DEBUG_PRINT(F("LED state changed: "));
        BACNET_DEBUG_PRINTLN(currentState ? F("ON") : F("OFF"));
    }
    
    // Note: The LED is controlled automatically by BACnet WriteProperty
    // You don't need to call digitalWrite() - the BACnetBinaryValue class
    // handles pin control when Present_Value is written via BACnet
}
