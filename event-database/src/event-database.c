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

#include "logger/log.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "event-engine.h"

#define PORT 54321
#define BUFFER_SIZE 1024 * 1024

int main(void) {
	log_set_level(2);

	log_info("Starting event database...");

	event_engine_t event_engine;
	event_engine_init(&event_engine);

	command_t* commands[128];
	int parsed_commands_count;

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create listen socket\n");
		return 1;
	}
	int opt_val = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	if ((bind(listen_sock, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return 1;
	}
	int wait_size = 16;
	if (listen(listen_sock, wait_size) < 0) {
		printf("could not open socket for listening\n");
		return 1;
	}
	struct sockaddr_in client_address;
	unsigned int client_address_len = 0;

	while (1) {
		// open a new socket to transmit data per connection
		int sock;
		if ((sock =
				 accept(listen_sock, (struct sockaddr *)&client_address, &client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 0;
		int len = 0, maxlen = BUFFER_SIZE;
		char buffer[BUFFER_SIZE];
		char *pbuffer = buffer;

		printf("client connected with ip address: %s\n",
			   inet_ntoa(client_address.sin_addr));

		// keep running as long as the client keeps the connection open
		while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
			len = n;
			log_debug("Received: %.*s (%d)", len, buffer, len);
			parsed_commands_count = event_engine_parse(&event_engine, buffer, len, commands);

			for (int i = 0; i < parsed_commands_count; i++) {
				command_t* command = commands[i];

				if (command->type == ADD_EVENT) {
					// TODO: handler thre return value corretcly
					event_engine_dispatch_event(&event_engine, (event_t*) command->command_data);
					send(sock, "\"OK\"\n", strlen("\"OK\"\n"), 0);

					free(command->command_data);
				} else if(command->type == GET_STATE) {
					json_t* json = event_engine_get_reducer_state(&event_engine, (char*) command->command_data);
					if (json == NULL) {
						log_warn("Unable to get state");
						send(sock, "{\"R\":\"ERROR\"}\n", strlen("{\"R\":\"ERROR\"}\n"), 0);
					} else {
						char* state = json_dumps(json, JSON_COMPACT | JSON_ENCODE_ANY);
						log_warn("State: %s", state);
						// TODO: remove double sends
						send(sock, state, strlen(state), 0);
						send(sock, "\n", strlen("\n"), 0);
						json_decref(json);
						free(state);
					}
					free(command->command_data);
				} else if (command->type == ADD_REDUCER) {
					add_reducer_command_t* add_reducer_command = commands[i]->command_data;
					// TODO: handler the return value correctly
					event_engine_add_reducer(&event_engine, add_reducer_command);
					send(sock, "\"OK\"\n", strlen("\"OK\"\n"), 0);
					free(add_reducer_command);
				}
			}
		}

		close(sock);
	}

	close(listen_sock);

	return EXIT_SUCCESS;
}
