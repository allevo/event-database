/*
 * net.c
 *
 *  Created on: Jan 7, 2019
 *      Author: tommaso
 */

#include "event-net.h"
#include "logger/log.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define RESPONSE_BUFFER_LENGTH 1024 * 1024


typedef enum { STARTED, ENDED } incoming_client_state_t;
typedef struct {
	pthread_t* thread;
	int client_sock;
	parse_function_t parse_function;
	void* ptr;
	int state;
} incoming_client_t;

void event_net_init(event_net_t* event_net) {
	log_trace("event_net: %p", event_net);
}

int event_net_start_server(event_net_t* event_net) {
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		log_error("could not create listen socket");
		return 1;
	}
	int opt_val = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	if ((bind(listen_sock, (struct sockaddr *)&server_address, sizeof(server_address))) < 0) {
		log_error("could not bind socket");
		return 1;
	}
	int wait_size = 16;
	if (listen(listen_sock, wait_size) < 0) {
		log_error("could not open socket for listening\n");
		return 1;
	}
	struct sockaddr_in client_address;
	unsigned int client_address_len = 0;

	pthread_t threads[THREAD_COUNT];
	incoming_client_t** incoming_clients = calloc(THREAD_COUNT, sizeof(incoming_client_t));

	void* retval;
	while (1) {
		// open a new socket to transmit data per connection
		int sock;
		if ((sock =
				 accept(listen_sock, (struct sockaddr *)&client_address, &client_address_len)) < 0) {
			log_error("could not open a socket to accept data\n");
			return 1;
		}

		for (int i = 0; i < THREAD_COUNT; i++) {
			log_trace("AA %p %d", incoming_clients[i], incoming_clients[i] == NULL);
		}

		log_error("client connected with ip address: %s\n", inet_ntoa(client_address.sin_addr));

		int free_thread_index = 0;
		for (; free_thread_index < THREAD_COUNT; free_thread_index++) {
			log_debug("Is NULL %p %d", incoming_clients[free_thread_index], incoming_clients[free_thread_index] == NULL);
			if (incoming_clients[free_thread_index] == NULL) break;
			log_debug("Is ENDED %d", incoming_clients[free_thread_index]->state);
			if (incoming_clients[free_thread_index]->state == STARTED) continue;

			if (pthread_join(threads[free_thread_index], &retval) != 0) {
				log_error("Unable to join a thread");
				// TODO: do something
			}
			free(incoming_clients[free_thread_index]);
			incoming_clients[free_thread_index] = NULL;

			log_debug("Chosen %d", free_thread_index);
			break;
		}
		log_debug("Found index: %d", free_thread_index);
		if (free_thread_index == THREAD_COUNT) {
			log_error("Unable to find a free thread for incoming client");
			close(sock);
			continue;
		}

		incoming_client_t* incoming_client = malloc(sizeof(incoming_client_t));
		incoming_client->thread = &threads[free_thread_index];
		incoming_client->client_sock = sock;
		incoming_client->parse_function = event_net->parse_function;
		incoming_client->ptr = event_net->ptr;
		incoming_client->state = STARTED;
		incoming_clients[free_thread_index] = incoming_client;

		if(pthread_create(&threads[free_thread_index], NULL, handle_client, incoming_client)) {
			log_error("Error creating thread");
			return 1;
		}
	}

	close(listen_sock);
}

void* handle_client (void* ptr) {
	incoming_client_t* incoming_client = ptr;

	int sock = incoming_client->client_sock;

	long int n = 0;
	size_t maxlen = BUFFER_SIZE;
	char buffer[BUFFER_SIZE];
	char *pbuffer = buffer;

	char response[RESPONSE_BUFFER_LENGTH];
	size_t response_length;

	// keep running as long as the client keeps the connection open
	while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
		log_debug("Received: %.*s (%d)", n, buffer, n);

		incoming_client->parse_function(pbuffer, (size_t) n, response, &response_length, incoming_client->ptr);

		send(sock, response, response_length, 0);
	}

	close(sock);

	incoming_client->state = ENDED;

	return NULL;
}
