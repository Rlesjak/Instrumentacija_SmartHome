// fifo.h
#pragma once

struct fifo_controller_t {
	const int len;
	float *arr;
	int head;
} ;

// Push float to buffer
void fifo_push(fifo_controller_t *controller, float value);
// Return average of the buffer
float fifo_getAverage(fifo_controller_t *controller);