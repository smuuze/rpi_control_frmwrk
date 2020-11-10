/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_qeue_interface.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/qeue_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"
UT_ACTIVATE()

// --------------------------------------------------------------------------------

// Test-Case IDs

#define TEST_CASE_ID_INITIALIZE				0
#define TEST_CASE_ID_ENQEUE				1
#define TEST_CASE_ID_IS_FULL				2
#define TEST_CASE_ID_IS_EMPTY				2

// --------------------------------------------------------------------------------

// Test-Case constants

#define UNITTEST_QEUE_MAX_SIZE				2
#define UNITTEST_MSG_TYPE_MAX_LENGTH			32

#define UNITTEST_MESSAGE_01				"Hello i'm a string"
#define UNITTEST_MESSAGE_02				"I#m another string"

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(UNITTEST_U8_QEUE, u8, sizeof(u8), UNITTEST_QEUE_MAX_SIZE)
QEUE_INTERFACE_BUILD_QEUE(UNITTEST_MSG_QEUE, char, UNITTEST_MSG_TYPE_MAX_LENGTH, UNITTEST_QEUE_MAX_SIZE)

// --------------------------------------------------------------------------------

void ut_prepare_message(char* p_msg, const char* p_string) {

	memset(p_msg, '\0', UNITTEST_MSG_TYPE_MAX_LENGTH);
	memcpy(p_msg, p_string, strlen(p_string));
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

// signal slots

// --------------------------------------------------------------------------------

// test cases

static void UNITTEST_qeue_interface_init(void) {
	
	UT_START_TEST_CASE("QEUE_INTERFACE_INITIALIZE")
	{	
		SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		UNITTEST_U8_QEUE_init();

		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);

		UNITTEST_MSG_QEUE_init();

		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_qeue_interface_enqeue_elements(void) {
	
	UT_START_TEST_CASE("QEUE_INTERFACE_ENQEUE")
	{	
		SET_TEST_CASE_ID(TEST_CASE_ID_ENQEUE);

		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);

		u8 val_to_enqeue = 1;
		u8 enqeue_success = UNITTEST_U8_QEUE_enqeue(&val_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);
		
		u8 val_to_deqeue = 0;
		u8 deqeue_success = UNITTEST_U8_QEUE_deqeue(&val_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_CHECK_IS_EQUAL(val_to_deqeue, val_to_enqeue);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);

		// ----

		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

		char msg_to_enqeue[UNITTEST_MSG_TYPE_MAX_LENGTH];
		ut_prepare_message(msg_to_enqeue, UNITTEST_MESSAGE_01);

		enqeue_success = UNITTEST_MSG_QEUE_enqeue(msg_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		char msg_to_deqeue[UNITTEST_MSG_TYPE_MAX_LENGTH];

		deqeue_success = UNITTEST_MSG_QEUE_deqeue(msg_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_COMPARE_STRING(msg_to_deqeue, UNITTEST_MESSAGE_01);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

	}
	UT_END_TEST_CASE()
}

static void UNITTEST_qeue_interface_qeue_is_full(void) {
	
	UT_START_TEST_CASE("QEUE_INTERFACE_IS_FULL")
	{	
		SET_TEST_CASE_ID(TEST_CASE_ID_IS_FULL);

		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);

		u8 val_to_enqeue = 1;
		u8 enqeue_success = UNITTEST_U8_QEUE_enqeue(&val_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);

		val_to_enqeue = 2;
		enqeue_success = UNITTEST_U8_QEUE_enqeue(&val_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);

		val_to_enqeue = 3;
		enqeue_success = UNITTEST_U8_QEUE_enqeue(&val_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);

		val_to_enqeue = 4;
		enqeue_success = UNITTEST_U8_QEUE_enqeue(&val_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);

		// ----

		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

		char msg_to_enqeue[UNITTEST_MSG_TYPE_MAX_LENGTH];
		ut_prepare_message(msg_to_enqeue, UNITTEST_MESSAGE_01);

		enqeue_success = UNITTEST_MSG_QEUE_enqeue(msg_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		ut_prepare_message(msg_to_enqeue, UNITTEST_MESSAGE_02);

		enqeue_success = UNITTEST_MSG_QEUE_enqeue(msg_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		enqeue_success = UNITTEST_MSG_QEUE_enqeue(msg_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		enqeue_success = UNITTEST_MSG_QEUE_enqeue(msg_to_enqeue);

		UT_CHECK_IS_EQUAL(enqeue_success, 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_qeue_interface_qeue_is_empty(void) {
	
	UT_START_TEST_CASE("QEUE_INTERFACE_IS_EMPTY")
	{	
		SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);
		
		u8 val_to_deqeue = 0;
		u8 deqeue_success = UNITTEST_U8_QEUE_deqeue(&val_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_CHECK_IS_EQUAL(val_to_deqeue, 1);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 0);
		
		deqeue_success = UNITTEST_U8_QEUE_deqeue(&val_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_CHECK_IS_EQUAL(val_to_deqeue, 2);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);
		
		deqeue_success = UNITTEST_U8_QEUE_deqeue(&val_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 0);
		UT_CHECK_IS_EQUAL(val_to_deqeue, 2);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);
		
		deqeue_success = UNITTEST_U8_QEUE_deqeue(&val_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 0);
		UT_CHECK_IS_EQUAL(val_to_deqeue, 2);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_U8_QEUE_is_empty(), 1);

		// ----

		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 1);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		char msg_to_deqeue[UNITTEST_MSG_TYPE_MAX_LENGTH];

		deqeue_success = UNITTEST_MSG_QEUE_deqeue(msg_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_COMPARE_STRING(msg_to_deqeue, UNITTEST_MESSAGE_01);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 0);

		deqeue_success = UNITTEST_MSG_QEUE_deqeue(msg_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 1);
		UT_COMPARE_STRING(msg_to_deqeue, UNITTEST_MESSAGE_02);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

		deqeue_success = UNITTEST_MSG_QEUE_deqeue(msg_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 0);
		UT_COMPARE_STRING(msg_to_deqeue, UNITTEST_MESSAGE_02);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

		deqeue_success = UNITTEST_MSG_QEUE_deqeue(msg_to_deqeue);

		UT_CHECK_IS_EQUAL(deqeue_success, 0);
		UT_COMPARE_STRING(msg_to_deqeue, UNITTEST_MESSAGE_02);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_full(), 0);
		UT_CHECK_IS_EQUAL(UNITTEST_MSG_QEUE_is_empty(), 1);

	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for qeue-interface v1.0")
	{
		UNITTEST_qeue_interface_init();
		UNITTEST_qeue_interface_enqeue_elements();
	TRACER_ENABLE();
		UNITTEST_qeue_interface_qeue_is_full();
	TRACER_DISABLE();
		UNITTEST_qeue_interface_qeue_is_empty();
	}
	UT_END_TESTBENCH()
}

// --------------------------------------------------------------------------------