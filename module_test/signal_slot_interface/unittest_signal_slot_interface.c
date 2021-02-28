/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_signal_slot_interface.c
 * \brief
 * \author	sebastian lesse
 *
 * ----------------------------------------------------------------------------
 */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "common/exception_interface.h"

#include "time_management/time_management.h"

//-----------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

//-----------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_SEND_SIGNAL_OK				1
#define TEST_CASE_ID_DONT_SEND_SIGNAL_IN_SIGNAL_CONTEXT		2
#define TEST_CASE_ID_DONT_SEND_SIGNAL_BEFORE_SIGNAL_TIMEOUT	3

//-----------------------------------------------------------------------------

static u8 counter_CASE_SEND_OK_SIGNAL = 0;
static u8 counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL = 0;
static u8 counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL = 0;
static u8 counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL = 0;
static u8 counter_SIGNAL_CONTEXT_EXCEPTION = 0;
static u8 counter_SIGNAL_PAUSE_EXCEPTION = 0;

//-----------------------------------------------------------------------------

static void unittest_reset_counter(void) {
	counter_CASE_SEND_OK_SIGNAL = 0;
	counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL = 0;
	counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL = 0;
	counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL = 0;
	counter_SIGNAL_CONTEXT_EXCEPTION = 0;
	counter_SIGNAL_PAUSE_EXCEPTION = 0;
}

//-----------------------------------------------------------------------------

static void ut_signal_send_ok_slot_callback(const void* p_argument);

static void ut_signal_send_in_signal_context_slot_callback(const void* p_argument);

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

static unittest_wait_ms(u16 waittime_ms) {

	TIMEOUT_TIMER_start();
	while (TIMEOUT_TIMER_is_up(waittime_ms) == 0) {

	}
}

//-----------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(UT_CASE_SEND_OK_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(UT_CASE_SEND_OK_SIGNAL, UT_CASE_SEND_OK_SLOT, ut_signal_send_ok_slot_callback)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(UT_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(UT_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL, UT_CASE_SEND_IN_SIGNAL_CONTEXT_SLOT, ut_signal_send_in_signal_context_slot_callback)

//-----------------------------------------------------------------------------

EXCEPTION_HANDLE(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_EXCEPTION, DEBUG_PASS("SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_EXCEPTION"); counter_SIGNAL_CONTEXT_EXCEPTION += 1; )
EXCEPTION_HANDLE(SIGNAL_SLOT_INTERFACE_SIGNAL_PAUSE_EXCEPTION, DEBUG_PASS("SIGNAL_SLOT_INTERFACE_SIGNAL_PAUSE_EXCEPTION"); counter_SIGNAL_PAUSE_EXCEPTION += 1; )

//-----------------------------------------------------------------------------

static void UNITTEST_signal_init(void) {
	
	UT_START_TEST_CASE("Signal Init")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);
		unittest_reset_counter();

		UT_CASE_SEND_OK_SIGNAL_init();
		UT_CASE_SEND_OK_SLOT_connect();

		UT_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL_init();
		UT_CASE_SEND_IN_SIGNAL_CONTEXT_SLOT_connect();

		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_CONTEXT_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_PAUSE_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_signal_send_ok(void) {
	
	UT_START_TEST_CASE("Signal Send OK")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SEND_SIGNAL_OK);
		unittest_reset_counter();

		u8 test_byte = 8;

		unittest_wait_ms(6);
		UT_CASE_SEND_OK_SIGNAL_send(NULL);

		unittest_wait_ms(6);
		UT_CASE_SEND_OK_SIGNAL_send((const void*) test_byte);

		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_SIGNAL, 2);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_CONTEXT_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_PAUSE_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_signal_send_in_signal_context(void) {
	
	UT_START_TEST_CASE("Avoid Sending Signals in Signal-Context")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SEND_SIGNAL_OK);
		unittest_reset_counter();

		u32 test_variable = 8;

		UT_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL_send(NULL);
		unittest_wait_ms(6);
		UT_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL_send((const void*) test_variable);

		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL, 2);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_CONTEXT_EXCEPTION, 4);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_PAUSE_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_signal_respect_send_timeout(void) {
	
	UT_START_TEST_CASE("Respect Signal-Send-Timeout")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SEND_SIGNAL_OK);
		unittest_reset_counter();

		u32 test_variable = 8;

		unittest_wait_ms(6);
		UT_CASE_SEND_OK_SIGNAL_send(NULL);
		UT_CASE_SEND_OK_SIGNAL_send((const void*) test_variable);

		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_CONTEXT_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_SIGNAL_PAUSE_EXCEPTION, 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	//TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for Signal-Slot-Interface v1.0")
	{
		UNITTEST_signal_init();
		UNITTEST_signal_send_ok();
		UNITTEST_signal_send_in_signal_context();
		UNITTEST_signal_send_ok();
		UNITTEST_signal_respect_send_timeout();
		UNITTEST_signal_send_ok();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------

static void ut_signal_send_ok_slot_callback(const void* p_argument) {
	DEBUG_PASS("ut_signal_send_ok_slot_callback()");
	counter_CASE_SEND_OK_SIGNAL += 1;

	if (p_argument == NULL) {
		counter_CASE_SEND_OK_WITH_NULL_ARGUMENT_SIGNAL += 1;
	} else {
		counter_CASE_SEND_OK_WITH_VALID_ARGUMENT_SIGNAL += 1;
	}
}

static void ut_signal_send_in_signal_context_slot_callback(const void* p_argument) {

	DEBUG_PASS("ut_signal_send_in_signal_context_slot_callback()");

	counter_CASE_SEND_IN_SIGNAL_CONTEXT_SIGNAL += 1;

	unittest_wait_ms(6);

	u32 test_variable = 8;

	UT_CASE_SEND_OK_SIGNAL_send(NULL);
	UT_CASE_SEND_OK_SIGNAL_send((const void*) test_variable);
}