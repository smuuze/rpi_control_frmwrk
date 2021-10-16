/**
 * @file 	unittest_power_management.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-07-13
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "time_management/time_management.h"
#include "app_tasks/ir_remote_mcu_task.h"
#include "power_management/power_management_interface.h"
#include "power_management/power_module_5V.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION			0
#define TEST_CASE_ID_5V_POWER_ON			1

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS				100

// --------------------------------------------------------------------------------

static u8 counter_5V_POWER_ON_CALLBACK = 0;
static u8 counter_5V_POWER_OFF_CALLBACK = 0;
static u8 counter_UT_POWER_ON_CALLBACK = 0;
static u8 counter_UT_POWER_OFF_CALLBACK = 0;

static void unittest_reset_counter(void) {

	counter_5V_POWER_ON_CALLBACK = 0;
	counter_5V_POWER_OFF_CALLBACK = 0;

	counter_UT_POWER_ON_CALLBACK = 0;
	counter_UT_POWER_OFF_CALLBACK = 0;
}

// --------------------------------------------------------------------------------

// stubs

void ut_power_mgnmnt_5v_power_on_callback(void) {
	DEBUG_PASS("ut_power_mgnmnt_5v_power_on_callback()");
	counter_5V_POWER_ON_CALLBACK += 1;
}

void ut_power_mgnmnt_5v_power_off_callback(void) {
	DEBUG_PASS("ut_power_mgnmnt_5v_power_off_callback()");
	counter_5V_POWER_OFF_CALLBACK += 1;
}

void ut_power_mgnmnt_ut_power_on_callback(void) {
	DEBUG_PASS("ut_power_mgnmnt_ut_power_on_callback()");
	counter_UT_POWER_ON_CALLBACK += 1;
}

void ut_power_mgnmnt_ut_power_off_callback(void) {
	DEBUG_PASS("ut_power_mgnmnt_ut_power_off_callback()");
	counter_UT_POWER_OFF_CALLBACK += 1;
}	

// --------------------------------------------------------------------------------

// slots callbacks

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

#define POWER_UNIT_UT_POWER_UP_TIME_MS			0
#define POWER_UNIT_UT_POWER_DOWN_TIME_MS		0

POWER_MGMN_BUILD_UNIT(POWER_UNIT_UT, POWER_UNIT_UT_POWER_UP_TIME_MS, POWER_UNIT_UT_POWER_DOWN_TIME_MS, ut_power_mgnmnt_ut_power_on_callback, ut_power_mgnmnt_ut_power_off_callback)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UT_TIMER)

// --------------------------------------------------------------------------------

static void TEST_CASE_initialization(void) {

	UT_START_TEST_CASE("Initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
		unittest_reset_counter();

		power_management_initialization();
		POWER_UNIT_UT_init();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5v_power_on(void) {

	UT_START_TEST_CASE("5V Power-On")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);

		POWER_UNIT_5V_request();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);

		u8 power_is_on = 0;

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_UP_TIME_MS * 3) == 0) {

			mcu_task_controller_schedule();

			if (POWER_UNIT_5V_is_on() != 0) {
				power_is_on = 1;
				break;
			}
		}

		UT_CHECK_IS_EQUAL(power_is_on, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_GREATER(UT_TIMER_elapsed(), POWER_UNIT_5V_POWER_UP_TIME_MS - 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_stays_on(void) {

	UT_START_TEST_CASE("5V Stays On")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		u8 power_is_on = 1;

		UT_TIMER_start();
		while (UT_TIMER_is_up(5000) == 0) {

			mcu_task_controller_schedule();

			if (POWER_UNIT_5V_is_on() == 0) {
				power_is_on = 0;
				break;
			}
		}

		UT_CHECK_IS_EQUAL(power_is_on, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_no_power_on_if_is_on(void) {

	UT_START_TEST_CASE("5V no power-on callback if already is on")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		POWER_UNIT_5V_request();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_no_power_if_if_still_requested(void) {

	UT_START_TEST_CASE("5V no power-off if still requested")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		POWER_UNIT_5V_release();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_power_off(void) {

	UT_START_TEST_CASE("5V power-off")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		POWER_UNIT_5V_release();

		u8 power_is_on = 1;

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_DOWN_TIME_MS * 2) == 0) {

			mcu_task_controller_schedule();

			if (POWER_UNIT_5V_is_on() == 0) {
				power_is_on = 0;
				break;
			}
		}

		UT_CHECK_IS_EQUAL(power_is_on, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 0);
		UT_CHECK_IS_GREATER(UT_TIMER_elapsed(), POWER_UNIT_5V_POWER_DOWN_TIME_MS - 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_no_requeest_count_overflow(void) {

	UT_START_TEST_CASE("5V no request-counter overflow")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		u16 i = 0;
		for ( ; i < POWER_MANAGEMENT_MAX_REQUEST_COUNT_VALUE + 1; i++) {
			POWER_UNIT_5V_request();
		}

		u8 power_is_on = 0;

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_UP_TIME_MS * 3) == 0) {

			mcu_task_controller_schedule();

			if (POWER_UNIT_5V_is_on() != 0) {
				power_is_on = 1;
				break;
			}
		}

		UT_CHECK_IS_EQUAL(power_is_on, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_GREATER(UT_TIMER_elapsed(), POWER_UNIT_5V_POWER_UP_TIME_MS - 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_no_release_count_overflow(void) {

	UT_START_TEST_CASE("5V no release-counter overflow")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		u16 i = 0;
		for ( ; i < POWER_MANAGEMENT_MAX_REQUEST_COUNT_VALUE + 1; i++) {
			POWER_UNIT_5V_release();
		}

		u8 power_is_on = 1;

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_DOWN_TIME_MS * 2) == 0) {

			mcu_task_controller_schedule();

			if (POWER_UNIT_5V_is_on() == 0) {
				power_is_on = 0;
				break;
			}
		}

		UT_CHECK_IS_EQUAL(power_is_on, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 0);
		UT_CHECK_IS_GREATER(UT_TIMER_elapsed(), POWER_UNIT_5V_POWER_DOWN_TIME_MS - 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_5V_stay_on_if_requested_on_ramp_down(void) {

	UT_START_TEST_CASE("5V stay on if requested on ramp-down")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		POWER_UNIT_5V_release();

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_DOWN_TIME_MS / 2) == 0) {

			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);

		POWER_UNIT_5V_request();

		UT_TIMER_start();
		while (UT_TIMER_is_up(POWER_UNIT_5V_POWER_DOWN_TIME_MS) == 0) {

			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 0);

	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_UT_power_on_immediately(void) {

	UT_START_TEST_CASE("UT-Power-Unit power-on immediately")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_5V_POWER_ON);
		unittest_reset_counter();

		POWER_UNIT_UT_request();
		mcu_task_controller_schedule();

		UT_CHECK_IS_EQUAL(counter_5V_POWER_ON_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_5V_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_5V_is_on(), 0);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_ON_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_UT_POWER_OFF_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(POWER_UNIT_UT_is_on(), 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST of the Power-Management Module")
	{
		TEST_CASE_initialization();

		TEST_CASE_5v_power_on();
		TEST_CASE_5V_stays_on();
		TEST_CASE_5V_no_power_on_if_is_on();

		TEST_CASE_5V_no_power_if_if_still_requested();
		TEST_CASE_5V_power_off();

		TEST_CASE_5V_no_requeest_count_overflow();
		TEST_CASE_5V_no_release_count_overflow();

		TEST_CASE_5v_power_on();
		TEST_CASE_5V_stay_on_if_requested_on_ramp_down();
		TEST_CASE_5V_power_off();

		TEST_CASE_UT_power_on_immediately();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
