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

#include "common/math_module.h"
#include "common/qeue_interface.h"
#include "common/common_tools_string.h"
#include "common/signal_slot_interface.h"

#include "ui/file_interface/file_interface.h"
#include "time_management/time_management.h"

#include "app_tasks/thread_parse_trace_object.h"
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

// --------------------------------------------------------------------------------

#define TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE					10
#define TRACER_RAW_TRACE_OBJECT_QEUE_SIZE					10

#define CONSOLE_OUTPUT_MAX_LINE_LEGNTH						2048

// --------------------------------------------------------------------------------

#define UNITTEST_GET_ARRAY_FROM_WORD_MSB(word)					(u8)(word >> 8), (u8)(word)

// --------------------------------------------------------------------------------

#define UNITTEST_WAIT_MSEC(milli_sec)						UNITTEST_TIMER_start();				\
										while (UNITTEST_TIMER_is_up(milli_sec) == 0) {	\
											mcu_task_controller_schedule();		\
										}						\
										UNITTEST_TIMER_stop()

// --------------------------------------------------------------------------------

#define UT_TEST_CASE_PARSE_TRACE_PASS_FILE_NAME_CHAR_ARRAY		't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','p','a','s','s','.','t','x','t'
#define UT_TEST_CASE_PARSE_TRACE_PASS_FILE_NAME_STRING			"test_case_trace_pass.txt"

#define UT_TEST_CASE_PARSE_TRACE_BYTE_FILE_NAME_CHAR_ARRAY		't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','b','y','t','e','.','t','x','t'
#define UT_TEST_CASE_PARSE_TRACE_BYTE_FILE_NAME_STRING			"test_case_trace_byte.txt"
#define UT_TEST_CASE_PARSE_TRACE_BYTE_LINE_NUMBER			123
#define UT_TEST_CASE_PARSE_TRACE_BYTE_TRACE_DATA			0xAA

#define UT_TEST_CASE_PARSE_TRACE_WORD_FILE_NAME_CHAR_ARRAY		't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','w','o','r','d','.','t','x','t'
#define UT_TEST_CASE_PARSE_TRACE_WORD_FILE_NAME_STRING			"test_case_trace_word.txt"
#define UT_TEST_CASE_PARSE_TRACE_WORD_LINE_NUMBER			999
#define UT_TEST_CASE_PARSE_TRACE_WORD_TRACE_DATA			0x01, 0x04
#define UT_TEST_CASE_PARSE_TRACE_WORD_TRACE_DATA_VALUE			0x0401

#define UT_TEST_CASE_PARSE_TRACE_LONG_FILE_NAME_CHAR_ARRAY		't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','l','o','n','g','.','t','x','t'
#define UT_TEST_CASE_PARSE_TRACE_LONG_FILE_NAME_STRING			"test_case_trace_long.txt"
#define UT_TEST_CASE_PARSE_TRACE_LONG_LINE_NUMBER			1234
#define UT_TEST_CASE_PARSE_TRACE_LONG_TRACE_DATA			0x01, 0x04, 0x07, 0x0A 
#define UT_TEST_CASE_PARSE_TRACE_LONG_TRACE_DATA_VALUE			0x00A070401

#define UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_CHAR_ARRAY		't','e','s','t','_','c','a','s','e','_','t','r','a','c','e','_','a','r','r','a','y','.','t','x','t'
#define UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_STRING			"test_case_trace_array.txt"
#define UT_TEST_CASE_PARSE_TRACE_ARRAY_LINE_NUMBER			4321
#define UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA			0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02,0x01, 0x02

#define UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA_MAX_LENGTH		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,	\
									0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,	\
									0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,	\
									0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,	\
									0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,	\
									0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,	\
									0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,	\
									0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,	\
									0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,	\
									0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,	\
									0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,	\
									0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,	\
									0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,	\
									0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,	\
									0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,	\
									0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe

// --------------------------------------------------------------------------------

// counter

u8 counter_FILE_SET_PATH = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

	counter_FILE_SET_PATH = 0;
}

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(RAW_TRACE_OBJECT_QEUE, TRACE_OBJECT_RAW, sizeof(TRACE_OBJECT_RAW), TRACER_RAW_TRACE_OBJECT_QEUE_SIZE)
QEUE_INTERFACE_BUILD_QEUE(TRACE_OBJECT_QEUE, TRACE_OBJECT, sizeof(TRACE_OBJECT), TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

static void TEST_CASE_init(void) {

	UT_START_TEST_CASE("Initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INIT);
		unittest_reset_counter();

		PARSE_TRACE_OBJECT_THREAD_start();
		UNITTEST_WAIT_MSEC(50);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_pass(void) {

	UT_START_TEST_CASE("Parse Trace PASS")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_PASS);
		unittest_reset_counter();


		u16 length = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_PASS_FILE_NAME_STRING);
		u16 object_length = length + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(length), // length 2 byte
				TRACER_TRACE_TYPE_RAW_PASS, // type 1 byte
				// trace data
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(111),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_PASS_FILE_NAME_CHAR_ARRAY, //"file-name", // file name
				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		TRACE_OBJECT_QEUE_deqeue(&trace_obj);
		TRACE_OBJECT_QEUE_mutex_release();

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_PASS);
		UT_CHECK_IS_EQUAL(trace_obj.length, length);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, 111);
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_PASS_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_byte(void) {

	UT_START_TEST_CASE("Parse Trace Byte")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_BYTE);
		unittest_reset_counter();


		u16 length = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_BYTE_FILE_NAME_STRING);
		u16 object_length = length + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER + 1;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(length), // length 2 byte
				TRACER_TRACE_TYPE_RAW_BYTE, // type 1 byte
				UT_TEST_CASE_PARSE_TRACE_BYTE_TRACE_DATA, // trace data
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_TEST_CASE_PARSE_TRACE_BYTE_LINE_NUMBER),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_BYTE_FILE_NAME_CHAR_ARRAY, //"file-name", // file name
				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		if (mutex_get == 1) {
			RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
			RAW_TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_byte() - RAW_TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		if (mutex_get == 2) {
			TRACE_OBJECT_QEUE_deqeue(&trace_obj);
			TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_byte() - TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_BYTE);
		UT_CHECK_IS_EQUAL(trace_obj.length, length);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, UT_TEST_CASE_PARSE_TRACE_BYTE_LINE_NUMBER);
		UT_CHECK_IS_EQUAL(trace_obj.data.byte, UT_TEST_CASE_PARSE_TRACE_BYTE_TRACE_DATA);
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_BYTE_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_word(void) {

	UT_START_TEST_CASE("Parse Trace Word")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_WORD);
		unittest_reset_counter();


		u16 length = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_WORD_FILE_NAME_STRING);
		u16 object_length = length + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER + 2;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(length), // length 2 byte
				TRACER_TRACE_TYPE_RAW_WORD, // type 1 byte
				UT_TEST_CASE_PARSE_TRACE_WORD_TRACE_DATA, // trace data
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_TEST_CASE_PARSE_TRACE_WORD_LINE_NUMBER),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_WORD_FILE_NAME_CHAR_ARRAY, //"file-name", // file name
				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		if (mutex_get == 1) {
			RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
			RAW_TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_word() - RAW_TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		if (mutex_get == 2) {
			TRACE_OBJECT_QEUE_deqeue(&trace_obj);
			TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_word() - TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_WORD);
		UT_CHECK_IS_EQUAL(trace_obj.length, length);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, UT_TEST_CASE_PARSE_TRACE_WORD_LINE_NUMBER);
		UT_CHECK_IS_EQUAL(trace_obj.data.word, UT_TEST_CASE_PARSE_TRACE_WORD_TRACE_DATA_VALUE);
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_WORD_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_long(void) {

	UT_START_TEST_CASE("Parse Trace Long")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_LONG);
		unittest_reset_counter();


		u16 length = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_LONG_FILE_NAME_STRING);
		u16 object_length = length + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER + 4;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(length), // length 2 byte
				TRACER_TRACE_TYPE_RAW_LONG, // type 1 byte
				UT_TEST_CASE_PARSE_TRACE_LONG_TRACE_DATA, // trace data
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_TEST_CASE_PARSE_TRACE_LONG_LINE_NUMBER),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_LONG_FILE_NAME_CHAR_ARRAY, //"file-name", // file name
				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		if (mutex_get == 1) {
			RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
			RAW_TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_long() - RAW_TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		if (mutex_get == 2) {
			TRACE_OBJECT_QEUE_deqeue(&trace_obj);
			TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_long() - TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_LONG);
		UT_CHECK_IS_EQUAL(trace_obj.length, length);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, UT_TEST_CASE_PARSE_TRACE_LONG_LINE_NUMBER);
		UT_CHECK_IS_EQUAL(trace_obj.data.integer, UT_TEST_CASE_PARSE_TRACE_LONG_TRACE_DATA_VALUE);
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_LONG_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_array(void) {

	UT_START_TEST_CASE("Parse Trace Array")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_ARRAY);
		unittest_reset_counter();

		u8 ut_test_case_trace_array_value[] = {UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA};

		u16 byte_count = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ 1 // number of bytes of trace-data
				+ sizeof(ut_test_case_trace_array_value) // length of trace-data
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_STRING);


		u16 object_length = byte_count + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				
				// HEADER
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(byte_count), // length 2 byte
				TRACER_TRACE_TYPE_RAW_ARRAY, // type 1 byte

				// TRACE DATA
				sizeof(ut_test_case_trace_array_value), // length of trace-data
				UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA, // trace data

				// SOURCE FILE
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_TEST_CASE_PARSE_TRACE_ARRAY_LINE_NUMBER),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_CHAR_ARRAY, //"file-name", // file name

				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		if (mutex_get == 1) {
			RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
			RAW_TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_array() - RAW_TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		if (mutex_get == 2) {
			TRACE_OBJECT_QEUE_deqeue(&trace_obj);
			TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_array() - TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_ARRAY);
		UT_CHECK_IS_EQUAL(trace_obj.length, byte_count);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, UT_TEST_CASE_PARSE_TRACE_ARRAY_LINE_NUMBER);
		UT_COMPARE_ARRAY(trace_obj.data.array, ut_test_case_trace_array_value, sizeof(ut_test_case_trace_array_value));
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_parse_trace_array_max_length(void) {

	UT_START_TEST_CASE("Parse Trace Array (max length)")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRACE_ARRAY_MAX_LENGTH);
		unittest_reset_counter();

		u8 ut_test_case_trace_array_value[] = {UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA_MAX_LENGTH};

		u16 byte_count = TRACE_PARSER_NUM_BYTES_TRACE_TYPE
				+ TRACE_PARSER_NUM_BYTES_LINE_NUMBER
				+ TRACER_PARSER_NUM_BYTES_FOOTER
				+ 1 // number of bytes of trace-data
				+ sizeof(ut_test_case_trace_array_value) // length of trace-data
				+ common_tools_string_length(UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_STRING);


		u16 object_length = byte_count + TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER;

		TRACE_OBJECT_RAW raw_trace_obj = {
			.data = {
				
				// HEADER
				TRACE_PARSER_HEADER_BYTE_ARRAY,
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(byte_count), // length 2 byte
				TRACER_TRACE_TYPE_RAW_ARRAY, // type 1 byte

				// TRACE DATA
				sizeof(ut_test_case_trace_array_value), // length of trace-data
				UT_TEST_CASE_PARSE_TRACE_ARRAY_TRACE_DATA_MAX_LENGTH, // trace data

				// SOURCE FILE
				UNITTEST_GET_ARRAY_FROM_WORD_MSB(UT_TEST_CASE_PARSE_TRACE_ARRAY_LINE_NUMBER),// line number 2 byte
				UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_CHAR_ARRAY, //"file-name", // file name

				0xFE // footer
				},
			.length = object_length
		};

		TRACE_OBJECT trace_obj;
		memset((u8*)&trace_obj, 0x00, sizeof(TRACE_OBJECT));

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

		if (mutex_get == 1) {
			RAW_TRACE_OBJECT_QEUE_enqeue(&raw_trace_obj);
			RAW_TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_array() - RAW_TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UNITTEST_WAIT_MSEC(1000);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_get += 1;
				break;
			}

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		if (mutex_get == 2) {
			TRACE_OBJECT_QEUE_deqeue(&trace_obj);
			TRACE_OBJECT_QEUE_mutex_release();
		} else {
			DEBUG_PASS("TEST_CASE_parse_trace_array() - TRACE_OBJECT_QEUE_mutex_get() has FAILED !!! ---");
		}

		UT_CHECK_IS_EQUAL(mutex_get, 2);
		UT_CHECK_IS_EQUAL(trace_obj.type, TRACE_OBJECT_TYPE_ARRAY);
		UT_CHECK_IS_EQUAL(trace_obj.length, byte_count);
		UT_CHECK_IS_EQUAL(trace_obj.line_number, UT_TEST_CASE_PARSE_TRACE_ARRAY_LINE_NUMBER);
		UT_COMPARE_ARRAY(trace_obj.data.array, ut_test_case_trace_array_value, sizeof(ut_test_case_trace_array_value));
		UT_COMPARE_STRING(trace_obj.file_name, UT_TEST_CASE_PARSE_TRACE_ARRAY_FILE_NAME_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_terminate(void) {

	UT_START_TEST_CASE("Terminate")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TERMINATE);

		UNITTEST_WAIT_MSEC(1000);
		PARSE_TRACE_OBJECT_THREAD_terminate();
		UNITTEST_WAIT_MSEC(1000);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for parse trace object 1.0")
	{
		TEST_CASE_init();
		TEST_CASE_parse_trace_pass();
		TEST_CASE_parse_trace_byte();
		TEST_CASE_parse_trace_word();
		TEST_CASE_parse_trace_long();
		TEST_CASE_parse_trace_array();
		TEST_CASE_parse_trace_array_max_length();
		TEST_CASE_terminate();

		// TST-CASE FILE-NAME TOO LONG

	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
