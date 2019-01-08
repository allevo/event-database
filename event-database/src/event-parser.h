/*
 * event-parser.h
 *
 *  Created on: Jan 7, 2019
 *      Author: tommaso
 */

#ifndef EVENT_PARSER_H_
#define EVENT_PARSER_H_

void parse_input_buffer (char* buffer, size_t len, char* response, size_t* response_length, void* ptr);

#endif /* EVENT_PARSER_H_ */
