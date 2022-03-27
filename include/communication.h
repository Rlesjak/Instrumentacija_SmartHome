// communication.h
#pragma once

#include <Arduino.h>

#define DATA_START_CHAR "["
#define DATA_END_CHAR "]"
#define DATA_DELIMITER "|"

namespace comm
{
	boolean configure(int baud);
	void sendPacket(float *arr, int len);
}