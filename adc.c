/**
 * Arduino-compatible ADC wrapper
 * This file replaces AVR-specific ADC ISR and register usage with
 * Arduino IDE friendly calls (analogRead, noInterrupts/interrupts,
 * analogReference). It preserves the original adc API so existing
 * code can use `adc_result_8bit`, `adc_result_10bit`, `adc_millivolts`,
 * `adc_enable`, and `adc_init` but without AVR register access.
 */

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include "adc.h"

/* Keep compatibility constants from original file */
#ifndef ADC_MILLIVOLTS_MAX
#define ADC_MILLIVOLTS_MAX 5000L
#endif
#ifndef ADC_VALUE_MAX
#define ADC_VALUE_MAX 1024L
#endif

#define ADC_CHANNELS_MAX 8
static volatile uint16_t Sample_Result[ADC_CHANNELS_MAX];
static volatile uint8_t Enabled_Channels;

/*
 * Note: The original AVR implementation used ADC interrupts and
 * hardware registers (ADCSRA, ADMUX, ISR(ADC_vect), etc.). On
 * Arduino (AVR core or other cores) the easiest portable approach
 * is to perform synchronous `analogRead()` calls. This file
 * provides the same API but performs immediate reads when a
 * channel is requested.
 */

void adc_init(void)
{
    /* Configure analog reference to default (typically AVcc). */
    analogReference(DEFAULT);
    Enabled_Channels = 0;
    /* Optional: perform a dummy read to stabilize ADC */
    for (uint8_t i = 0; i < ADC_CHANNELS_MAX; i++) {
        Sample_Result[i] = 0;
    }
}

void adc_enable(uint8_t index)
{
    if (index >= ADC_CHANNELS_MAX) return;
    /* Mark channel enabled and do an initial synchronous read to
       populate the sample result. Use noInterrupts/interrupts to
       mimic atomic behavior when updating shared state. */
    noInterrupts();
    Enabled_Channels |= (1 << index);
    /* analogRead takes either A0..An or 0..n depending on core.
       Passing the numeric index works on Arduino AVR and many
       other cores. If your platform requires `A0 + index`, change
       the call accordingly. */
    Sample_Result[index] = analogRead(index);
    interrupts();
}

uint8_t adc_result_8bit(uint8_t index)
{
    uint8_t result = 0;

    if (index < ADC_CHANNELS_MAX) {
        adc_enable(index);
        noInterrupts();
        /* Convert 10-bit to 8-bit by shifting */
        result = (uint8_t)(Sample_Result[index] >> 2);
        interrupts();
    }

    return result;
}

uint16_t adc_result_10bit(uint8_t index)
{
    uint16_t result = 0;

    if (index < ADC_CHANNELS_MAX) {
        adc_enable(index);
        noInterrupts();
        /* Perform a fresh synchronous read to ensure latest value */
        Sample_Result[index] = analogRead(index);
        result = Sample_Result[index];
        interrupts();
    }

    return result;
}

uint16_t adc_millivolts(uint8_t index)
{
    uint32_t value;

    value = adc_result_10bit(index);
    value = (value * ADC_MILLIVOLTS_MAX) / ADC_VALUE_MAX;

    return (uint16_t)(value);
}

/*
 * The AVR code used an ISR and ADC registers to continuously sample
 * channels. That approach depends on direct register access and is
 * not portable to all Arduino cores or desirable for a generic
 * Arduino project. If you need interrupt-driven continuous sampling
 * on a specific board/architecture, implement a board-specific
 * variant that configures ADC registers or timers accordingly.
 */
