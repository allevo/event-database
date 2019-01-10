/*
 * tests.c
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#include "minunit.h"

#include <stdint.h>
#include <time.h>

#include "../event-engine.h"
#include "../event-parser.h"

#include "../logger/log.h"

#define EVENT "{\"type\":\"E\",\"name\":\"my-name\"}"
#define ANOTHER_EVENT "{\"type\":\"E\",\"name\":\"other-name\"}"
#define EVENT_WITH_PARAMS "{\"type\":\"E\",\"name\":\"my-name\",\"params\":{}}"
#define GET_STATE_COMMAND "{\"type\":\"S:G\",\"name\":\"counter\"}"
#define ADD_REDUCER_COMMAND "{\"type\":\"R:A\",\"name\":\"counter\",\"so\":\"../event-database-example/Debug/libevent-database-example.so\",\"rfn\":\"example_counter\",\"sfn\":\"setup_example_counter\",\"ffn\":\"example_get_state_counter\"}"

#define wait_for_a_while() nanosleep((const struct timespec[]){{0, 500000000L}}, NULL) // 500ms

#define setup() command_t* commands[10]; \
	event_engine_t event_engine; \
	event_engine_init(&event_engine);

void test_setup(void) {
	log_set_level(4);
}
void test_teardown(void) { }

MU_TEST(test_parse_empty_buffer) {
	setup()
	const char buffer[1024];

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, 0, commands);

	mu_assert_int_eq(0, (int) parsed_command_count);
}
MU_TEST(test_parse_empty_object) {
	setup()
	const char buffer[1024] = "{}";

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, 2, commands);

	mu_assert_int_eq(0, (int) parsed_command_count);
}
MU_TEST(test_parse_one_event_no_parameters) {
	setup()
	const char buffer[1024] = EVENT;
	event_t* event;

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);

	mu_assert_int_eq(1, (int) parsed_command_count);

	event = commands[0]->command_data;
	mu_assert_stringn_eq("my-name", event->name.data, event->name.len);
	mu_check(event->event_data == NULL);
}
MU_TEST(test_parse_multiple_empty_object) {
	setup()
	const char buffer[1024] = "{}\n{}\n{}\n\n\n{}";

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);

	mu_assert_int_eq(0, (int) parsed_command_count);
}
MU_TEST(test_parse_multiple_events) {
	setup()
	const char buffer[1024] = EVENT "\n" ANOTHER_EVENT "\n" EVENT "\n\n\n\n" EVENT;
	event_t* event;

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);

	mu_assert_int_eq(4, (int) parsed_command_count);

	event = commands[0]->command_data;
	mu_assert_stringn_eq("my-name", event->name.data, event->name.len);
	mu_check(event->event_data == NULL);
	event = commands[1]->command_data;
	mu_assert_stringn_eq("other-name", event->name.data, event->name.len);
	mu_check(event->event_data == NULL);
	event = commands[2]->command_data;
	mu_assert_stringn_eq("my-name", event->name.data, event->name.len);
	mu_check(event->event_data == NULL);
	event = commands[3]->command_data;
	mu_assert_stringn_eq("my-name", event->name.data, event->name.len);
	mu_check(event->event_data == NULL);
}
MU_TEST(test_parse_only_new_lines) {
	setup()
	const char buffer[1024] = "\n\n\n\n\n\n";

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(0, (int) parsed_command_count);
}
MU_TEST(test_parse_wrong_json_1) {
	setup()
	const char buffer[1024] = "{";

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(0, (int) parsed_command_count);
}
MU_TEST(test_parse_wrong_json_2) {
	setup()
	const char buffer[1024] = EVENT "\n{";

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);
}
MU_TEST(test_parse_wrong_json_3) {
	setup()
	const char buffer[1024] = EVENT "\n{\n" EVENT;

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(2, (int) parsed_command_count);
}
MU_TEST(test_parse_wrong_json_4) {
	setup()
	const char buffer[1024] = "{\n" EVENT;

	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);
}
MU_TEST(test_parse_event_with_params) {
	setup()
	const char buffer[1024] = EVENT_WITH_PARAMS;
	event_t* event;
	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);

	event = commands[0]->command_data;
	mu_assert_stringn_eq("my-name", event->name.data, event->name.len);
	mu_check(event->event_data != NULL);
	mu_assert_stringn_eq("{}", json_dumps(event->event_data, 0), 2);
}
MU_TEST(test_parse_get_state) {
	setup()
	const char buffer[1024] = GET_STATE_COMMAND;
	char* state_name;
	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);

	mu_assert_int_eq(GET_STATE, commands[0]->type);

	state_name = commands[0]->command_data;
	mu_assert_string_eq("counter", state_name);
}
MU_TEST(test_parse_add_reducer) {
	setup()
	const char buffer[1024] = ADD_REDUCER_COMMAND;
	add_reducer_command_t* add_reducer;
	size_t parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);

	mu_assert_int_eq(ADD_REDUCER, commands[0]->type);

	add_reducer = (add_reducer_command_t*) commands[0]->command_data;
	mu_assert_string_eq("counter", add_reducer->name);
	mu_check(add_reducer->so_path != NULL);
}

MU_TEST_SUITE(test_parse) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_parse_empty_buffer);
	MU_RUN_TEST(test_parse_empty_object);
	MU_RUN_TEST(test_parse_one_event_no_parameters);
	MU_RUN_TEST(test_parse_only_new_lines);
	MU_RUN_TEST(test_parse_multiple_empty_object);
	MU_RUN_TEST(test_parse_multiple_events);
	MU_RUN_TEST(test_parse_wrong_json_1);
	MU_RUN_TEST(test_parse_wrong_json_2);
	MU_RUN_TEST(test_parse_wrong_json_3);
	MU_RUN_TEST(test_parse_wrong_json_4);
	MU_RUN_TEST(test_parse_event_with_params);

	MU_RUN_TEST(test_parse_get_state);
	MU_RUN_TEST(test_parse_add_reducer);
}


typedef struct {
	uint32_t count;
} example_state_t;
MU_TEST(test_reducer_1) {
	setup()
	const char buffer[1024] = EVENT_WITH_PARAMS;
	const char* example_so_path = "../event-database-example/Debug/libevent-database-example.so";
	event_t* event;

	event_engine_init(&event_engine);

	const char admin_buffer[2014] = ADD_REDUCER_COMMAND;
	size_t parsed_command_count = event_engine_parse(&event_engine, admin_buffer, strlen(admin_buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);
	mu_assert_int_eq(ADD_REDUCER, commands[0]->type);
	add_reducer_command_t* add_reducer_command = commands[0]->command_data;
	mu_assert_string_eq("counter", add_reducer_command->name);
	mu_assert_string_eq(example_so_path, add_reducer_command->so_path);

	int ret = event_engine_add_reducer(&event_engine, add_reducer_command);

	mu_assert_int_eq(0, ret);
	mu_assert_int_eq(1, (int) event_engine.reducers_count);
	mu_assert_string_eq("counter", event_engine.reducers[0].name);
	mu_assert_string_eq(example_so_path, event_engine.reducers[0].library_path);
	mu_check(event_engine.reducers[0].state.user_data != NULL);
	example_state_t* example_state = event_engine.reducers[0].state.user_data;
	mu_assert_int_eq(0, (int) example_state->count);

	parsed_command_count = event_engine_parse(&event_engine, buffer, strlen(buffer), commands);
	mu_assert_int_eq(1, (int) parsed_command_count);

	event = commands[0]->command_data;

	ret = event_engine_dispatch_event(&event_engine, event);

	wait_for_a_while();

	mu_assert_int_eq(0, ret);
	mu_assert_int_eq(1, (int) example_state->count);

	event_engine_dispatch_event(&event_engine, event);
	event_engine_dispatch_event(&event_engine, event);
	event_engine_dispatch_event(&event_engine, event);
	event_engine_dispatch_event(&event_engine, event);

	wait_for_a_while();

	mu_assert_int_eq(5, (int) example_state->count);

	json_t* num = event_engine_get_reducer_state(&event_engine, "counter");

	 mu_check(num != NULL);
	 mu_assert_int_eq(5, (int) json_number_value(num));
}

MU_TEST(test_reducer_2) {
	char buffer1[1024] = ADD_REDUCER_COMMAND "\n" EVENT_WITH_PARAMS "\n";
	char buffer2[1024] = GET_STATE_COMMAND "\n";
	char response[1024];
	size_t response_length;
	event_engine_t event_engine;
	event_engine_init(&event_engine);

	const char* expected_reponse_buffer1 = "\"OK\"\n\"OK\"\n";
	size_t expected_reponse_length_buffer1 = strlen("\"OK\"\n\"OK\"\n");

	parse_input_buffer(buffer1, 1024, response, &response_length, &event_engine);

	mu_assert_int_eq((int) expected_reponse_length_buffer1, (int) response_length);
	mu_assert_stringn_eq(expected_reponse_buffer1, response, expected_reponse_length_buffer1);

	wait_for_a_while();

	parse_input_buffer(buffer2, 1024, response, &response_length, &event_engine);

	mu_assert_int_eq(2, (int) response_length);
	mu_assert_stringn_eq("1\n", response, 2);
}

MU_TEST_SUITE(tests_reducer) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_reducer_1);
	MU_RUN_TEST(test_reducer_2);
}


int main(void) {
	MU_RUN_SUITE(test_parse);
	MU_RUN_SUITE(tests_reducer);
	MU_REPORT();
	return minunit_fail != 0;
}
