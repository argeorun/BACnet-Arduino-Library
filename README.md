# BACnet for Arduino

**A professional Arduino library for BACnet MS/TP protocol with automatic board detection and memory optimization**

1. OVERVIEW

BACnet for Arduino is a complete Arduino library that brings BACnet MS/TP protocol to your Arduino projects. Featuring automatic board detection, proportional memory scaling, and focused single-object examples, this library makes it easy to create BACnet-compatible devices on Arduino hardware ranging from Uno (2KB RAM) to ESP32 (520KB RAM).

<u>1A. Key Features</u>

- Automatic Board Detection - Detects Arduino board and configures memory usage automatically
- Proportional Memory Scaling - Larger boards get more buffers and features automatically  
- Progressive Enhancement - More RAM = more BACnet objects and features enabled
- Single-Object Examples - Each example focuses on ONE object type with ALL its features
- Arduino Library Manager - Standard installation via Library Manager
- Multi-Board Support - Uno, Mega, Due, Zero, ESP32, STM32, and more
- Educational - Clear, well-documented examples for learning BACnet

<u>1B. Supported BACnet Objects</u>

| Object Type           | Tier 1 (Uno) | Tier 2 (Mega) | Tier 3 (Due) | Tier 4 (ESP32) |
|-----------------------|--------------|---------------|--------------|----------------|
| Binary Value (BV)     | ✅           | ✅            | ✅           | ✅             |
| Analog Value (AV)     | ✅           | ✅            | ✅           | ✅             |
| Binary Output (BO)    | ❌           | ✅            | ✅           | ✅             |
| Analog Input (AI)     | ❌           | ✅            | ✅           | ✅             |
| Multi-State Value (MSV) | ❌         | ✅            | ✅           | ✅             |
| Calendar              | ❌           | ❌            | ✅           | ✅             |
| Schedule              | ❌           | ❌            | ✅           | ✅             |
| Trend Log             | ❌           | ❌            | ✅           | ✅             |

<u>1C. Board Tier System</u>

<u>Tier 1 (2KB RAM) - Arduino Uno, Nano</u>  
- Basic BV and AV objects only
- MAX_APDU: 128 bytes
- Perfect for simple devices

<u>Tier 2 (8KB RAM) - Arduino Mega 2560</u>

- All I/O objects (BO, AI, MSV)
- COV notifications
- Priority arrays
- MAX_APDU: 512 bytes

<u>Tier 3 (32KB RAM) - Arduino Due, Zero</u>

- Scheduling objects (Calendar, Schedule)
- Trend logging
- Intrinsic reporting
- MAX_APDU: 1476 bytes

<u>Tier 4 (64KB+ RAM) - ESP32, STM32</u>

- Full BACnet feature set
- Secure Connect (future)
- Complex multi-object systems

2. EXAMPLE SKETCHES

The library includes 50+ examples organized by object type and complexity:

<u>2A. Basic Examples (Tier 1 - All Boards)</u>

- MinimalDevice - Bare minimum BACnet device
- DeviceDiscovery - Who-Is/I-Am demo

<u>2B. Binary Value Examples (Tier 1)</u>

- BV_LED_Control - Control LED via BACnet
- BV_DigitalInput - Read button state
- BV_WithCOV - COV notifications (Tier 2+)

<u>2C. Analog Value Examples (Tier 1)</u>

- AV_Temperature - Read LM35 temperature sensor
- AV_ADC_Reading - Generic ADC input
- AV_WithCOV - COV notifications (Tier 2+)

<u>2D. Binary Output Examples (Tier 2 - Mega+)</u>

- BO_Relay_Control - Control relay with priority
- BO_Multiple - Multiple relays

<u>2E. Analog Input Examples (Tier 2 - Mega+)</u>

- AI_TempHumidity - DHT22 sensor
- AI_Multiple - Multiple sensors

<u>2F. Advanced Examples (Tier 3 - Due+)</u>

- Calendar_Simple - Calendar object
- Schedule_Simple - Schedule object
- TrendLog_Simple - Data logging

Each example includes:
- Complete circuit diagram
- Wiring instructions  
- Expected RAM/Flash usage
- YABE testing guide

3. QUICK START

<u>3A. Installation</u>

<u>Option 1: Arduino Library Manager (Recommended)</u>
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "BACnet for Arduino"
4. Click Install

<u>Option 2: Manual Installation</u>

1. Download the library ZIP from GitHub
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file

<u>3B. Your First BACnet Device (5 Minutes)</u>

```cpp
#include <BACnetArduino.h>

// Create device with MAC address 123
BACnetDevice device(123);

// Create Binary Value object (LED)
BACnetBinaryValue led(0, "LED", 13);  // Object ID 0, name "LED", pin 13

void setup() {
    Serial.begin(38400);
    device.begin();  // Displays board tier and configuration
    device.addObject(&led);
    Serial.println("BACnet Device Ready!");
}

void loop() {
    device.process();  // Handle BACnet communications
}
```

Upload this to your Arduino Uno with an RS-485 shield, and you'll have a BACnet device discoverable on the network!

<u>3C. Library Structure</u>

```
BACnet-for-Arduino/
├── library.properties           # Library metadata
├── keywords.txt                 # Syntax highlighting
├── LICENSE                      # MIT License (Arduino wrapper)
├── NOTICE.txt                   # Attribution to bacnet-stack
├── README.md                    # This file
│
├── src/                        # Library source code
│   ├── BACnetArduino.h         # Main library header (include this)
│   ├── BACnetDevice.h/cpp      # Device object class
│   ├── BACnetBinaryValue.h/cpp # Binary Value object
│   ├── BACnetAnalogValue.h/cpp # Analog Value object
│   ├── BACnetConfig.h          # Auto board detection
│   ├── hardware/               # Hardware abstraction
│   │   ├── BACnetRS485.h/cpp
│   │   ├── BACnetADC.h/cpp
│   │   └── BACnetTimer.h/cpp
│   └── bacnet/                 # BACnet protocol stack (original)
│
└── examples/                   # 50+ example sketches
    ├── 01.Basic/
    │   ├── MinimalDevice/
    │   └── DeviceDiscovery/
    ├── 02.BinaryValue/
    │   ├── BV_LED_Control/
    │   ├── BV_WithCOV/
    │   └── ...
    ├── 03.AnalogValue/
    ├── 04.BinaryOutput/
    ├── 05.AnalogInput/
    └── ...
```

4. HARDWARE REQUIREMENTS

<u>4A. RS-485 Transceiver Module</u>

BACnet MS/TP uses RS-485 for communication. You'll need an RS-485 transceiver:

<u>Recommended Modules:</u>

- DFRobot DFR0259 RS-485 Shield (Arduino Uno/Mega) - DE/RE control
- Generic MAX485 module (any Arduino board) - DE/RE control
- MAX3485, SN65HVD72 (industrial grade) - DE/RE control
- Auto-direction RS-485 modules (UART-to-RS485 converters) - No DE/RE needed
- SP3485, MAX13487E (auto direction) - No DE/RE needed

<u>Typical Wiring (MAX485 with DE/RE control):</u>

```
MAX485          Arduino
  VCC    →      5V
  GND    →      GND
  DI     →      TX (Serial1 TX on Mega/Due)
  RO     →      RX (Serial1 RX on Mega/Due)
  DE+RE  →      D2 (or any digital pin)
  A      →      RS-485 Bus A
  B      →      RS-485 Bus B
```

<u>Wiring (Auto-Direction RS-485 modules - No DE/RE):</u>

```
RS-485 Module   Arduino
  VCC    →      5V
  GND    →      GND
  TXD    →      TX (Serial1 TX on Mega/Due)
  RXD    →      RX (Serial1 RX on Mega/Due)
  A      →      RS-485 Bus A
  B      →      RS-485 Bus B
```
*Note: Some RS-485 modules have automatic direction control and don't need DE/RE pins. Set `BACNET_RS485_ENABLE_PIN` to `-1` for these modules.*

<u>4B. Supported Arduino Boards</u>

| Board | RAM | Tier | Status |
|-------|-----|------|--------|
| Arduino Uno | 2KB | 1 | ✅ Tested |
| Arduino Nano | 2KB | 1 | ✅ Compatible |
| Arduino Mega 2560 | 8KB | 2 | ✅ Tested |
| Arduino Due | 96KB | 3 | ✅ Compatible |
| Arduino Zero | 32KB | 3 | ✅ Compatible |
| ESP32 | 520KB | 4 | ✅ Compatible |
| STM32F4 | 128KB | 4 | ✅ Compatible |

5. CONFIGURATION

<u>5A. RS-485 Port Selection</u>

```cpp
// In your sketch, before including BACnetArduino.h:
#define BACNET_RS485_PORT Serial1  // For Mega/Due (default)
// OR
#define BACNET_RS485_PORT Serial   // For Uno (uses main serial)

// RS-485 transceiver control pin (optional)
#define BACNET_RS485_ENABLE_PIN 2  // DE/RE control pin
// OR
#define BACNET_RS485_ENABLE_PIN -1 // No DE/RE control (auto direction modules)

#include <BACnetArduino.h>
```

<u>5B. BACnet Device Configuration</u>

```cpp
BACnetDevice device(
    123,              // MAC address (0-127)
    260123,           // Device Instance ID (0-4194303)
    38400             // Baud rate (9600, 19200, 38400, 76800)
);
```

<u>5C. Automatic Board Detection</u>

The library automatically detects your board and configures memory limits:

```cpp
void setup() {
    Serial.begin(38400);
    device.begin();  // Prints detected configuration
    // Output example:
    // === BACnet Configuration ===
    // Board: Arduino Uno
    // RAM: 2 KB
    // Tier: 1
    // MAX_APDU: 128
    // MAX_TSM: 3
    // Enabled Object Types:
    //   - Binary Value (BV)
    //   - Analog Value (AV)
}
```

6. TESTING WITH YABE

[YABE (Yet Another BACnet Explorer)](https://sourceforge.net/projects/yetanotherbacnetexplorer/) is the recommended tool for testing:

1. Connect RS-485 bus to your PC (USB-RS485 adapter)
2. Launch YABE and add MS/TP device
3. Configure COM port and baud rate (38400)
4. Discover devices - your Arduino appears
5. Read/Write object properties
6. Subscribe to COV notifications (Tier 2+)

<u>6A. Expected Results</u>

```
Device: 260123 (Arduino BACnet Device)
  Object: Binary Value 0 (LED)
    Present_Value: INACTIVE
    Description: "LED on pin 13"
    Status_Flags: [0,0,0,0]
```

Write `ACTIVE` to Present_Value → LED turns ON!

7. TROUBLESHOOTING

<u>7A. "Binary Output not defined" Error</u>

Problem: Trying to use Tier 2+ objects on Arduino Uno
Solution: Use Arduino Mega or higher, or stick to BV/AV objects

<u>7B. Device Not Discovered by YABE</u>

Check:
- RS-485 wiring correct (A-A, B-B)
- Baud rate matches (default 38400)
- MAC address unique on network
- RS-485 transceiver powered
- DE/RE pin configured correctly (or set to -1 for auto-direction modules)
- For auto-direction modules: ensure module supports half-duplex RS-485

<u>7C. Compilation Errors</u>

Solution: Ensure you have latest Arduino IDE (1.8.19+) and EEPROM library installed

<u>7D. Out of Memory</u>

Problem: Sketch too large for board  
Solution: 
- Use smaller example for your board tier
- Reduce number of objects
- Upgrade to board with more RAM

8. API REFERENCE

<u>8A. BACnetDevice Class</u>

```cpp
BACnetDevice(uint8_t mac_address, 
             uint32_t device_instance = 260001,
             uint32_t baud_rate = 38400);

void begin();                    // Initialize device
void process();                  // Process BACnet messages (call in loop)
void addObject(BACnetObject* obj); // Register object
void setDeviceName(const char* name);
void setLocation(const char* location);
```

<u>8B. BACnetBinaryValue Class</u>

```cpp
BACnetBinaryValue(uint32_t instance,
                  const char* name,
                  uint8_t pin = -1);

void setValue(bool value);       // Set Present_Value
bool getValue();                 // Get Present_Value
void setPin(uint8_t pin);       // Associate with digital pin
void enableCOV();               // Enable COV notifications (Tier 2+)
```

<u>8C. BACnetAnalogValue Class</u>

```cpp
BACnetAnalogValue(uint32_t instance,
                  const char* name,
                  BACNET_ENGINEERING_UNITS units = UNITS_DEGREES_CELSIUS);

void setValue(float value);      // Set Present_Value
float getValue();                // Get Present_Value
void setPin(int pin);           // Associate with ADC pin
void setUnits(BACNET_ENGINEERING_UNITS units);
void setMinValue(float min);
void setMaxValue(float max);
void enableCOV(float increment); // COV with increment (Tier 2+)
```

9. DEPENDENCIES

Built-in Arduino Libraries (no installation needed):

- Arduino.h
- EEPROM.h
- HardwareSerial (Serial ports)

External Libraries: None

10. LICENSING

This library uses a dual-license structure:

<u>10A. Arduino Wrapper Code - MIT License</u>

All Arduino-specific code (C++ classes, examples, hardware abstraction) is licensed under MIT License:
- `src/BACnetArduino.h/cpp`
- `src/BACnetDevice.h/cpp`  
- `src/BACnet*.h/cpp` (object classes)
- `src/hardware/` (all files)
- `examples/` (all sketches)
- Documentation files

Why MIT? Maximum compatibility with Arduino ecosystem and ease of use in both open-source and commercial projects.

<u>10B. BACnet Protocol Stack - GPL v2+ with Linking Exception</u>

The `src/bacnet/` directory contains the original BACnet Protocol Stack with original copyright and licensing preserved:

- Copyright: Steve Karg and contributors
- License: GPL v2 or later with linking exception
- Effect: You can use this library in proprietary projects thanks to the linking exception

See `LICENSE` and `NOTICE.txt` files for complete details.

11. ACKNOWLEDGMENTS

This library is inspired by and built upon the excellent [BACnet Protocol Stack](https://github.com/bacnet-stack/bacnet-stack) project.

Original BACnet Stack:

- Author: Steve Karg <skarg@users.sourceforge.net>
- Contributors: Greg Shue, Joel Bender, and many others
- Project: https://github.com/bacnet-stack/bacnet-stack

The BACnet Protocol Stack represents years of dedicated work by talented developers to create a robust, standards-compliant BACnet implementation. This Arduino library provides a modern wrapper to make that excellent work accessible to Arduino developers, while properly crediting the original authors.

Thank you to Steve Karg and all contributors for creating and maintaining this comprehensive BACnet implementation!

12. ADDITIONAL RESOURCES

- BACnet International: http://www.bacnet.org/
- BACnet Stack Project: https://github.com/bacnet-stack/bacnet-stack  
- Arduino Reference: https://www.arduino.cc/reference/
- YABE BACnet Explorer: https://sourceforge.net/projects/yetanotherbacnetexplorer/
- Library Documentation: See `extras/docs/` folder

13. CONTRIBUTING

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test on physical hardware
- Submit pull request with description

14. SUPPORT

- Issues: Report bugs on GitHub Issues
- Discussions: Ask questions on GitHub Discussions  
- Email: [your.email@example.com]

15. ROADMAP

<u>15A. Version 1.0 (Current)</u>

- Arduino Uno through ESP32 support
- Automatic board detection
- BV, AV objects (all boards)
- BO, AI, MSV objects (Tier 2+)
- 50+ example sketches

<u>15B. Version 1.1 (Planned)</u>

- Calendar and Schedule objects (Tier 3)
- Trend Log object (Tier 3)
- BACnet/IP support (ESP32, W5500 shield)
- Web configuration interface (ESP32)

<u>15C. Version 2.0 (Future)</u>

- BACnet Secure Connect (Tier 4)
- MQTT bridge functionality
- More object types (Accumulator, Loop, etc.)

---

Version: 1.0.0  
Last Updated: November 26, 2025  
Status: Production Ready  
License: MIT (wrapper) + GPL v2+ with linking exception (stack)  
Author: [Your Name]  
Inspired by: BACnet Protocol Stack by Steve Karg
