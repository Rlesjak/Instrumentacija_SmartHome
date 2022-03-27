// opt3001.h
#pragma once

#include <ClosedCube_OPT3001.h>

#define OPT3001_ADDRESS 0x44

namespace opt
{
	boolean configure();
	float read();

	extern ClosedCube_OPT3001 opt3001;
}