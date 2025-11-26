# BV_LED_Control - LED Control via BACnet

## Overview

This example demonstrates the simplest BACnet device: controlling Arduino's built-in LED using BACnet WriteProperty commands.

**Difficulty**: ⭐ Beginner  
**Hardware Needed**: Arduino board + RS-485 transceiver  
**RAM Usage**: ~1.5 KB (fits Arduino Uno)  
**BACnet Objects**: 1 Binary Value (LED)

---

## What This Example Teaches

- ✅ Creating a basic BACnet device
- ✅ Adding a Binary Value object
- ✅ Controlling physical output via BACnet
- ✅ Testing with YABE BACnet Explorer
- ✅ Understanding Present_Value property
- ✅ BACnet device discovery (Who-Is/I-Am)

---

## Hardware Setup

### Required Components

1. **Arduino Board** (choose one):
   - Arduino Mega 2560 (recommended for testing - dual serial ports)
   - Arduino Uno (works, but debug output disabled)
   - Arduino Due, ESP32, STM32 (also compatible)

2. **RS-485 Transceiver** (choose one):
   - DFRobot DFR0259 RS-485 Shield
   - Generic MAX485 module
   - Auto-direction RS-485 module (no DE/RE needed)

3. **BACnet Testing Tool**:
   - YABE (Yet Another BACnet Explorer) - free download

4. **USB-RS485 Adapter** (for PC connection to RS-485 bus)

### Wiring Diagram - Arduino Mega + MAX485

```
MAX485 Module        Arduino Mega 2560
─────────────        ─────────────────
VCC          ───→    5V
GND          ───→    GND
DI (TXD)     ───→    TX1 (Pin 18)
RO (RXD)     ───→    RX1 (Pin 19)
DE + RE      ───→    D2 (enable pin)
A            ───→    RS-485 Bus A (to other devices/PC adapter)
B            ───→    RS-485 Bus B (to other devices/PC adapter)

Built-in LED         Pin 13 (no wiring needed)
```

### Wiring Diagram - Auto-Direction RS-485 Module

```
RS-485 Module        Arduino Mega 2560
─────────────        ─────────────────
VCC          ───→    5V
GND          ───→    GND
TXD          ───→    TX1 (Pin 18)
RXD          ───→    RX1 (Pin 19)
A            ───→    RS-485 Bus A
B            ───→    RS-485 Bus B

Note: No DE/RE pin needed
Set BACNET_RS485_ENABLE_PIN to -1 in BACnetConfig.h
```

### RS-485 Bus Termination

For reliable communication:
- Add 120Ω termination resistor at each end of bus
- Connect between A and B terminals
- Required for distances > 1 meter

---

## Software Setup

### 1. Install Library

**Arduino IDE:**
```
Sketch → Include Library → Manage Libraries
Search: "BACnet for Arduino"
Install
```

### 2. Upload Sketch

1. Connect Arduino via USB
2. Select board: **Tools → Board → Arduino Mega 2560**
3. Select port: **Tools → Port → COMx**
4. Upload sketch: **Sketch → Upload**

### 3. Verify Serial Output (Mega only)

Open Serial Monitor (9600 baud):
```
=================================
BACnet LED Control Example
=================================
=== BACnet Configuration ===
Board: Arduino Mega 2560
RAM: 8 KB
Tier: 2
MAX_APDU: 512
MAX_TSM: 12
Enabled Object Types:
  - Binary Value (BV)
  - Analog Value (AV)
  - Binary Output (BO)
  - Analog Input (AI)
===========================

Device Ready!
Waiting for BACnet commands...
```

---

## Testing with YABE

### Step 1: Install YABE

Download from: https://sourceforge.net/projects/yetanotherbacnetexplorer/

### Step 2: Connect RS-485

1. Connect USB-RS485 adapter to PC
2. Connect adapter to RS-485 bus (A-A, B-B)
3. Note the COM port number (e.g., COM3)

### Step 3: Configure YABE

1. Launch YABE
2. Click **"Add Device"**
3. Select **"MSTP"**
4. Configure:
   - **COM Port**: Your USB-RS485 adapter port
   - **Baud Rate**: 38400
   - **Max Master**: 127
   - **Max Info Frames**: 1
5. Click **OK**

### Step 4: Discover Devices

1. Right-click on MSTP device in tree
2. Select **"Device Discovery"** or click magnifying glass icon
3. Wait 5-10 seconds
4. You should see: **Device 260001** (Arduino LED Controller)

### Step 5: Read Properties

1. Expand **Device 260001**
2. Find **Binary Value 0 (LED)**
3. Right-click → **"Read All Properties"**
4. Verify:
   - **Object_Name**: "LED"
   - **Present_Value**: INACTIVE (LED off)
   - **Status_Flags**: [0,0,0,0]
   - **Out_Of_Service**: FALSE

### Step 6: Control LED

**Turn LED ON:**
1. Right-click on **Binary Value 0 (LED)**
2. Select **"Write Property"**
3. Property: **Present_Value**
4. Value: **ACTIVE** (or 1)
5. Click **Write**
6. LED on Arduino should turn ON!

**Turn LED OFF:**
1. Right-click again
2. Write Property → **Present_Value**
3. Value: **INACTIVE** (or 0)
4. Click **Write**
5. LED should turn OFF!

### Expected Serial Monitor Output (Mega)

```
LED state changed: ON
LED state changed: OFF
LED state changed: ON
...
```

---

## BACnet Properties Reference

### Binary Value 0 (LED)

| Property | Value | Description |
|----------|-------|-------------|
| Object_Identifier | binary-value,0 | Unique object ID |
| Object_Name | "LED" | Human-readable name |
| Object_Type | BINARY_VALUE | Object type |
| Present_Value | ACTIVE/INACTIVE | Current LED state |
| Status_Flags | [0,0,0,0] | In_Alarm, Fault, Overridden, Out_Of_Service |
| Out_Of_Service | FALSE | Object is in service |
| Description | "" | Optional description |

### Device 260001

| Property | Value | Description |
|----------|-------|-------------|
| Object_Identifier | device,260001 | Device instance |
| Object_Name | "Arduino LED Controller" | Device name |
| Vendor_Name | "Arduino" | Vendor |
| Model_Name | "Arduino Mega 2560" | Board model |
| Protocol_Version | 1 | BACnet protocol version |
| Protocol_Services_Supported | Read/Write/WhoIs/IAm | Supported services |

---

## Troubleshooting

### LED Not Responding

**Problem**: Writing Present_Value doesn't change LED state

**Solutions**:
1. Verify `device.task()` is called in `loop()`
2. Check LED_BUILTIN pin is 13 on your board
3. Verify object was added: `device.addObject(&led)`
4. Check for error messages in Serial Monitor

### Device Not Discovered by YABE

**Problem**: YABE doesn't find device 260001

**Solutions**:
1. Check RS-485 wiring (A-A, B-B, not A-B!)
2. Verify baud rate matches (38400)
3. Check MAX485 DE/RE pin connection (D2)
4. Try different MAC address (avoid 0, use 1-127)
5. Add 120Ω termination resistors on bus
6. Verify power to RS-485 module

### Compilation Errors

**Error**: `'BACnetDevice' was not declared in this scope`

**Solution**: Install library via Library Manager

**Error**: `#error "Board not supported"`

**Solution**: Update BACnetConfig.h with your board definition

### Memory Issues on Uno

**Warning**: `Low memory available, stability problems may occur`

**Solution**: Normal for Uno. Example uses ~1.5KB, leaving ~500 bytes free.

---

## Advanced: Customization

### Change MAC Address

```cpp
#define BACNET_MAC_ADDRESS 20  // Use 1-127, must be unique
```

### Change Device Instance

```cpp
#define BACNET_DEVICE_INSTANCE 400100  // Must be unique on network
```

### Use Different Pin

```cpp
// Control external LED on pin 8
BACnetBinaryValue led(0, "LED", 8);
```

### Add Description

```cpp
led.setDescription("External status LED");
```

---

## How It Works

### Initialization (setup)

1. **Create BACnet device** with MAC address and instance
2. **Create Binary Value object** linked to pin 13
3. **Initialize device** (`device.begin()`)
   - Configures RS-485 communication
   - Initializes BACnet protocol stack
   - Sets up MS/TP datalink layer
4. **Register object** with device
5. **Ready** to receive BACnet commands

### Runtime (loop)

1. **Process BACnet messages** (`device.task()`)
   - Receives RS-485 data
   - Parses BACnet frames
   - Handles service requests
2. **WriteProperty received** for Binary Value 0
   - BACnet stack updates Present_Value
   - BACnetBinaryValue automatically calls `digitalWrite()`
   - LED changes state
3. **ReadProperty received**
   - Returns current Present_Value
   - Reads actual pin state

### Automatic Pin Control

The `BACnetBinaryValue` class automatically:
- Configures pin as OUTPUT
- Writes HIGH/LOW when Present_Value changes
- Reads pin state for ReadProperty requests
- No manual `digitalWrite()` needed!

---

## Next Steps

Once you have this example working:

1. **BV_DigitalInput** - Read button state via BACnet
2. **BV_WithCOV** - Automatic change notifications (Tier 2+)
3. **AV_Temperature** - Read analog sensor via BACnet
4. **Multiple Objects** - Control multiple LEDs

---

## RAM Usage Analysis

### Arduino Uno (2KB total)

```
BACnet Stack:         ~800 bytes
Device Object:        ~100 bytes
Binary Value Object:  ~80 bytes
PDU Buffer (128):     ~144 bytes
MS/TP State:          ~200 bytes
─────────────────────────────────
Total:                ~1324 bytes
Available:            ~724 bytes  ✅ Safe
```

### Arduino Mega (8KB total)

```
BACnet Stack:         ~1000 bytes
Device Object:        ~150 bytes
Binary Value Object:  ~80 bytes
PDU Buffer (512):     ~528 bytes
MS/TP State:          ~200 bytes
Debug Output:         ~50 bytes
─────────────────────────────────
Total:                ~2008 bytes
Available:            ~6184 bytes  ✅ Plenty of headroom
```

---

## Related Documentation

- **BACnet Protocol**: http://www.bacnet.org/
- **YABE Documentation**: See YABE help menu
- **Library API Reference**: See main README.md
- **RS-485 Basics**: See extras/docs/RS485_Guide.md

---

**Example Created**: November 26, 2025  
**Tested On**: Arduino Mega 2560  
**BACnet Protocol Version**: ASHRAE 135-2020  
**Library Version**: 1.0.0
