#include <Arduino.h>
#include <Wire.h>
#include <opt3001.h>
#include <dps310.h>
#include <fifo.h>
#include <util.h>
#include <sh_hdc2080.h>
#include <communication.h>
#include <settings.hpp>

// Buffers
float _tempBuff[TEMP_BUFF_SIZE] = {0.0f};
fifo_controller_t tempBuff = {TEMP_BUFF_SIZE, _tempBuff, 0};

float _humiBuff[TEMP_BUFF_SIZE] = {0.0f};
fifo_controller_t humiBuff = {TEMP_BUFF_SIZE, _humiBuff, 0};

float _lightBuff[LIG_PR_BUFF_SIZE] = {0.0f};
fifo_controller_t lightBuff = {LIG_PR_BUFF_SIZE, _lightBuff, 0};

float _pressureBuff[LIG_PR_BUFF_SIZE] = {0.0f};
fifo_controller_t pressureBuff = {LIG_PR_BUFF_SIZE, _pressureBuff, 0};

// Timestamps
unsigned long LastTempTimestamp;
unsigned long LastPressTimestamp;
unsigned long LastPrintTimestamp;
unsigned long PressureChangeTimestampUp;
unsigned long PressureChangeTimestampDown;
unsigned long CurrentTimestamp;

// State
bool IsLightOn=false;
float LastPressure = 0.0f;
float Averages[4];

int settings_high_temp = HIGH_TEMP;
int settings_low_temp = LOW_TEMP;
int settings_min_lux = MIN_LUX;



void setup() {
	comm::configure(9600);
	opt::configure();
	dps::configure();
	hdc::configure();
	unsigned long now = millis();
	LastTempTimestamp = now;
	LastPressTimestamp = now;
	LastPrintTimestamp = now;
	PressureChangeTimestampUp = 0;
	PressureChangeTimestampDown = 0;

	delay(1000);
	float temp, humi;
	hdc::read( &temp, &humi );

	for (int i = 0; i < TEMP_BUFF_SIZE; i++)
	{
		_tempBuff[i] = temp;
		_humiBuff[i] = humi;
	}
}

void loop() {
	comm::serviceSerial();
	
	CurrentTimestamp = millis();

	// Send data over serial
	if (CurrentTimestamp - LastPrintTimestamp > PRINT_INTERVAL)
	{
		Averages[0] = fifo_getAverage( &lightBuff );
		Averages[1] = fifo_getAverage( &tempBuff );
		Averages[2] = fifo_getAverage( &pressureBuff );
		Averages[3] = fifo_getAverage( &humiBuff );
		
		comm::sendPacket(Averages, 4);

		LastPrintTimestamp = CurrentTimestamp;
	}
	
	// Measure temperature and humidity
	if (CurrentTimestamp - LastTempTimestamp > TEMP_MES_INTERVAL)
	{
		float temp, humi;
		hdc::read( &temp, &humi );

		fifo_push( &tempBuff, temp );
		fifo_push( &humiBuff, humi );

		// Detect temperature change
		if (temp >= settings_high_temp){	
			// Turn heating off
			comm::sendEvent(EVT_HEATING_OFF);
		}
		else if (temp < settings_low_temp)
		{
			// Turn heating on
			comm::sendEvent(EVT_HEATING_ON);
		}

		LastTempTimestamp = CurrentTimestamp;
	}

	// Mesure light and pressure
	if (CurrentTimestamp - LastPressTimestamp > LIG_PR_MES_INTERVAL)
	{
		float pressure, light, _temp;
		dps::read( &_temp, &pressure );
		light = opt::read();

		fifo_push( &lightBuff, light);
		fifo_push( &pressureBuff, pressure );

		// Detect pressure change
		if (LastPressure - pressure > PRESS_TRESHOLD) {
			// Pressure drop
			if ((PressureChangeTimestampDown != 0) && (CurrentTimestamp - PressureChangeTimestampDown > PRESSURE_DEBOUNCE_TIME))
			{
				if (LastPressure - pressure > PRESS_TRESHOLD) comm::sendEvent(EVT_PRESS_DOWN);
				PressureChangeTimestampDown = 0;
			}
			else
			{
				PressureChangeTimestampDown = CurrentTimestamp;
			}
		}
		else if (pressure - LastPressure > PRESS_TRESHOLD){
			// Pressure rise
			if ((PressureChangeTimestampUp != 0) && (CurrentTimestamp - PressureChangeTimestampUp > PRESSURE_DEBOUNCE_TIME))
			{
				if (pressure - LastPressure > PRESS_TRESHOLD) comm::sendEvent(EVT_PRESS_UP);
				PressureChangeTimestampUp = 0;
			}
			else
			{
				PressureChangeTimestampUp = CurrentTimestamp;
			}
		}
		LastPressure = pressure;


		// Detect light change
		if(IsLightOn == true){
			if(light >= settings_min_lux){
				// Turn light on
				comm::sendEvent(EVT_LIGHT_ON);
				IsLightOn=false;
			}
			
		}
		else{
			if(light < settings_min_lux){
				// Turn light off
				comm::sendEvent(EVT_LIGHT_OFF);
				IsLightOn=true;
			}

		}

		LastPressTimestamp = CurrentTimestamp;
	}
}