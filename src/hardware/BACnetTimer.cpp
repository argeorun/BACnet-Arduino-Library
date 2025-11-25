/*
 * BACnetTimer.cpp - Timer Hardware Abstraction Layer implementation
 * 
 * Copyright (c) 2025 Geo_Arun
 * Licensed under MIT License
 */

#include "BACnetTimer.h"

unsigned long BACnetTimer::millis() {
    return ::millis();
}

unsigned long BACnetTimer::micros() {
    return ::micros();
}

void BACnetTimer::delay(unsigned long ms) {
    ::delay(ms);
}

void BACnetTimer::delayMicroseconds(unsigned int us) {
    ::delayMicroseconds(us);
}
