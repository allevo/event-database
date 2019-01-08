/*
 * event-parser.c
 *
 *  Created on: Jan 7, 2019
 *      Author: tommaso
 */

#include <string.h>

#include <jansson.h>

#include "event-engine.h"
#include "logger/log.h"

void parse_input_buffer (char* buffer, size_t len, char* response, size_t* response_length, void* ptr) {
	log_debug("Parsing function");
	event_engine_t* event_engine = ptr;

	size_t parsed_commands_count;
	command_t* commands[128];

	*response_length = 0;

	parsed_commands_count = event_engine_parse(event_engine, buffer, len, commands);

	for (long unsigned int i = 0; i < parsed_commands_count; i++) {
		command_t* command = commands[i];

		log_debug("Command type: %d", command->type);

		if (command->type == ADD_REDUCER) {
			add_reducer_command_t* add_reducer_command = commands[i]->command_data;
			// TODO: handler the return value correctly
			event_engine_add_reducer(event_engine, add_reducer_command);

			size_t l = strlen("\"OK\"\n");
			memcpy(&response[*response_length], "\"OK\"\n", l);
			*response_length += l;
		}

		if (command->type == ADD_EVENT) {

			// TODO: handler the return value correctly
			event_engine_dispatch_event(event_engine, (event_t*) command->command_data);

			size_t l = strlen("\"OK\"\n");
			memcpy(&response[*response_length], "\"OK\"\n", l);
			*response_length += l;
		} else if(command->type == GET_STATE) {
			json_t* json = event_engine_get_reducer_state(event_engine, (char*) command->command_data);
			if (json == NULL) {
				log_warn("Unable to get state");

				size_t l = strlen("{\"R\":\"ERROR\"}\n");
				memcpy(&response[*response_length], "{\"R\":\"ERROR\"}\n", l);
				*response_length += l;
			} else {
				char* state = json_dumps(json, JSON_COMPACT | JSON_ENCODE_ANY);
				log_trace("State: %s", state);

				size_t l = strlen(state);
				memcpy(&response[*response_length], state, l);
				*response_length += l;
				memcpy(&response[*response_length], "\n", 1);
				*response_length += 1;
				json_decref(json);
				free(state);
			}
		}

		if (command->command_data) {
			free(command->command_data);
		}
	}

	log_debug("Response %.*s (%d)", *response_length, response, *response_length);
}
