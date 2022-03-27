#include <opt3001.h>
using namespace opt;

ClosedCube_OPT3001 opt::opt3001;

// Configure sensor
// Return false if failed
boolean opt::configure() {
	OPT3001_Config newConfig;
	
	newConfig.RangeNumber = B1100;
	newConfig.ConvertionTime = B0;
	newConfig.Latch = B1;
	newConfig.ModeOfConversionOperation = B11;

	opt3001.begin(OPT3001_ADDRESS);
	
	OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
	if (errorConfig != NO_ERROR)
		return false;
	return true;
}

// Read from sensor
float opt::read() {
	OPT3001 result = opt3001.readResult();
	if (result.error == NO_ERROR)
		return result.lux;
	return -1;
}