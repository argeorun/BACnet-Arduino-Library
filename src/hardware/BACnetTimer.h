/*
 * BACnetTimer.h - Timer Hardware Abstraction Layer
 * 
 * Copyright (c) 2025 George Arun <argeorun@gmail.com>
 * Licensed under MIT License
 * 
 * Provides millisecond timing for BACnet protocol stack.
 */

#ifndef BACNET_TIMER_H
#define BACNET_TIMER_H

#include <Arduino.h>

class BACnetTimer {
public:
    /**
     * Get current milliseconds
     */
    static unsigned long millis();
    
    /**
     * Get current microseconds
     */
    static unsigned long micros();
    
    /**
     * Delay for milliseconds
     */
    static void delay(unsigned long ms);
    
    /**
     * Delay for microseconds
     */
    static void delayMicroseconds(unsigned int us);
};

#endif // BACNET_TIMER_H
