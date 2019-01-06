/*
 * event-database-sdk.h
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#ifndef EVENT_DATABASE_SDK_H_
#define EVENT_DATABASE_SDK_H_

#include <stddef.h>
#include <jansson.h>

typedef struct {
	char* data;
	size_t len;
} raw_string_t;

typedef struct {
	raw_string_t name;
	json_t* event_data;
	void* internal;
} event_t;

typedef struct {
	void* user_data;
} state_t;

#endif /* EVENT_DATABASE_SDK_H_ */
