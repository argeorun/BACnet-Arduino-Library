# Disabled Files in BAC_arduino_test

This document tracks all source files that have been disabled during the Arduino port to prevent compilation issues.

## Files Disabled Due to Multiple Definitions

These files exist in both the root BAC_arduino_test folder (Arduino-customized versions) and the src/bacnet folder (generic library versions). The src folder versions were disabled to avoid "multiple definition" linker errors.

### Object Files
- `src/bacnet/basic/object/device.c.disabled` - Duplicate of root `device.c`
- `src/bacnet/basic/object/av.c.disabled` - Duplicate of root `av.c` (Analog Value object)
- `src/bacnet/basic/object/bv.c.disabled` - Duplicate of root `bv.c` (Binary Value object)

### Service Handler Files
- `src/bacnet/basic/service/h_rp.c.disabled` - Duplicate of root `h_rp.c` (ReadProperty handler)
- `src/bacnet/basic/service/h_wp.c.disabled` - Duplicate of root `h_wp.c` (WriteProperty handler)
- `src/bacnet/basic/service/h_whois.c.disabled` - Duplicate of root `h_whois.c` (Who-Is handler)
- `src/bacnet/basic/service/h_apdu.c.disabled` - Duplicate of root `apdu.c` (APDU handler)

### Datalink Files
- `src/bacnet/datalink/dlmstp.c.disabled` - Duplicate of root `dlmstp.c` (MS/TP datalink)

### Device Management Files  
- `src/bacnet/basic/object/client/device-client.c.disabled` - Client device functions (conflicts with root device.c)
- `src/bacnet/basic/server/bacnet_device.c.disabled` - Server device functions (conflicts with root device.c)

## Files Disabled Due to RAM/Flash Constraints

These files were disabled because they contain large arrays or data structures that exceed Arduino Uno's memory constraints (2KB RAM, 32KB Flash).

### Object Type Files
- `src/bacnet/basic/object/schedule.c.disabled` - Large Schedule_Descr array exceeds RAM
- `src/bacnet/basic/object/trendlog.c.disabled` - Large Logs array exceeds RAM

### Secure Connect Implementation
- `src/bacnet/basic/object/sc_netport.c.disabled` - Secure Connect network port (depends on BSC)

## Removed Folders

These entire folders were removed as they are not compatible with Arduino or exceed memory constraints.

- `src/bacnet/basic/ucix/` - Linux UCI configuration interface (not portable to Arduino)
- `src/bacnet/datalink/bsc/` - BACnet Secure Connect implementation (too large for Arduino Uno)

## Notes

- Root folder .c files are Arduino-customized versions with hardware abstraction
- src/bacnet folder files are the generic BACnet protocol stack library
- When both versions exist, the root version takes precedence for Arduino builds
- All disabled files are preserved with .disabled extension for reference
