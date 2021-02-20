/**
 * @file 	unittest_json_parser.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-06
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_NOT_ACTIVE_AFTER_STOP			1
#define TEST_CASE_ID_ACTIVE_AFTER_START				2
#define TEST_CASE_ID_15_MS_PASSED				3
#define TEST_CASE_ID_255_MS_PASSED				4
#define TEST_CASE_ID_256_MS_PASSED				5
#define TEST_CASE_ID_65535_MS_PASSED				6
#define TEST_CASE_ID_65536_MS_PASSED				7
#define TEST_CASE_ID_4294967295_MS_PASSED			8
#define TEST_CASE_ID_4294967296_MS_PASSED			9
#define TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW			10
#define TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK			11
#define TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW_P2		12
#define TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK_P2		13

// --------------------------------------------------------------------------------

// stubs

void clock_driver_init(void) {

}

void rtc_driver_init(void) {

}

u32 local_rtc_timer_gettime_u32(void) {
	
	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_NOT_ACTIVE_AFTER_STOP :
		case TEST_CASE_ID_ACTIVE_AFTER_START :
			return 0;

		case TEST_CASE_ID_15_MS_PASSED :
			return 15;

		case TEST_CASE_ID_255_MS_PASSED:
			return 255;

		case TEST_CASE_ID_256_MS_PASSED:
			return 256;

		case TEST_CASE_ID_65535_MS_PASSED:
			return 65535;

		case TEST_CASE_ID_65536_MS_PASSED:
			return 65536;

		case TEST_CASE_ID_4294967295_MS_PASSED:
			return 4294967295;

		case TEST_CASE_ID_4294967296_MS_PASSED:
			return (u32)(4294967295 + 1);

		case TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW:
			return (u32)(4294967295 - 50);

		case TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK:
			return (u32)(4294967295 + 50 );

		case TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW_P2:
			return 4294967295;

		case TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK_P2:
			return 99;

		default:
			return 0;
	}
}

u8 local_rtc_timer_gettime_u8(void) {
	return (u8)local_rtc_timer_gettime_u32();
}

u16 local_rtc_timer_gettime_u16(void) {
	return (u16)local_rtc_timer_gettime_u32();
}

// --------------------------------------------------------------------------------

// slots callbacks

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U8(UT_U8_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(UT_U16_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U32(UT_U32_TIMER)

// --------------------------------------------------------------------------------

static void TEST_CASE_not_active_after_stop(void) {

	UT_START_TEST_CASE("Not active after stop")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_NOT_ACTIVE_AFTER_STOP);

		UT_U8_TIMER_stop();
		UT_U16_TIMER_stop();
		UT_U32_TIMER_stop();

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_active(), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_active(), 0);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_active(), 0);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_active_after_start(void) {

	UT_START_TEST_CASE("Active after start")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ACTIVE_AFTER_START);

		UT_U8_TIMER_start();
		UT_U16_TIMER_start();
		UT_U32_TIMER_start();

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_active(), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_active(), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_active(), 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_15ms_passed(void) {

	UT_START_TEST_CASE("15 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_15_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 15);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 15);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 15);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(14), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(15), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(16), 0);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(14), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(15), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(16), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(14), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(15), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(16), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_255ms_passed(void) {

	UT_START_TEST_CASE("255 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_255_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 255);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 255);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 255);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(254), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(255), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(0), 1);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(254), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(255), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(256), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(254), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(255), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(256), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_256ms_passed(void) {

	UT_START_TEST_CASE("256 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_256_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 256);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 256);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(255), 0);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(0), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(1), 0);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(255), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(256), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(257), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(255), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(256), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(257), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_65535ms_passed(void) {

	UT_START_TEST_CASE("65535 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_65535_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 255);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 65535);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 65535);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(65534), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(65535), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(0), 1);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65534), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65535), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65536), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_65536ms_passed(void) {

	UT_START_TEST_CASE("65536 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_65536_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 65536);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(65534), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(65535), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(0), 1);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65535), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65536), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(65537), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_4294967295ms_passed(void) {

	UT_START_TEST_CASE("4294967295 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_4294967295_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 255);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 65535);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 4294967295);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(4294967294), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(4294967295), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(0), 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_4294967296ms_passed(void) {

	UT_START_TEST_CASE("4294967296 ms passed")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_4294967296_MS_PASSED);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 0);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(4294967295), 0);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(0), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(1), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_overflow_on_timeout_check(void) {

	UT_START_TEST_CASE("overflow_on_timeout_check")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW);

		UT_U8_TIMER_start();
		UT_U16_TIMER_start();
		UT_U32_TIMER_start();

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_start_time(), 205);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_start_time(), 65485);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_start_time(), 4294967245);

		UT_SET_TEST_CASE_ID(TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 100);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 100);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 100);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(101), 0);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(101), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(101), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_overflow_on_timeout_check_part2(void) {

	UT_START_TEST_CASE("overflow_on_timeout_check part2")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GET_TIME_BEFORE_OVERFLOW_P2);

		UT_U8_TIMER_start();
		UT_U16_TIMER_start();
		UT_U32_TIMER_start();

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_start_time(), 255);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_start_time(), 65535);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_start_time(), 4294967295);

		UT_SET_TEST_CASE_ID(TEST_CASE_ID_OVERFLOW_ON_TIMEOUT_CHECK_P2);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_elapsed(), 100);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_elapsed(), 100);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_elapsed(), 100);

		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U8_TIMER_is_up(101), 0);

		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U16_TIMER_is_up(101), 0);

		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(99), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(100), 1);
		UT_CHECK_IS_EQUAL(UT_U32_TIMER_is_up(101), 0);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for Time-Management 1.0")
	{
		TEST_CASE_not_active_after_stop();
		TEST_CASE_active_after_start();
		TEST_CASE_15ms_passed();
		TEST_CASE_255ms_passed();
		TEST_CASE_256ms_passed();
		TEST_CASE_65535ms_passed();
		TEST_CASE_65536ms_passed();
		TEST_CASE_4294967295ms_passed();
		TEST_CASE_4294967296ms_passed();

		TEST_CASE_not_active_after_stop();
		TEST_CASE_active_after_start();
		TEST_CASE_overflow_on_timeout_check();
		TEST_CASE_overflow_on_timeout_check_part2();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
