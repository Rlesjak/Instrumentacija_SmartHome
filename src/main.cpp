#include <Arduino.h>
#include <Wire.h>
#include <opt3001.h>
#include <dps310.h>
#include <fifo.h>
#include <util.h>

#define MES_INTERVAL 200
#define AVG_INTERVAL 6000
#define PRINT_INTERVAL 1000

#define OPT_BUFF_SIZE ROLLING_BUFFER_SIZE(AVG_INTERVAL, MES_INTERVAL)

float _lightReadingsBuffer[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t lightReadingsBuffer = {OPT_BUFF_SIZE, _lightReadingsBuffer, 0};

float _tempReadingsBuffer[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t tempReadingsBuffer = {OPT_BUFF_SIZE, _tempReadingsBuffer, 0};

float _pressureReadingsBuffer[OPT_BUFF_SIZE] = {0.0f};
fifo_controller_t pressureReadingsBuffer = {OPT_BUFF_SIZE, _pressureReadingsBuffer, 0};

unsigned long lastMesTimestamp;
unsigned long lastPrintTimestamp;
unsigned long currentTimestamp;

void setup() {
	Serial.begin(9600);
	opt::configure();
	dps::configure();
	lastMesTimestamp = millis();
	lastPrintTimestamp = millis();
}

void loop() {
	currentTimestamp = millis();
	if (currentTimestamp - lastPrintTimestamp > PRINT_INTERVAL)
	{
		Serial.print("[");

		Serial.print(fifo_getAverage( &lightReadingsBuffer ));
		Serial.print("|");
		Serial.print(fifo_getAverage( &tempReadingsBuffer ));
		Serial.print("|");
		Serial.print(fifo_getAverage( &pressureReadingsBuffer ));

		Serial.println("]");
		lastPrintTimestamp = currentTimestamp;
	}
	
	if (currentTimestamp - lastMesTimestamp > MES_INTERVAL)
	{
		float temp;
		float pressure;
		dps::read(&temp, &pressure);
		fifo_push( &lightReadingsBuffer, opt::read() );
		fifo_push( &tempReadingsBuffer, temp );
		fifo_push( &pressureReadingsBuffer, pressure );
		lastMesTimestamp = currentTimestamp;
	}
}