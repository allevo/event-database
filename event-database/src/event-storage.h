/*
 * event-storage.h
 *
 *  Created on: Jan 6, 2019
 *      Author: tommaso
 */

#ifndef EVENT_STORAGE_H_
#define EVENT_STORAGE_H_

#include <event-database-sdk.h>

#include "pipe/pipe.h"

typedef struct {
	pipe_consumer_t* consumer;
	pipe_producer_t* producer;

	pthread_t writer_thread;
	int storage_file_fp;
} event_storage_t;

int event_storage_init(event_storage_t* event_storage);
int event_storage_add_event(event_storage_t* event_storage, event_t* event);

#endif /* EVENT_STORAGE_H_ */
