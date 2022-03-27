// hdc2080.h
#pragma once

#include <HDC2080.h>
#define HDC2080_I2C_ADDR 0x40

namespace hdc
{	
	boolean configure();
	void read(float *temp, float *humi);
}