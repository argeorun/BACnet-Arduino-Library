/**
 * @file BasicDevice.ino
 * @brief Basic BACnet Device Example - Minimal setup for testing
 * 
 * This example demonstrates the minimal code needed to create a BACnet device.
 * Used for compilation verification across different Arduino boards.
 * 
 * @author George Arun <argeorun@gmail.com>
 * @date 2025-11-26
 * @license MIT
 */

#include <BACnetArduino.h>

// Create BACnet device with default MAC address 1
BACnetDevice device(1);

void setup() {
    // Initialize debug serial (automatically disabled on UNO)
    BACNET_DEBUG_BEGIN(9600);
    BACNET_DEBUG_PRINTLN(F("BACnet Basic Device Test"));
    
    // Initialize BACnet device
    device.begin();
    
    BACNET_DEBUG_PRINTLN(F("Setup complete"));
}

void loop() {
    // Run BACnet task
    device.task();
}
