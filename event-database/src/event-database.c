/*
 ============================================================================
 Name        : event-database.c
 Author      : Tommaso Allevi
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/log.h"

#include "event-engine.h"
#include "event-net.h"
#include "event-parser.h"

int main(void) {
	log_set_level(0);

	log_info("Starting event database...");

	event_engine_t event_engine;
	event_engine_init(&event_engine);

	event_net_t event_net;
	event_net.parse_function = parse_input_buffer;
	event_net.ptr = &event_engine;
	event_net_init(&event_net);

	int ret = event_net_start_server(&event_net);

	return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
