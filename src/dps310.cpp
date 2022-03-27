#include <dps310.h>
using namespace dps;

Adafruit_DPS310 dps::dps;
Adafruit_Sensor *dps_temp = dps::dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps::dps.getPressureSensor();

boolean dps::configure()
{
	if ( !dps.begin_I2C() )
		return false;

	// Setup highest precision
	dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
	dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
	
	return true;
}

void dps::read(float *temp, float *pressure)
{
	sensors_event_t temp_event, pressure_event;

	dps_temp->getEvent(&temp_event);
	*temp = temp_event.temperature;
	delay(30);
	dps_pressure->getEvent(&pressure_event);
	*pressure = pressure_event.pressure;
}