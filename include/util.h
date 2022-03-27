// util.h
#pragma once

#include <Arduino.h>

#define ROLLING_BUFFER_SIZE(RollingAverageTime, SamplingInterval) (RollingAverageTime/SamplingInterval)

void pulsePin(uint8_t pin);
