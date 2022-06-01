#include <communication.h>
#include <settings.hpp>
#include <util.h>
using namespace comm;

// Local funcs protoypes
void processCommand(char* buff, int len);
void executeWriteCommand(int code, int value);
void printAvailableCodes();

// Init serial comm
boolean comm::configure(int baud){
	Serial.begin(baud);
	return true;
}


// Send data packet
void comm::sendPacket(float *arr, int len)
{
	Serial.print(DATA_START_CHAR);
	Serial.print(arr[0]);
	for ( int i=1; i<len; i++ )
	{
		Serial.print(DATA_DELIMITER);
		Serial.print(arr[i]);
	}
	Serial.println(DATA_END_CHAR);
}

// Send event packet
void comm::sendEvent(int code)
{
	Serial.print(EVT_START_CHAR);
	Serial.print(code);
	Serial.println(EVT_END_CHAR);
}

// Receve commands

// state
// 0 - clear
// 10 - waiting
// 20 - receaving
// 30 - process and execute
namespace comm
{
	#define CMD_BUFF_SIZE 12
	int state = 0;
	int cntr = 0;
	char cmd_buff[CMD_BUFF_SIZE] = {'\0'};
}

// Called by the main while loop
void comm::serviceSerial(void)
{
	switch (state)
	{
		// Clear
		case 0:
			cntr = 0;
			clearCharArray(cmd_buff, CMD_BUFF_SIZE);
			state = 10;
			break;

		// Waiting for data
		case 10:
			if (Serial.available() > 0)
				state = 20;
			break;

		// Reading data byte by byte
		case 20:
			cmd_buff[cntr] = Serial.read();
			if (cmd_buff[cntr] == '\n')
				state = 30;
			// If command is longer than CMD_BUFF_SIZE characters abort
			else if (cntr >= CMD_BUFF_SIZE)
				state = 0;
			cntr++;
			break;

		// Process and execute
		case 30:
			processCommand(cmd_buff, cntr);
			state = 0;
			break;
		default:
			break;
	}
}

// Local funcs

void processCommand(char* buff, int len)
{
	// Detect command start character
	if (buff[0] == COMMAND_START_CHAR)
	{	
		// Find the position of = sign
		int pos = 0;
		for (int i = 0; i < len; i++)
		{
			if (buff[i] == COMMAND_ASSIGN_CHAR)
			{
				pos = i;
				break;
			}
		}
		if (pos == 0)
		{
			// List available codes
			printAvailableCodes();
			return;
		}
		

		// Evaluate the number in between $ and = sign
		char code_ascii[pos-1];
		for (int i = 0; i < pos-1; i++)
		{
			code_ascii[i] = buff[i+1];
		}
		int code = atoi(code_ascii);

		// Evaluate the number after the = sign
		char value_ascii[len-pos-1];
		for (int i = 0; i < len-pos-1; i++)
		{
			value_ascii[i] = buff[i+1+pos];
		}
		int value = atoi(value_ascii);
		
		executeWriteCommand(code, value);
	}
	// Send error
}

void executeWriteCommand(int code, int value)
{
	switch (code)
	{
	case 1:
		settings_low_temp = value;
		break;
	case 2:
		settings_high_temp = value;
		break;
	case 3:
		settings_min_lux = value;
		break;
	
	default:
		Serial.println("NOT OK");
		return;
		break;
	}

	Serial.print("OK - ");
	Serial.println(value);
}

void printAvailableCodes()
{
	Serial.println("Available Codes:");

	Serial.print("$1 - Low Temp - Current = ");
	Serial.println(settings_low_temp);
	Serial.print("$2 - High Temp - Current = ");
	Serial.println(settings_high_temp);
	Serial.print("$3 - Min Lux - Current = ");
	Serial.println(settings_min_lux);
}