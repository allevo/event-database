/*
 * counter.c
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */


#include "counter.h"
#include <assert.h>

int example_counter (state_t* state, event_t* event) {
	example_counter_t* counter = state->user_data;
	counter->count++;
	return 0;
}

int setup_example_counter (state_t* state) {
	state->user_data = malloc(sizeof(example_counter_t));
	example_counter_t* counter = state->user_data;
	counter->count = 0;
	return 0;
}

json_t* example_get_state_counter (state_t* state) {
	example_counter_t* counter = state->user_data;

	if (counter == NULL) {
		return NULL;
	}

	int count = counter->count;
	return json_integer(count);
}
