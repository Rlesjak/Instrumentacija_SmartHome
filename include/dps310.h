// dps310.h
#pragma once

#include <Arduino.h>
#include <Adafruit_DPS310.h>

namespace dps
{
	boolean configure();
	void read(float *temp, float *pressure);

	extern Adafruit_DPS310 dps;
}