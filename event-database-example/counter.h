/*
 * counter.h
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include <stdint.h>
#include <event-database-sdk.h>
#include <jansson.h>

typedef struct {
	uint32_t count;
} example_counter_t;


int example_counter (state_t* state, event_t* event);
int setup_example_counter (state_t* state);
json_t* example_get_state_counter (state_t* state);

#endif /* COUNTER_H_ */
