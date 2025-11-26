# Old C Implementation - Foundation of BACnet-for-Arduino Library

**Date Archived**: November 26, 2025  
**Status**: Historical reference - No longer actively used  
**License**: MIT (Arduino-specific portions) + GPL v2+ (bacnet-stack portions)

---

## About These Files

This directory contains the **original C implementation** files that served as the foundation for developing the BACnet-for-Arduino C++ wrapper library. These files were created during the initial Arduino port and represent the evolutionary steps toward the current library architecture.

### Historical Significance

These files were instrumental in:
1. **Proving BACnet stack could run on Arduino** - Demonstrated feasibility on AVR architecture
2. **Testing RS485 communication** - Validated hardware abstraction approach
3. **Memory optimization research** - Identified RAM constraints and solutions
4. **API design exploration** - Informed the C++ wrapper class design

### Development Timeline

1. **atmega328/** - Original AVR/Makefile project (working baseline)
2. **BAC_arduino_test/** - Arduino IDE adaptation (C files + .ino)
3. **BACnet-for-Arduino/** - These archived files (transitional C implementation)
4. **src/** - Current C++ wrapper library (production code)

---

## File Origins and Authors

### Files from bacnet-stack Project (GPL v2+ with linking exception)

These files were adapted from the [bacnet-stack](https://github.com/bacnet-stack/bacnet-stack) project:

- **apdu.c** - APDU handling
- **h_rp.c** - ReadProperty service handler
- **h_whois.c** - Who-Is service handler
- **h_wp.c** - WriteProperty service handler
- **dlmstp.c** - MS/TP datalink layer

**Original Author**: Steve Karg <skarg@users.sourceforge.net>  
**Contributors**: Greg Shue, Joel Bender, and many others  
**Original License**: GPL v2 or later, with linking exception  
**Copyright**: Copyright (C) 2005-2024 Steve Karg and contributors

### Files Created for Arduino Port (MIT License)

These files were created specifically for the Arduino adaptation:

- **adc.c** - AVR ADC wrapper (replaced by analogRead())
- **av.c** - Analog Value object implementation
- **bv.c** - Binary Value object implementation
- **device.c** - Device object management
- **nvdata.c** - EEPROM/non-volatile data handling
- **pin_config.c** - Arduino pin configuration
- **stack.c** - Stack initialization and management
- **timer.c** - Timer abstraction (millis/micros wrappers)
- **BACnet-for-Arduino.ino** - Original Arduino sketch

**Author**: George Arun <argeorun@gmail.com>  
**License**: MIT License  
**Purpose**: Arduino-specific adaptations and hardware abstraction  
**Copyright**: Copyright (c) 2025 George Arun

---

## Why These Files Are No Longer Used

### Replaced by C++ Wrapper Classes

The current library uses object-oriented C++ wrappers that provide:
- **Better encapsulation** - Each BACnet object is a C++ class
- **Type safety** - Compile-time type checking
- **Arduino conventions** - Follows Arduino library best practices
- **Easier API** - Simpler for Arduino users to understand
- **Better organization** - Clear separation of concerns

### Old Approach (C files)
```cpp
// BACnet-for-Arduino.ino (old)
#include "device.c"
#include "av.c"
#include "bv.c"

void setup() {
    device_init();
    av_init(0, "Temperature");
    bv_init(0, "LED");
}
```

### New Approach (C++ library)
```cpp
// Example sketch (new)
#include <BACnetArduino.h>

BACnetDevice device(123);
BACnetAnalogValue temp(0, "Temperature");
BACnetBinaryValue led(0, "LED");

void setup() {
    device.begin();
    device.addObject(&temp);
    device.addObject(&led);
}
```

---

## File Mapping: Old → New

| Old C Implementation | Current C++ Wrapper | Notes |
|---------------------|---------------------|-------|
| `device.c` | `src/BACnetDevice.cpp` | Device management class |
| `av.c` | `src/BACnetAnalogValue.cpp` | Analog Value class |
| `bv.c` | `src/BACnetBinaryValue.cpp` | Binary Value class |
| `stack.c` | `src/BACnetDevice.cpp` | Integrated into device initialization |
| `timer.c` | `src/hardware/BACnetTimer.cpp` | Hardware abstraction |
| `pin_config.c` | `src/BACnetConfig.h` | Compile-time configuration |
| `nvdata.c` | `src/BACnetDevice.cpp` | EEPROM handling integrated |
| `adc.c` | Arduino `analogRead()` | No longer needed |
| `BACnet-for-Arduino.ino` | `examples/` | Multiple focused examples |

---

## Attribution and Acknowledgments

### Original BACnet Protocol Stack

All BACnet protocol functionality is provided by the [bacnet-stack](https://github.com/bacnet-stack/bacnet-stack) project, located in `src/bacnet/` directory (unchanged from original).

**Primary Author**: Steve Karg  
**Project**: https://github.com/bacnet-stack/bacnet-stack  
**License**: GPL v2+ with linking exception  
**Key Contributors**:
- Steve Karg (skarg@users.sourceforge.net)
- Greg Shue
- Joel Bender
- Christian Fortin
- Krzysztof Malorny
- And many more (see AUTHORS file in bacnet-stack)

### Arduino Adaptation

The Arduino-specific code (these archived files and the current C++ wrappers) was created to make BACnet accessible to Arduino developers.

**Author**: George Arun <argeorun@gmail.com>  
**License**: MIT License (for Arduino-specific code only)  
**Purpose**: Educational and maker-friendly BACnet implementation

---

## License Information

### Dual Licensing

This project uses dual licensing:

1. **Arduino Wrapper Code** (MIT License)
   - All files in this archive directory
   - All files in `src/` (except `src/bacnet/`)
   - All example sketches
   - Hardware abstraction layers

2. **BACnet Protocol Stack** (GPL v2+ with linking exception)
   - All files in `src/bacnet/`
   - Original copyright and license preserved
   - Linking exception allows proprietary use

### MIT License (Arduino Wrapper)

```
MIT License

Copyright (c) 2025 George Arun <argeorun@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## Using This Code

### For Reference Only

These files are preserved for:
- **Historical documentation** - Understanding development process
- **Educational purposes** - Learning BACnet/Arduino integration
- **Comparison** - Seeing evolution from C to C++ approach

### Do Not Use Directly

If you want to use BACnet on Arduino:
- ✅ Use the current C++ library in `src/`
- ✅ Include `<BACnetArduino.h>` in your sketches
- ✅ Use the examples in `examples/` directory
- ❌ Do not compile these archived C files

---

## Questions or Historical Context

For questions about the development history or evolution of this library:
- **Email**: argeorun@gmail.com
- **GitHub**: https://github.com/argeorun/BACnet-Arduino-Library
- **Issues**: Use GitHub Issues for questions

For questions about the BACnet protocol stack itself:
- **Original Project**: https://github.com/bacnet-stack/bacnet-stack
- **Documentation**: See `src/bacnet/` directory

---

**Archived**: November 26, 2025  
**By**: George Arun  
**Reason**: Library converted to C++ wrapper architecture  
**Preservation**: These files represent the foundation and evolutionary history of the BACnet-for-Arduino library.
