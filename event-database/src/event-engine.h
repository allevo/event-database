/*
 * event-engine.h
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#ifndef EVENT_ENGINE_H_
#define EVENT_ENGINE_H_

#include <stddef.h>
#include <dlfcn.h>

#include <jansson.h>

#include <event-database-sdk.h>

typedef int (*setup_state_function_t)(state_t*);
typedef int (*reducer_function_t)(state_t*, event_t*);
typedef json_t* (*formatter_function_t)(state_t*);

typedef struct {
	char* name;
	char* library_path;
	reducer_function_t handler;
	formatter_function_t formatter;
	state_t state;
} reducer_t;

typedef struct {
	size_t reducers_count;
	reducer_t* reducers;
} event_engine_t;

typedef struct {
	char* name;
	char* so_path;
} add_reducer_command_t;

typedef enum { ADD_EVENT, GET_STATE, ADD_REDUCER } command_type_t;
typedef struct {
	command_type_t type;
	void* command_data;
} command_t;


int event_engine_init(event_engine_t*);
size_t event_engine_parse(event_engine_t*, const char*, size_t, command_t** commands);

int event_engine_add_reducer (event_engine_t* event_engine, add_reducer_command_t* add_reducer_command);

int event_engine_dispatch_event (event_engine_t* event_engine, event_t* event);

json_t* event_engine_get_reducer_state (event_engine_t* event_engine, const char* reducer_name);

#endif /* EVENT_ENGINE_H_ */
