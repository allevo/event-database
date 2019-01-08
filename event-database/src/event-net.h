/*
 * net.h
 *
 *  Created on: Jan 7, 2019
 *      Author: tommaso
 */

#define BUFFER_SIZE 1024 * 1024
#define THREAD_COUNT 2
#define PORT 54321

#include <stddef.h>

typedef void (*parse_function_t) (char* buffer, size_t len, char* response, size_t* response_length, void* ptr);

typedef struct {
	parse_function_t parse_function;
	void* ptr;
} event_net_t;

void event_net_init(event_net_t* event_net);
int event_net_start_server(event_net_t* event_net);

void* handle_client (void* ptr);
