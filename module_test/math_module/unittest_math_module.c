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

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "common/math_module.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE			1

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots callbacks

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

static void TEST_CASE_div_u8(void) {

	UT_START_TEST_CASE("Division u8")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		UT_CHECK_IS_EQUAL(div_u8(255, 1), (255/1));
		UT_CHECK_IS_EQUAL(div_u8(0, 1), (0/1));
		UT_CHECK_IS_EQUAL(div_u8(1, 1), (1/1));
		UT_CHECK_IS_EQUAL(div_u8(13, 13), (13/13));
		UT_CHECK_IS_EQUAL(div_u8(149, 50), (149/50));
		UT_CHECK_IS_EQUAL(div_u8(255, 255), (255/255));
		UT_CHECK_IS_EQUAL(div_u8(1, 255), (1/255));
		UT_CHECK_IS_EQUAL(div_u8(1, 0), 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_div_i8(void) {

	UT_START_TEST_CASE("Division i8")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		UT_CHECK_IS_EQUAL(div_i8(120, -1), (120/-1));
		UT_CHECK_IS_EQUAL(div_i8(0, 1), (0/1));
		UT_CHECK_IS_EQUAL(div_i8(1, 1), (1/1));
		UT_CHECK_IS_EQUAL(div_i8(1, -1), (1/-1));
		UT_CHECK_IS_EQUAL(div_i8(-1, 1), (-1/1));
		UT_CHECK_IS_EQUAL(div_i8(-1, -1), (-1/-1));
		UT_CHECK_IS_EQUAL(div_i8(-13, -13), (-13/-13));
		UT_CHECK_IS_EQUAL(div_i8(-149, 50), (-149/50));
		UT_CHECK_IS_EQUAL(div_i8(120, 120), (120/120));
		UT_CHECK_IS_EQUAL(div_i8(1, 121), (1/121));
		UT_CHECK_IS_EQUAL(div_i8(1, 0), 0);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for json-parser 1.0")
	{
		TEST_CASE_div_u8();
		TEST_CASE_div_i8();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
