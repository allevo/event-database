/*
 * test.c
 *
 *  Created on: Jan 5, 2019
 *      Author: tommaso
 */

#include "minunit.h"
#include "../counter.h"

static state_t state;
static event_t event;


void test_setup(void) {
	event.name.data = "MY_EVENT_TYPE";
	event.name.len = 34 - 21;
}
void test_teardown(void) { }

MU_TEST(test_ok) {
	setup_example_counter(&state);
	mu_assert_int_eq(((example_counter_t*) state.user_data)->count, 0);

	example_counter(&state, &event);
	mu_assert_int_eq(((example_counter_t*) state.user_data)->count, 1);

	example_counter(&state, &event);
	mu_assert_int_eq(((example_counter_t*) state.user_data)->count, 2);

	example_counter(&state, &event);
	mu_assert_int_eq(((example_counter_t*) state.user_data)->count, 3);

	json_t* num = example_get_state_counter(&state);
	mu_assert_int_eq(3, json_number_value(num));

	state_t wrong_state;
	wrong_state.user_data = NULL;
	num = example_get_state_counter(&wrong_state);
	mu_check(num == NULL);
}

MU_TEST_SUITE(test_suite) {
	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	MU_RUN_TEST(test_ok);
}


int main(int argc, char **argv) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return 0;
}
