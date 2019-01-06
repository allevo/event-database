/*
 * event-engine.c
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#include "event-engine.h"
#include "logger/log.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int event_engine_init(event_engine_t* event_engine) {
	event_engine->reducers_count = 0;
	event_engine->reducers = malloc(sizeof(reducer_t) * event_engine->reducers_count);
	if (event_engine->reducers == NULL) return -1;
	return 0;
}

int event_engine_add_reducer (event_engine_t* event_engine, add_reducer_command_t* add_reducer_command) {
	const char* setup_function_name = "setup_example_counter";
	const char* reducer_function_name = "example_counter";
	const char* formatter_function_name = "example_get_state_counter";
	const char* name = add_reducer_command->name;
	const char* file_path = add_reducer_command->so_path;
	char *error;

	void* handle = dlopen(file_path, RTLD_LAZY);
	if (!handle) {
		log_error("%s: %s", file_path, dlerror());
		return -1;
	}
	dlerror();

	event_engine->reducers = realloc(event_engine->reducers, sizeof(reducer_t) * ++event_engine->reducers_count);

	reducer_t* reducer = &event_engine->reducers[event_engine->reducers_count - 1];
	reducer->library_path = malloc(sizeof(char*) * strlen(file_path));
	strcpy(reducer->library_path, file_path);
	log_debug("Opening file at %s", reducer->library_path);


	setup_state_function_t setup_state_function;
	setup_state_function = dlsym(handle, setup_function_name);
	if ((error = dlerror()) != NULL)  {
		free(reducer);
		dlclose(handle);
		log_error("Error on getting %s at %s", setup_function_name, file_path);
		return -1;
	}

	reducer_function_t reducer_function;
	reducer_function = dlsym(handle, reducer_function_name);
	if ((error = dlerror()) != NULL)  {
		free(reducer);
		dlclose(handle);
		log_error("Error on getting %s at %s", reducer_function_name, file_path);
		return -1;
	}

	formatter_function_t formatter_function;
	formatter_function = dlsym(handle, formatter_function_name);
	if ((error = dlerror()) != NULL)  {
		free(reducer);
		dlclose(handle);
		log_error("Error on getting %s at %s", formatter_function_name, file_path);
		return -1;
	}

	int res = setup_state_function(&(reducer->state));
	if (res != 0) {
		free(reducer);
		dlclose(handle);
		log_error("Error on initializing %s at %s", setup_function_name, file_path);
		return -1;
	}

	reducer->handler = reducer_function;
	reducer->formatter = formatter_function;
	reducer->name = malloc(sizeof(char) * strlen(name));
	strcpy(reducer->name, name);

	log_info("Created new reducer: %s", reducer->name);

	return 0;
}

command_t* create_command_from_json (event_engine_t* event_engine, json_t* json) {
	if (!json_is_object(json)) return NULL;
	json_t* command_type_json = json_object_get(json, "type");
	if (command_type_json == NULL || !json_is_string(command_type_json)) return NULL;

	const char* command_type = json_string_value(command_type_json);
	command_t* command = malloc(sizeof(command_t));

	if (strcmp(command_type, "E") == 0) {
		command->type = ADD_EVENT;

		json_t* event_name = json_object_get(json, "name");
		if (event_name == NULL || !json_is_string(event_name)) return NULL;

		json_t* event_params = json_object_get(json, "params");

		event_t* event = malloc(sizeof(event_t));

		// avoid to free this object for now
		if (event_params != NULL) {
			json_incref(event_params);
		}
		event->event_data = event_params;

		const char* event_name_const = json_string_value(event_name);

		size_t event_name_len = json_string_length(event_name);
		event->name.data = malloc(sizeof(char*) * event_name_len);
		memcpy(event->name.data, event_name_const, event_name_len);
		event->name.len = event_name_len;
		log_info("Event parsed: %.*s (%u)", event_name_len, event->name.data, event_name_len);

		command->command_data = event;
		return command;
	} else if (strcmp(command_type, "S:G") == 0) {
		command->type = GET_STATE;
		json_t* reducer_name = json_object_get(json, "name");
		if (reducer_name == NULL || !json_is_string(reducer_name)) return NULL;

		const char* reducer_name_type = json_string_value(reducer_name);
		command->command_data = malloc(sizeof(char) * strlen(reducer_name_type));
		strcpy(command->command_data, reducer_name_type);

		log_info("Get state parsed: %s", (char*) command->command_data);
		return command;
	} else if (strcmp(command_type, "R:A") == 0) {
		command->type = ADD_REDUCER;
		json_t* reducer_name = json_object_get(json, "name");
		if (reducer_name == NULL || !json_is_string(reducer_name)) return NULL;
		json_t* reducer_so_path = json_object_get(json, "so");
		if (reducer_so_path == NULL || !json_is_string(reducer_so_path)) return NULL;

		add_reducer_command_t* add_reducer = malloc(sizeof(add_reducer_command_t));
		add_reducer->name = malloc(sizeof(char) * json_string_length(reducer_name));
		memcpy(add_reducer->name, json_string_value(reducer_name), json_string_length(reducer_name) + 1);

		add_reducer->so_path = malloc(sizeof(char) * json_string_length(reducer_so_path));
		memcpy(add_reducer->so_path, json_string_value(reducer_so_path), json_string_length(reducer_so_path) + 1);

		command->command_data = add_reducer;
		return command;
	} else {
		free(command);
		return NULL;
	}
}

size_t event_engine_parse(event_engine_t* event_engine, const char* buffer, size_t len, command_t** commands) {
	size_t diff;
	size_t count = 0;
	uint32_t i = 0;
	uint32_t start_event = 0;

	json_t* json = NULL;
	command_t* command;

	json_error_t json_error;
	while (i < len) {
		log_debug("Parsing... %u %u %c", i, len, buffer[i]);
		if (!(buffer[i++] == '\n' || i == len)) continue;
		diff = i - start_event;
		if (diff <= 0) continue;

		log_debug("Loading json: %.*s (%u)", diff, &buffer[start_event], diff);

		// TODO: Avoid parsing non object
		json = json_loadb(&buffer[start_event], diff, 0, &json_error);

		start_event = i;
		if (!json) {
			log_warn("Invalid json: %.*s (%u)", diff, &buffer[start_event - diff], diff);
			continue;
		}

		log_debug("Creating command...");
		command = create_command_from_json(event_engine, json);
		if (command != NULL) {
			commands[count] = command;
			count ++;
		} else {
			log_warn("Invalid event!");
		}

		start_event = i;
	}

	return count;
}

int event_engine_dispatch_event (event_engine_t* event_engine, event_t* event) {
	log_debug("Dispatching event: %.*s", event->name.len, event->name.data);
	for (int i = 0; i < event_engine->reducers_count; i++) {
		reducer_t* reducer = &event_engine->reducers[i];

		reducer->handler(&(reducer->state), event);
	}
	return 0;
}

json_t* event_engine_get_reducer_state (event_engine_t* event_engine, const char* reducer_name) {
	reducer_t* reducer = NULL;

	for (int i = 0; i < event_engine->reducers_count; i++) {
		log_debug("Reducer name %s === %s", event_engine->reducers[i].name, reducer_name);

		if (strcmp(reducer_name, event_engine->reducers[i].name) != 0) continue;
		log_debug("Reducer found");
		reducer = &event_engine->reducers[i];
	}

	if (reducer == NULL) {
		log_warn("Reducer '%s' not found", reducer_name);
		return NULL;
	}

	log_trace("reducer->formatter %p with state %p", reducer->formatter, reducer->state.user_data);
	json_t* output = reducer->formatter(&(reducer->state));
	log_trace("Formatter output %p", output);
	if (output == NULL) {
		log_warn("Unable to format the state: %s", reducer_name);
		return NULL;
	}

	return output;
}
