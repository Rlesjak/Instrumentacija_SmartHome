#include <sh_hdc2080.h>
using namespace hdc;

HDC2080 sensor(HDC2080_I2C_ADDR);

boolean hdc::configure()
{
	// Initialize I2C communication
	sensor.begin();
	// Begin with a device reset
	sensor.reset();

	// Configure Measurements
	sensor.setMeasurementMode(TEMP_AND_HUMID);
	sensor.setRate(TWO_HZ);   
	sensor.setTempRes(FOURTEEN_BIT);
	sensor.setHumidRes(FOURTEEN_BIT);

	//begin measuring
	sensor.triggerMeasurement();

	return true;
}

void hdc::read(float *temp, float *humi)
{
	*temp = sensor.readTemp();
	*humi = sensor.readHumidity();
}