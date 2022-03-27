#include <communication.h>
using namespace comm;

boolean comm::configure(int baud){
	Serial.begin(baud);
	return true;
}

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