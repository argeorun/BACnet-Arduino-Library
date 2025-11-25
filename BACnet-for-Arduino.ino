/**
 * @brief This module manages the main code for the BACnet demo
 * @author Steve Karg <skarg@users.sourceforge.net>
 * @date 2007
 * @copyright SPDX-License-Identifier: MIT
 * 
 * @note Modified for Arduino IDE compatibility
 * @note Removed hardware.h and AVR-specific code
 * @note Replace AVR functions with Arduino generic equivalents
 */
#include <stdbool.h>
#include <stdint.h>

/* Compile-time configuration - must be included before BACnet headers */
#include "compile_config.h"

/* Arduino IDE compatibility - use Arduino.h instead of avr headers */
#if defined(ARDUINO)
#include <Arduino.h>
#include <EEPROM.h>
#else
/* For testing outside Arduino environment */
#include <stdint.h>
#include <stdbool.h>
#endif

/* EEPROM functions - moved from h_eeprom.cpp */
/* These need C linkage since they're called from C code (nvdata.c) */
extern "C" {
int eeprom_bytes_read(uint16_t eeaddr, uint8_t *buf, int len)
{
    int count = 0;
    while (len > 0) {
        buf[count] = EEPROM.read(eeaddr);
        count++;
        eeaddr++;
        len--;
    }
    return count;
}

int eeprom_bytes_write(uint16_t eeaddr, const uint8_t *buf, int len)
{
    int count = 0;
    while (len > 0 && eeaddr < EEPROM.length()) {
        EEPROM.write(eeaddr, buf[count]);
        count++;
        eeaddr++;
        len--;
    }
    return count;
}
}

#include "adc.h"
#include "nvdata.h"
#include "rs485.h"
#include "stack.h"
//#include "bacnet/datalink/datalink.h"
#include "src/bacnet/datalink/datalink.h"
//#include "bacnet/datalink/dlmstp.h"
#include "src/bacnet/datalink/dlmstp.h"
//#include "bacnet/npdu.h"
#include "src/bacnet/npdu.h"
//#include "bacnet/dcc.h"
#include "src/bacnet/dcc.h"
//#include "bacnet/basic/services.h"
#include "src/bacnet/basic/services.h"
//#include "bacnet/basic/sys/mstimer.h"
#include "src/bacnet/basic/sys/mstimer.h"
//#include "bacnet/basic/tsm/tsm.h"
#include "src/bacnet/basic/tsm/tsm.h"
//#include "bacnet/iam.h"
#include "src/bacnet/iam.h"
//#include "bacnet/basic/object/device.h"
#include "src/bacnet/basic/object/device.h"
//#include "bacnet/basic/object/av.h"
#include "src/bacnet/basic/object/av.h"
//#include "bacnet/basic/object/bv.h"
#include "src/bacnet/basic/object/bv.h"

/* From the WhoIs hander - performed by the DLMSTP module */
extern bool Send_I_Am_Flag;
/* main loop task timer */
static struct mstimer Task_Timer;
/* uptime */
static uint32_t Uptime_Seconds;

/**
 * @brief Initialize hardware for Arduino
 * @note Removed AVR-specific register access
 * @note Hardware initialization should be done in setup() for Arduino
 */
static void hardware_init(void)
{
    /* Arduino-specific initialization */
    /* Clock prescaler configuration not needed - Arduino handles this */
    
    /* I/O port initialization not needed - Arduino handles this */
    /* pinMode() calls should be done in setup() or abstraction layers */
    
    /* Watchdog timer configuration - Use Arduino watchdog library if needed */
    /* #include <avr/wdt.h> and wdt_disable() or wdt_enable() */
    
    /* Configure Specialized Hardware */
    RS485_Initialize();
    mstimer_init();
    adc_init();

    /* Enable global interrupts - Arduino handles this automatically */
    /* sei() is called by Arduino after setup() */
#if defined(ARDUINO)
    interrupts(); // Arduino way to enable interrupts
#endif
}

/**
 * @brief process some values once per second
 */
static void one_second_task(void)
{
    BACNET_BINARY_PV value;
    float hours;

    /* LED toggling */
    value = Binary_Value_Present_Value(9);
   
    if (value == BINARY_ACTIVE) {
        value = BINARY_INACTIVE;
    } else {
        value = BINARY_ACTIVE;
    }
    Binary_Value_Present_Value_Set(9, value);
    /* uptime */
    Uptime_Seconds += 1;
    hours = (float)Uptime_Seconds / 3600.0f;
    Analog_Value_Present_Value_Set(99, hours, 0);
}

/**
 * Initialize the device's non-volatile data
 */
static void device_nvdata_init(void)
{
    uint8_t value8;
    uint16_t value16;
    uint32_t value32;
    const char *default_name = "Arduino BACnet Device";
    const char *default_description = "Arduino BACnet MS/TP Device";
    const char *default_location = "Arduino Location";

    value16 = nvdata_unsigned16(NV_EEPROM_TYPE_0);
    if (value16 != NV_EEPROM_TYPE_ID) {
        /* set to defaults */
        nvdata_unsigned16_set(NV_EEPROM_TYPE_0, NV_EEPROM_TYPE_ID);
        nvdata_unsigned8_set(NV_EEPROM_VERSION, NV_EEPROM_VERSION_ID);
        nvdata_unsigned8_set(NV_EEPROM_MSTP_MAC, 123);
        nvdata_unsigned8_set(NV_EEPROM_MSTP_BAUD_K, 38);
        nvdata_unsigned8_set(NV_EEPROM_MSTP_MAX_MASTER, 127);
        nvdata_unsigned24_set(NV_EEPROM_DEVICE_0, 260123);
        nvdata_name_set(
            NV_EEPROM_DEVICE_NAME, CHARACTER_UTF8, default_name,
            strlen(default_name));
        nvdata_name_set(
            NV_EEPROM_DEVICE_DESCRIPTION, CHARACTER_UTF8, default_description,
            strlen(default_description));
        nvdata_name_set(
            NV_EEPROM_DEVICE_LOCATION, CHARACTER_UTF8, default_location,
            strlen(default_location));
    }
    value8 = nvdata_unsigned8(NV_EEPROM_MSTP_MAC);
    dlmstp_set_mac_address(value8);
    value8 = nvdata_unsigned8(NV_EEPROM_MSTP_BAUD_K);
    value32 = RS485_Baud_Rate_From_Kilo(value8);
    RS485_Set_Baud_Rate(value32);
    value8 = nvdata_unsigned8(NV_EEPROM_MSTP_MAX_MASTER);
    if (value8 > 127) {
        value8 = 127;
    }
    dlmstp_set_max_master(value8);
    dlmstp_set_max_info_frames(1);
    Send_I_Am_Flag = true;
}

/**
 * Static receive buffer,
 * initialized with zeros by the C Library Startup Code.
 * Note: Added a little safety margin to the buffer,
 * so that in the rare case, the message would be filled
 * up to MAX_MPDU and some decoding functions would overrun,
 * these decoding functions will just end up in a safe field
 * of static zeros. */
static uint8_t PDUBuffer[MAX_MPDU + 16];

#if defined(ARDUINO)
/**
 * @brief Arduino setup function - called once at startup
 * @note This replaces the initialization part of main()
 */
void setup(void)
{
    hardware_init();
    Analog_Value_Init();
    Binary_Value_Init();
    device_nvdata_init();
    dlmstp_init(NULL);
    mstimer_set(&Task_Timer, 1000);
}

/**
 * @brief Arduino loop function - called repeatedly
 * @note This replaces the infinite for loop in main()
 */
void loop(void)
{
    uint16_t pdu_len = 0;
    BACNET_ADDRESS src; /* source address */
    
    /* process periodic tasks */
    if (mstimer_expired(&Task_Timer)) {
        mstimer_reset(&Task_Timer);
        one_second_task();
    }
    
    /* BACnet handling */
    pdu_len = dlmstp_receive(&src, &PDUBuffer[0], MAX_MPDU, 0);
    if (pdu_len) {
        npdu_handler(&src, &PDUBuffer[0], pdu_len);
    }
}

#else
/**
 * @brief Main function for non-Arduino builds
 * @return 0, should never reach this
 */
int main(void)
{
    uint16_t pdu_len = 0;
    BACNET_ADDRESS src; /* source address */

    hardware_init();
    Analog_Value_Init();
    Binary_Value_Init();
    device_nvdata_init();
    dlmstp_init(NULL);
    mstimer_set(&Task_Timer, 1000);
    for (;;) {
        /* process */
        if (mstimer_expired(&Task_Timer)) {
            mstimer_reset(&Task_Timer);
            one_second_task();
        }
        /* BACnet handling */
        pdu_len = dlmstp_receive(&src, &PDUBuffer[0], MAX_MPDU, 0);
        if (pdu_len) {
            npdu_handler(&src, &PDUBuffer[0], pdu_len);
        }
    }
}
#endif
