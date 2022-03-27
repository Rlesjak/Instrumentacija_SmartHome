#include <fifo.h>

// Buffer head is always kept on the first available(empty) element of the buffer
// it is not kept on the last written element!

void fifo_push(fifo_controller_t *controller, float value)
{
	// Write to current head
	controller->arr[controller->head] = value;

	// Circlar
	if (++controller->head >= controller->len)
		controller->head -= controller->len;
}

float fifo_getAverage(fifo_controller_t *controller)
{
	float avg = 0;
	for (int i = 0; i < controller->len; i++)
		avg += controller->arr[i];
	
	avg = avg/(float)controller->len;
	return avg;
}