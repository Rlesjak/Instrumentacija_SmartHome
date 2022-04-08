#include <Arduino.h>
#include <Wire.h>
#include <opt3001.h>
#include <dps310.h>
#include <fifo.h>
#include <util.h>
#include <sh_hdc2080.h>
#include <communication.h>

#define TEMP_MES_INTERVAL 60000
#define TEMP_AVG_INTERVAL 600000
#define TEMP_BUFF_SIZE ROLLING_BUFFER_SIZE(TEMP_AVG_INTERVAL, TEMP_MES_INTERVAL)

#define LIG_PR_MES_INTERVAL 1000
#define LIG_PR_AVG_INTERVAL 10000
#define LIG_PR_BUFF_SIZE ROLLING_BUFFER_SIZE(LIG_PR_AVG_INTERVAL, LIG_PR_MES_INTERVAL)


#define PRINT_INTERVAL 1000

#define MIN_LUX 750
#define PRESS_TRESHOLD 6
#define LOW_TEMP 22
#define HIGH_TEMP 24

float _tempBuff[TEMP_BUFF_SIZE] = {0.0f};
fifo_controller_t tempBuff = {TEMP_BUFF_SIZE, _tempBuff, 0};

float _humiBuff[TEMP_BUFF_SIZE] = {0.0f};
fifo_controller_t humiBuff = {TEMP_BUFF_SIZE, _humiBuff, 0};



float _lightBuff[LIG_PR_BUFF_SIZE] = {0.0f};
fifo_controller_t lightBuff = {LIG_PR_BUFF_SIZE, _lightBuff, 0};

float _pressureBuff[LIG_PR_BUFF_SIZE] = {0.0f};
fifo_controller_t pressureBuff = {LIG_PR_BUFF_SIZE, _pressureBuff, 0};

// Timestamps
unsigned long lastTempTimestamp;
unsigned long lastPressTimestamp;

unsigned long lastPrintTimestamp;
unsigned long currentTimestamp;

// State
bool isLightOn=false;
float lastPressure = 0.0f;
float averages[4];



void setup() {
	comm::configure(9600);
	opt::configure();
	dps::configure();
	hdc::configure();
	lastTempTimestamp = millis();
	lastPressTimestamp = millis();
	lastPrintTimestamp = millis();

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
	currentTimestamp = millis();

	// Send data over serial
	if (currentTimestamp - lastPrintTimestamp > PRINT_INTERVAL)
	{
		averages[0] = fifo_getAverage( &lightBuff );
		averages[1] = fifo_getAverage( &tempBuff );
		averages[2] = fifo_getAverage( &pressureBuff );
		averages[3] = fifo_getAverage( &humiBuff );
		
		comm::sendPacket(averages, 4);

		lastPrintTimestamp = currentTimestamp;
	}
	
	// Measure temperature and humidity
	if (currentTimestamp - lastTempTimestamp > TEMP_MES_INTERVAL)
	{
		float temp, humi;
		hdc::read( &temp, &humi );

		fifo_push( &tempBuff, temp );
		fifo_push( &humiBuff, humi );

		// Detect temperature change
		if (temp >= HIGH_TEMP){	
			// Turn heating off
			comm::sendEvent(30);
		}
		else if (temp < LOW_TEMP)
		{
			// Turn heating on
			comm::sendEvent(31);
		}

		lastTempTimestamp = currentTimestamp;
	}

	// Mesure light and pressure
	if (currentTimestamp - lastPressTimestamp > LIG_PR_MES_INTERVAL)
	{
		float pressure, light, _temp;
		dps::read( &_temp, &pressure );
		light = opt::read();

		fifo_push( &lightBuff, light);
		fifo_push( &pressureBuff, pressure );

		// Detect pressure change
		if (lastPressure - pressure > PRESS_TRESHOLD) {
			// Pressure drop
			comm::sendEvent(20);
		}
		else if (pressure - lastPressure > PRESS_TRESHOLD){
			// Pressure rise
			comm::sendEvent(21);
		}
		lastPressure = pressure;


		// Detect light change
		if(isLightOn == true){
			if(light >= MIN_LUX){
				// Turn light on
				comm::sendEvent(10);
				isLightOn=false;
			}
			
		}
		else{
			if(light < MIN_LUX){
				// Turn light off
				comm::sendEvent(11);
				isLightOn=true;
			}

		}

		lastPressTimestamp = currentTimestamp;
	}
}