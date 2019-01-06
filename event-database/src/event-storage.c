/*
 * event-storage.c
 *
 *  Created on: Jan 6, 2019
 *      Author: tommaso
 */

#include "event-storage.h"

#include "logger/log.h"

#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>

void* write_to_file (void *x_void_ptr) {
	return NULL;
}

int event_storage_init(event_storage_t* event_storage) {
	pipe_t* p = pipe_new(sizeof(event_t*), 0);
	event_storage->consumer = pipe_consumer_new(p);
	if (event_storage->consumer == NULL) {
		pipe_free(p);
		return -1;
	}
	event_storage->producer = pipe_producer_new(p);
	if (event_storage->producer == NULL) {
		pipe_free(p);
		pipe_consumer_free(event_storage->consumer);
		return -1;
	}

	pipe_free(p);

	int x = 0;
	if(pthread_create(&event_storage->writer_thread, NULL, write_to_file, &x)) {
		log_error("Error creating writer_thread");
		pipe_consumer_free(event_storage->consumer);
		pipe_producer_free(event_storage->producer);
		return -1;
	}

	const char* database_file_path = "/tmp/storage.db";
	event_storage->storage_file_fp = open(database_file_path, O_WRONLY | O_APPEND);
	if (event_storage->storage_file_fp == -1) {
		log_error("Error opening database at: %s (%s)", database_file_path, strerror(errno));
		pipe_consumer_free(event_storage->consumer);
		pipe_producer_free(event_storage->producer);
		pthread_cancel(event_storage->writer_thread);
		return -1;
	}
	return 0;
}

int event_storage_add_event(event_storage_t* event_storage, event_t* event) {
	event->internal = &event_storage->storage_file_fp;
	pipe_push(event_storage->producer, event, 1);


	return 0;
}
