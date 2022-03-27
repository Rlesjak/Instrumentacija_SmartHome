#include <Arduino.h>
#include <Wire.h>
#include <opt3001.h>
#include <dps310.h>
#include <fifo.h>
#include <util.h>
#include <sh_hdc2080.h>
#include <communication.h>

#define MES_INTERVAL 200
#define AVG_INTERVAL 6000
#define PRINT_INTERVAL 1000

#define OPT_BUFF_SIZE ROLLING_BUFFER_SIZE(AVG_INTERVAL, MES_INTERVAL)

float _lightBuff[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t lightBuff = {OPT_BUFF_SIZE, _lightBuff, 0};

float _tempBuff[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t tempBuff = {OPT_BUFF_SIZE, _tempBuff, 0};

float _pressureBuff[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t pressureBuff = {OPT_BUFF_SIZE, _pressureBuff, 0};

float _temp2Buff[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t temp2Buff = {OPT_BUFF_SIZE, _temp2Buff, 0};

float _humiBuff[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t humiBuff = {OPT_BUFF_SIZE, _humiBuff, 0};


unsigned long lastMesTimestamp;
unsigned long lastPrintTimestamp;
unsigned long currentTimestamp;

void setup() {
	comm::configure(9600);
	opt::configure();
	dps::configure();
	hdc::configure();
	lastMesTimestamp = millis();
	lastPrintTimestamp = millis();
}

void loop() {
	currentTimestamp = millis();
	if (currentTimestamp - lastPrintTimestamp > PRINT_INTERVAL)
	{
		float averages[] = {
			fifo_getAverage( &lightBuff ), 
			fifo_getAverage( &tempBuff ), 
			fifo_getAverage( &pressureBuff ),
			fifo_getAverage( &temp2Buff ),
			fifo_getAverage( &humiBuff )
			};
		
		comm::sendPacket(averages, 5);

		lastPrintTimestamp = currentTimestamp;
	}
	
	if (currentTimestamp - lastMesTimestamp > MES_INTERVAL)
	{
		float temp, temp2, pressure, humi;
		dps::read( &temp, &pressure );
		hdc::read( &temp2, &humi );
		fifo_push( &lightBuff, opt::read() );
		fifo_push( &tempBuff, temp );
		fifo_push( &pressureBuff, pressure );
		fifo_push( &temp2Buff, temp2 );
		fifo_push( &humiBuff, humi );
		lastMesTimestamp = currentTimestamp;
	}
}