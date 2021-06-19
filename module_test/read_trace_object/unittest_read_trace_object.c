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
#include "common/qeue_interface.h"
#include "common/common_tools_string.h"
#include "common/signal_slot_interface.h"
#include "driver/cfg_driver_interface.h"

#include "ui/file_interface/file_interface.h"
#include "time_management/time_management.h"

#include "app_tasks/thread_read_trace_object.h"
#include "mcu_task_management/mcu_task_controller.h"
#include "tracer/trace_object.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INIT							1
#define TEST_CASE_ID_TERMINATE							2
#define TEST_CASE_ID_TRACE_PASS							3
#define TEST_CASE_ID_TRACE_BYTE							4
#define TEST_CASE_ID_TRACE_WORD							5
#define TEST_CASE_ID_TRACE_LONG							6
#define TEST_CASE_ID_TRACE_ARRAY						7
#define TEST_CASE_ID_TRACE_ARRAY_MAX_LENGTH					8
#define TEST_CASE_ID_TRACE_INCOMPLETE						9
#define TEST_CASE_ID_TRACE_INCORRECT						10
#define TEST_CASE_ID_TRACE_INCORRECT_NUMBER_OF_BYTES_TOO_LOW			11

// --------------------------------------------------------------------------------

#define TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE					10
#define TRACER_RAW_TRACE_OBJECT_QEUE_SIZE					10

#define CONSOLE_OUTPUT_MAX_LINE_LEGNTH						2048
#define UT_DATA_BUFFER_MAX_LENGTH						1024

// --------------------------------------------------------------------------------

#define UNITTEST_GET_ARRAY_FROM_WORD_MSB(word)					(u8)(word >> 8), (u8)(word)

// --------------------------------------------------------------------------------

#define UNITTEST_WAIT_MSEC(milli_sec)						UNITTEST_TIMER_start();				\
										while (UNITTEST_TIMER_is_up(milli_sec) == 0) {	\
											mcu_task_controller_schedule();		\
										}						\
										UNITTEST_TIMER_stop()

// --------------------------------------------------------------------------------

// Unittest Data

#define UT_READ_TRACE_OBJECT_PASS_NUMBER_OF_BYTES				0x00, 0x1C
#define UT_READ_TRACE_OBJECT_PASS_LINE_NUMBER					111
#define UT_READ_TRACE_OBJECT_PASS_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','p','a','s','s','.','t','x','t'
#define UT_READ_TRACE_OBJECT_PASS_DATA_ARRAY					TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_PASS_NUMBER_OF_BYTES,				\
										TRACER_TRACE_TYPE_RAW_PASS,						\
										/* TRACE DATA HERE - IF AVAILABLE */					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_PASS_LINE_NUMBER),\
										UT_READ_TRACE_OBJECT_PASS_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY

#define UT_READ_TRACE_OBJECT_BYTE_NUMBER_OF_BYTES				0x00, 0x1D
#define UT_READ_TRACE_OBJECT_BYTE_TRACE_DATA					0xAA
#define UT_READ_TRACE_OBJECT_BYTE_LINE_NUMBER					133
#define UT_READ_TRACE_OBJECT_BYTE_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','b','y','t','e','.','t','x','t'
#define UT_READ_TRACE_OBJECT_BYTE_DATA_ARRAY					TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_BYTE_NUMBER_OF_BYTES,				\
										TRACER_TRACE_TYPE_RAW_BYTE,						\
										UT_READ_TRACE_OBJECT_BYTE_TRACE_DATA,					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_BYTE_LINE_NUMBER),\
										UT_READ_TRACE_OBJECT_BYTE_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY

#define UT_READ_TRACE_OBJECT_WORD_NUMBER_OF_BYTES				0x00, 0x1E
#define UT_READ_TRACE_OBJECT_WORD_TRACE_DATA					0xAA, 0xBB
#define UT_READ_TRACE_OBJECT_WORD_LINE_NUMBER					456
#define UT_READ_TRACE_OBJECT_WORD_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','w','o','r','d','.','t','x','t'
#define UT_READ_TRACE_OBJECT_WORD_DATA_ARRAY					TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_WORD_NUMBER_OF_BYTES,				\
										TRACER_TRACE_TYPE_RAW_WORD,						\
										UT_READ_TRACE_OBJECT_WORD_TRACE_DATA,					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_WORD_LINE_NUMBER),\
										UT_READ_TRACE_OBJECT_WORD_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY

#define UT_READ_TRACE_OBJECT_LONG_NUMBER_OF_BYTES				0x00, 0x20
#define UT_READ_TRACE_OBJECT_LONG_TRACE_DATA					0xAA, 0xBB, 0xCC, 0xDD
#define UT_READ_TRACE_OBJECT_LONG_LINE_NUMBER					789
#define UT_READ_TRACE_OBJECT_LONG_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','l','o','n','g','.','t','x','t'
#define UT_READ_TRACE_OBJECT_LONG_DATA_ARRAY					TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_LONG_NUMBER_OF_BYTES,				\
										TRACER_TRACE_TYPE_RAW_LONG,						\
										UT_READ_TRACE_OBJECT_LONG_TRACE_DATA,					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_LONG_LINE_NUMBER),\
										UT_READ_TRACE_OBJECT_LONG_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY

#define UT_READ_TRACE_OBJECT_ARRAY_NUMBER_OF_BYTES				0x00, 0x22
#define UT_READ_TRACE_OBJECT_ARRAY_TRACE_DATA					0x04, 0xAA, 0xBB, 0xCC, 0xDD
#define UT_READ_TRACE_OBJECT_ARRAY_LINE_NUMBER					1011
#define UT_READ_TRACE_OBJECT_ARRAY_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','a','r','r','a','y','.','t','x','t'
#define UT_READ_TRACE_OBJECT_ARRAY_DATA_ARRAY					TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_ARRAY_NUMBER_OF_BYTES,				\
										TRACER_TRACE_TYPE_RAW_ARRAY,						\
										UT_READ_TRACE_OBJECT_ARRAY_TRACE_DATA,					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_ARRAY_LINE_NUMBER),\
										UT_READ_TRACE_OBJECT_ARRAY_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY

#define UT_READ_TRACE_OBJECT_INCOMPLETE_NUMBER_OF_BYTES				0x00, 0x1E
#define UT_READ_TRACE_OBJECT_INCOMPLETE_TRACE_DATA				0xCC, 0xDD
#define UT_READ_TRACE_OBJECT_INCOMPLETE_LINE_NUMBER				456
//#define UT_READ_TRACE_OBJECT_INCOMPLETE_FILE_NAME				't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','w','o','r','d','.','t','x','t'
#define UT_READ_TRACE_OBJECT_INCOMPLETE_DATA_ARRAY				TRACE_PARSER_HEADER_BYTE_ARRAY,						\
										UT_READ_TRACE_OBJECT_INCOMPLETE_NUMBER_OF_BYTES,			\
										TRACER_TRACE_TYPE_RAW_WORD,						\
										UT_READ_TRACE_OBJECT_INCOMPLETE_TRACE_DATA,				\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_INCOMPLETE_LINE_NUMBER)

#define UT_READ_TRACE_OBJECT_OVERFLOW_NUMBER_OF_BYTES				0x00, 0xFF
#define UT_READ_TRACE_OBJECT_OVERFLOW_TRACE_DATA				0xFF,										\
										0xAA,0xAA,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xBB,0xBB
#define UT_READ_TRACE_OBJECT_OVERFLOW_REFERENCE_DATA				0xFF,										\
										0xAA,0xAA,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,\
										0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD,0xDD
#define UT_READ_TRACE_OBJECT_OVERFLOW_LINE_NUMBER				1112
#define UT_READ_TRACE_OBJECT_OVERFLOW_FILE_NAME					't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','o','v','e','r','f','l','o','w','.','t','x','t'
#define UT_READ_TRACE_OBJECT_OVERFLOW_DATA_ARRAY				TRACE_PARSER_HEADER_BYTE_ARRAY,							\
										UT_READ_TRACE_OBJECT_OVERFLOW_NUMBER_OF_BYTES,					\
										TRACER_TRACE_TYPE_RAW_ARRAY,							\
										UT_READ_TRACE_OBJECT_OVERFLOW_TRACE_DATA,					\
										UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_READ_TRACE_OBJECT_INCOMPLETE_LINE_NUMBER),	\
										UT_READ_TRACE_OBJECT_OVERFLOW_FILE_NAME,					\
										TRACE_PARSER_FOOTER_BYTE_ARRAY


#define UT_READ_TRACE_OBJECT_INCORRECT_DATA_ARRAY				0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, \
										0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, \
										0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, \
										0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04


// --------------------------------------------------------------------------------

// counter

static u8 counter_FILE_SET_PATH = 0;
static u8 counter_USART0_BYTES_AVAILABLE = 0;
static u8 counter_USART0_INITIALIZE = 0;
static u8 counter_USART0_CONFIGURE = 0;

static u16 counter_BYTES_AVAILABLE = 0;
static u16 counter_BYTES_READ = 0;

static u8 array_DATA_BUFFER[UT_DATA_BUFFER_MAX_LENGTH];

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

	counter_FILE_SET_PATH = 0;
	counter_USART0_BYTES_AVAILABLE = 0;
	counter_USART0_INITIALIZE = 0;
	counter_USART0_CONFIGURE = 0;
	counter_BYTES_AVAILABLE = 0;
	counter_BYTES_READ = 0;
	memset(array_DATA_BUFFER, 0x00, UT_DATA_BUFFER_MAX_LENGTH);
}

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(RAW_TRACE_OBJECT_QEUE, TRACE_OBJECT_RAW, sizeof(TRACE_OBJECT_RAW), TRACER_RAW_TRACE_OBJECT_QEUE_SIZE)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

// stubs

void usart0_driver_initialize(void) {
	DEBUG_PASS("usart0_driver_initialize");
	counter_USART0_INITIALIZE += 1;
}

/*!
 *
 */
void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {
	DEBUG_PASS("usart0_driver_configure()");
	counter_USART0_CONFIGURE += 1;
}

/*!
 *
 */
void usart0_driver_power_off(void) {
	DEBUG_PASS("usart0_driver_power_off");
}

u16 usart0_driver_bytes_available(void) {

	//DEBUG_TRACE_byte(counter_BYTES_AVAILABLE, "usart0_driver_bytes_available");

	if (counter_USART0_BYTES_AVAILABLE < 255) {
		counter_USART0_BYTES_AVAILABLE += 1;
	}

	switch (UT_GET_TEST_CASE_ID()) {

		case TEST_CASE_ID_TRACE_INCORRECT_NUMBER_OF_BYTES_TOO_LOW :
		case TEST_CASE_ID_TRACE_INCORRECT :
		case TEST_CASE_ID_TRACE_INCOMPLETE :
		case TEST_CASE_ID_TRACE_ARRAY :
		case TEST_CASE_ID_TRACE_LONG : 
		case TEST_CASE_ID_TRACE_WORD :
		case TEST_CASE_ID_TRACE_BYTE :
		case TEST_CASE_ID_TRACE_PASS :

			return counter_BYTES_AVAILABLE;
	}

	return 0;
}

u16 usart0_driver_get_N_bytes(u16 num_bytes, u8* p_buffer_to) {

	(void)num_bytes;
	(void)p_buffer_to;

	switch (UT_GET_TEST_CASE_ID()) {

		case TEST_CASE_ID_TRACE_INCORRECT_NUMBER_OF_BYTES_TOO_LOW :
		case TEST_CASE_ID_TRACE_INCORRECT :
		case TEST_CASE_ID_TRACE_INCOMPLETE :
		case TEST_CASE_ID_TRACE_ARRAY :
		case TEST_CASE_ID_TRACE_LONG :
		case TEST_CASE_ID_TRACE_WORD :
		case TEST_CASE_ID_TRACE_BYTE :
		case TEST_CASE_ID_TRACE_PASS :

			if (num_bytes > counter_BYTES_AVAILABLE) {
				num_bytes = (u8)counter_BYTES_AVAILABLE;
			}
			
			memcpy(p_buffer_to, array_DATA_BUFFER + counter_BYTES_READ, num_bytes);
			counter_BYTES_AVAILABLE -= num_bytes;
			counter_BYTES_READ += num_bytes;
	
			//DEBUG_TRACE_N(num_bytes, p_buffer_to, "usart0_driver_get_N_bytes()");

			return num_bytes;
	}

	return 0;
}


u8 usart0_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {
	DEBUG_PASS("usart0_driver_set_N_bytes");

	(void)num_bytes;
	(void)p_buffer_from;

	return 0;
}

u8 usart0_driver_is_ready_for_tx(void) {
	DEBUG_PASS("usart0_driver_is_ready_for_tx");
	return 0;
}

u8 usart0_driver_is_ready_for_rx(void) {
	DEBUG_PASS("usart0_driver_is_ready_for_rx");
	return 0;
}


void usart0_driver_start_rx(u16 num_of_rx_bytes) {
	DEBUG_PASS("usart0_driver_start_rx");
	(void) num_of_rx_bytes;
}

void usart0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {
	DEBUG_PASS("usart0_driver_wait_for_rx");
	(void) num_bytes;
	(void) timeout_ms;

}

void usart0_driver_stop_rx(void) {
	DEBUG_PASS("usart0_driver_stop_rx");

}

void usart0_driver_start_tx(void) {
	DEBUG_PASS("usart0_driver_start_tx");

}

void usart0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
	DEBUG_PASS("usart0_driver_wait_for_tx");
	(void) num_bytes;
	(void) timeout_ms;

}

void usart0_driver_stop_tx(void) {
	DEBUG_PASS("usart0_driver_stop_tx");

}

void usart0_driver_clear_rx_buffer(void) {
	DEBUG_PASS("usart0_driver_clear_rx_buffer");

}


void usart0_driver_clear_tx_buffer(void) {
	DEBUG_PASS("usart0_driver_clear_tx_buffer");

}


void usart0_driver_set_address (u8 addr) {
	DEBUG_PASS("usart0_driver_set_address");

}

u8 usart0_driver_mutex_request(void) {
	DEBUG_PASS("usart0_driver_mutex_request");

	return MUTEX_INVALID_ID;
}

void usart0_driver_mutex_release(u8 m_id) {
	DEBUG_PASS("usart0_driver_mutex_release");
	(void) m_id;
}


// --------------------------------------------------------------------------------

static void TEST_CASE_init(void) {

	UT_START_TEST_CASE("Initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INIT);
		unittest_reset_counter();

		READ_TRACE_OBJECT_THREAD_start();
		UNITTEST_WAIT_MSEC(1000);

		UT_CHECK_IS_EQUAL(counter_USART0_CONFIGURE, 1);
		UT_CHECK_IS_EQUAL(counter_USART0_INITIALIZE, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_pass(void) {

	UT_START_TEST_CASE("Read Trace PASS")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_PASS);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_PASS_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_pass()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 33);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_byte(void) {

	UT_START_TEST_CASE("Read Trace BYTE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_BYTE);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_BYTE_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_byte()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 34);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_word(void) {

	UT_START_TEST_CASE("Read Trace WORD")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_WORD);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_WORD_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_word()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 35);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_long(void) {

	UT_START_TEST_CASE("Read Trace LONG")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_LONG);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_LONG_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_long()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 37);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_array(void) {

	UT_START_TEST_CASE("Read Trace ARRAY")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_ARRAY);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_ARRAY_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_array()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 39);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_word_wrong_data_in_front(void) {

	UT_START_TEST_CASE("Read Trace WORD - wrong data in front")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_WORD);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_WORD_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		u8 i = 0;
		for ( ; i < 10; i++) {
			array_DATA_BUFFER[i] = i;
		}

		memcpy(array_DATA_BUFFER + i, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes + i;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_word()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 35);
		UT_COMPARE_ARRAY(raw_trace_obj.data, ut_trace_data_array, sizeof(ut_trace_data_array));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_word_incomplete_data(void) {

	UT_START_TEST_CASE("Read Trace WORD - incomplete data")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_INCOMPLETE);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_INCOMPLETE_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_word_incomplete_data()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		u8 is_empty = RAW_TRACE_OBJECT_QEUE_is_empty();
		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		u8 zero_data[255];
		memset(zero_data, 0x00, sizeof(zero_data));

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(is_empty, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 0);
		UT_COMPARE_ARRAY(raw_trace_obj.data, zero_data, sizeof(zero_data));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_incorrect_byte_stram(void) {

	UT_START_TEST_CASE("Read Trace - incorrect byte stream")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_INCORRECT);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_INCORRECT_DATA_ARRAY};
		u8 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, ut_trace_data_array, "TEST_CASE_read_trace_incorrect_byte_stram()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		u8 is_empty = RAW_TRACE_OBJECT_QEUE_is_empty();
		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		u8 zero_data[255];
		memset(zero_data, 0x00, sizeof(zero_data));

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(is_empty, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, 0);
		UT_COMPARE_ARRAY(raw_trace_obj.data, zero_data, sizeof(zero_data));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_trace_incorrect_number_of_bytes_too_low(void) {

	UT_START_TEST_CASE("Read Trace - Incorrect number of bytes - too low")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_INCORRECT_NUMBER_OF_BYTES_TOO_LOW);
		unittest_reset_counter();
		
		UNITTEST_WAIT_MSEC(250);

		u8 ut_trace_data_array[] = {UT_READ_TRACE_OBJECT_OVERFLOW_DATA_ARRAY};
		u16 num_bytes = sizeof(ut_trace_data_array);

		memcpy(array_DATA_BUFFER, ut_trace_data_array, num_bytes);
		counter_BYTES_AVAILABLE = num_bytes;

		DEBUG_TRACE_word(counter_BYTES_AVAILABLE, "TEST_CASE_read_trace_incorrect_number_of_bytes_too_low()");
		DEBUG_TRACE_N(counter_BYTES_AVAILABLE, array_DATA_BUFFER, "TEST_CASE_read_trace_incorrect_number_of_bytes_too_low()");

		TRACE_OBJECT_RAW raw_trace_obj;
		memset((u8*)&raw_trace_obj, 0x00, sizeof(TRACE_OBJECT_RAW));

		UNITTEST_WAIT_MSEC(1000);

		u8 mutex_get = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (RAW_TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		u8 is_empty_first = RAW_TRACE_OBJECT_QEUE_is_empty();
		RAW_TRACE_OBJECT_QEUE_deqeue(&raw_trace_obj);
		u8 is_empty_second = RAW_TRACE_OBJECT_QEUE_is_empty();
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		u8 reference_data[] = {
			TRACE_PARSER_HEADER_BYTE_ARRAY,		
			UT_READ_TRACE_OBJECT_OVERFLOW_NUMBER_OF_BYTES,
			TRACER_TRACE_TYPE_RAW_ARRAY,
			UT_READ_TRACE_OBJECT_OVERFLOW_REFERENCE_DATA
		};

		UT_CHECK_IS_EQUAL(mutex_get, 1);
		UT_CHECK_IS_EQUAL(is_empty_first, 0);
		UT_CHECK_IS_EQUAL(is_empty_second, 1);
		UT_CHECK_IS_EQUAL(raw_trace_obj.length, sizeof(reference_data));
		UT_COMPARE_ARRAY(raw_trace_obj.data, reference_data, sizeof(reference_data));
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_terminate(void) {

	UT_START_TEST_CASE("Terminate")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TERMINATE);

		UNITTEST_WAIT_MSEC(1000);
		READ_TRACE_OBJECT_THREAD_terminate();
		UNITTEST_WAIT_MSEC(1000);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for parse trace object 1.0")
	{
		TEST_CASE_init();

		TEST_CASE_read_trace_pass();
		TEST_CASE_read_trace_byte();
		TEST_CASE_read_trace_word();
		TEST_CASE_read_trace_long();
		TEST_CASE_read_trace_array();

		TEST_CASE_read_trace_word_wrong_data_in_front();
		TEST_CASE_read_trace_word_incomplete_data();
		TEST_CASE_read_trace_pass();
		TEST_CASE_read_trace_incorrect_byte_stram();
		TEST_CASE_read_trace_incorrect_number_of_bytes_too_low();
		
		TEST_CASE_terminate();

		// TST-CASE FILE-NAME TOO LONG
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
