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

#include "app_tasks/thread_print_trace_object.h"
#include "mcu_task_management/mcu_task_controller.h"
#include "tracer/trace_object.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INIT							1
#define TEST_CASE_ID_TERMINATE							2
#define TEST_CASE_ID_PRINTE_TRACE_PASS						3
#define TEST_CASE_ID_PRINTE_TRACE_BYTE						4
#define TEST_CASE_ID_PRINTE_TRACE_WORD						5
#define TEST_CASE_ID_PRINTE_TRACE_LONG						6
#define TEST_CASE_ID_PRINTE_TRACE_ARRAY						7
#define TEST_CASE_ID_SOURCE_FILE_NOT_FOUND					8
#define TEST_CASE_ID_SOURCE_LINE_NOT_FOUND					9
#define TEST_CASE_ID_ORDER_OF_PRINT_TRACES					10

// --------------------------------------------------------------------------------

#define TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE					10
#define CONSOLE_OUTPUT_MAX_LINE_LEGNTH						2048

// --------------------------------------------------------------------------------

#define UNITTEST_TEST_CASE_PRINT_TRACE_PASS_FILE_NAME				"unittest_print_trace_pass.txt"
#define UNITTSET_TEST_CASE_PRINT_TRACE_PASS_LINE_NUMBER				14
#define UNITTSET_TEST_CASE_PRINT_TRACE_PASS_FILE_LINE				"DEBUG_PASS(\"TEST_CASE_PRINT_TRACE_PASS()\")"
#define UNITTEST_TEST_CASE_PRINT_TRACE_PASS_CONSOLE_STRING			"unittest_print_trace_pass.txt:14 - TEST_CASE_PRINT_TRACE_PASS()"

#define UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_FILE_NAME				"unittest_print_trace_byte.txt"
#define UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_LINE_NUMBER				365
#define UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_FILE_LINE				"DEBUG_TRACE_byte(\"TEST_CASE_PRINT_TRACE_BYTE() - The byte is in the next line\")"
#define UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_CONSOLE_STRING			"unittest_print_trace_byte.txt:365 - TEST_CASE_PRINT_TRACE_BYTE() - The byte is in the next line\n\t- Data: 170 (AA)"
#define UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_TRACE_DATA				0xAA

#define UNITTEST_TEST_CASE_PRINT_TRACE_WORD_FILE_NAME				"unittest_print_trace_word.txt"
#define UNITTSET_TEST_CASE_PRINT_TRACE_WORD_LINE_NUMBER				4324
#define UNITTSET_TEST_CASE_PRINT_TRACE_WORD_FILE_LINE				"DEBUG_TRACE_byte(\"TEST_CASE_PRINT_TRACE_WORD() - The word is in the next line\")"
#define UNITTEST_TEST_CASE_PRINT_TRACE_WORD_CONSOLE_STRING			"unittest_print_trace_word.txt:4324 - TEST_CASE_PRINT_TRACE_WORD() - The word is in the next line\n\t- Data: 43981 (ABCD)"
#define UNITTEST_TEST_CASE_PRINT_TRACE_WORD_TRACE_DATA				0xABCD

#define UNITTEST_TEST_CASE_PRINT_TRACE_LONG_FILE_NAME				"unittest_print_trace_long.txt"
#define UNITTSET_TEST_CASE_PRINT_TRACE_LONG_LINE_NUMBER				56123
#define UNITTSET_TEST_CASE_PRINT_TRACE_LONG_FILE_LINE				"DEBUG_TRACE_byte(\"TEST_CASE_PRINT_TRACE_LONG() - The Long is in the next line\")"
#define UNITTEST_TEST_CASE_PRINT_TRACE_LONG_CONSOLE_STRING			"unittest_print_trace_long.txt:56123 - TEST_CASE_PRINT_TRACE_LONG() - The Long is in the next line\n\t- Data: 296472047 (11ABCDEF)"
#define UNITTEST_TEST_CASE_PRINT_TRACE_LONG_TRACE_DATA				0x11ABCDEF

#define UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_FILE_NAME				"unittest_print_trace_array.txt"
#define UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_LINE_NUMBER			1
#define UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_FILE_LINE				"DEBUG_TRACE_byte(\"TEST_CASE_PRINT_TRACE_ARRAY() - The Array is in the next line\")"
#define UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_CONSOLE_STRING			"unittest_print_trace_array.txt:1 - TEST_CASE_PRINT_TRACE_ARRAY() - The Array is in the next line\n 0000 |  01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 11  | ................\n 0010 |  12 13 14 15 16 17 18 19 1A __ __ __ __ __ __ __  | ........."
#define UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_TRACE_DATA				0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A

#define UNITTEST_TEST_CASE_FILE_NOT_FOUND_FILE_NAME				"unittest_print_trace_file_not_found.txt"
#define UNITTSET_TEST_CASE_FILE_NOT_FOUND_LINE_NUMBER				99
#define UNITTSET_TEST_CASE_FILE_NOT_FOUND_FILE_LINE				"DEBUG_PASS(\"TEST_CASE_PRINT_TRACE_FILE_NOT_FOUND()\")"
#define UNITTEST_TEST_CASE_FILE_NOT_FOUND_CONSOLE_STRING			"FILE NOT FOUND ! - unittest/source_base_path/unittest_print_trace_file_not_found.txt"

#define UNITTEST_TEST_CASE_LINE_NOT_FOUND_FILE_NAME				"unittest_print_trace_line_not_found.txt"
#define UNITTSET_TEST_CASE_LINE_NOT_FOUND_LINE_NUMBER				101
#define UNITTSET_TEST_CASE_LINE_NOT_FOUND_FILE_LINE				"DEBUG_PASS(\"TEST_CASE_PRINT_TRACE_LINE_NOT_FOUND()\")"
#define UNITTEST_TEST_CASE_LINE_NOT_FOUND_CONSOLE_STRING			"LINE NOT FOUND ! - unittest/source_base_path/unittest_print_trace_line_not_found.txt"

// --------------------------------------------------------------------------------

// counter

u8 counter_FILE_SET_PATH = 0;
u8 counter_FILE_HAS_CHANGED = 0;
u8 counter_FILE_IS_READABLE = 0;
u8 counter_FILE_IS_EXISTING = 0;
u8 counter_FILE_OPEN = 0;
u8 counter_FILE_CLOSE = 0;
u8 counter_FILE_READ_NEXT_LINE = 0;
u8 counter_FILE_GET_SIZE = 0;
u8 counter_FILE_DELETE = 0;
u8 counter_FILE_RENAME = 0;
u8 counter_FILE_CREATE = 0;
u8 counter_FILE_APPEND_LINE = 0;
u8 counter_FILE_READ_SPECIFIC_LINE = 0;
u8 counter_CONSOLE_WRITE_LINE = 0;


static char console_out_put_string_list[TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE][CONSOLE_OUTPUT_MAX_LINE_LEGNTH];

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {
	counter_FILE_SET_PATH = 0;
	counter_FILE_HAS_CHANGED = 0;
	counter_FILE_IS_READABLE = 0;
	counter_FILE_IS_EXISTING = 0;
	counter_FILE_OPEN = 0;
	counter_FILE_CLOSE = 0;
	counter_FILE_READ_NEXT_LINE = 0;
	counter_FILE_GET_SIZE = 0;
	counter_FILE_DELETE = 0;
	counter_FILE_RENAME = 0;
	counter_FILE_CREATE = 0;
	counter_FILE_APPEND_LINE = 0;
	counter_CONSOLE_WRITE_LINE = 0;
	counter_FILE_READ_SPECIFIC_LINE = 0;

	u8 i = 0;
	for ( ; i < TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE; i++) {
		common_tools_string_clear(console_out_put_string_list[i], CONSOLE_OUTPUT_MAX_LINE_LEGNTH);
	}
}

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(TRACE_OBJECT_QEUE, TRACE_OBJECT, sizeof(TRACE_OBJECT), TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_ARGUMENT_FILE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_ARGUMENT_PATH_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONSOLE_ACTIVATED_SIGNAL)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

// stubs

void file_set_path(FILE_INTERFACE* p_file, const char* path) {

	DEBUG_TRACE_STR(path, "file_set_path()");
	counter_FILE_SET_PATH += 1;

	memset(p_file->path, '\0', FILE_PATH_MAX_STRING_LENGTH);
	memcpy(p_file->path, path, strlen(path));
}

u8 file_has_changed(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_has_changed()");
	counter_FILE_HAS_CHANGED += 1;
	return 0;
}

u8 file_is_readable(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_is_readable()");
	counter_FILE_IS_READABLE += 1;
	return 1;
}

u8 file_is_existing(FILE_INTERFACE* p_file) {

	DEBUG_TRACE_STR(p_file->path, "file_is_existing()");
	counter_FILE_IS_EXISTING += 1;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_SOURCE_FILE_NOT_FOUND:
			return 0;
	}

	//if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED) {
	//	if (counter_FILE_IS_EXISTING < 4) {
	//		return 1;
	//	}
	//	return 0;
	//}

	//if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_QEUE_OVERFLOW) {
	//	return 1;
	//}

	return 1;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
	return counter_FILE_OPEN != 0;
}

u32 file_get_size(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_get_size()");
	counter_FILE_GET_SIZE += 1;

	//if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_FILE_SIZE_EXCEEDED) {
	//	return LOG_INTERFACE_MAX_LOG_FILE_SIZE_KB + 10;
	//}

	//if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_LOG_QEUE_OVERFLOW) {
	//	return 512;
	//}

	return 0;
}

u8 file_delete(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_delete()");
	counter_FILE_DELETE += 1;
	return 0;
}

u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path) {

	DEBUG_PASS("file_rename()");
	counter_FILE_RENAME += 1;
	return 0;
}

const char* file_get_path(FILE_INTERFACE* p_file) {
	return (const char*)p_file->path;
}

u8 file_create(FILE_INTERFACE* p_file) {

	DEBUG_TRACE_STR(p_file->path, "file_create()");
	counter_FILE_CREATE += 1;
	return 1;
}

u8 file_open(FILE_INTERFACE* p_file) {

	counter_FILE_OPEN += 1;
	DEBUG_TRACE_STR(p_file->path, "file_open()");

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_SOURCE_FILE_NOT_FOUND:
			return 0;
	}

	//if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_ENTER_SLEEP_IF_ACCES_NOT_PERMITTED) {
	//	return 0;
	//}

	return 1;
}

void file_close(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_close()");
	counter_FILE_CLOSE += 1;
}

i16 file_read_line(FILE* file_handle, char* line_to, u16 num_max_bytes) {

	DEBUG_PASS("file_read_line()");
	return 0;
}

i16 file_read_next_line(FILE_INTERFACE* p_file, char* line_to, u16 max_line_length) {

	DEBUG_TRACE_STR(p_file->path, "file_read_next_line()");
	return -1;
}

u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length) {
	DEBUG_TRACE_word(line_number, "file_read_specific_line()");
	counter_FILE_READ_SPECIFIC_LINE += 1;

	switch (UT_GET_TEST_CASE_ID()) {

		case TEST_CASE_ID_PRINTE_TRACE_PASS:
			common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_PASS_FILE_LINE, max_length);
			return common_tools_string_length(next_line);

		case TEST_CASE_ID_PRINTE_TRACE_BYTE:
			common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_FILE_LINE, max_length);
			return common_tools_string_length(next_line);

		case TEST_CASE_ID_PRINTE_TRACE_WORD:
			common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_WORD_FILE_LINE, max_length);
			return common_tools_string_length(next_line);

		case TEST_CASE_ID_PRINTE_TRACE_LONG:
			common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_LONG_FILE_LINE, max_length);
			return common_tools_string_length(next_line);

		case TEST_CASE_ID_PRINTE_TRACE_ARRAY:
			common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_FILE_LINE, max_length);
			return common_tools_string_length(next_line);

		case TEST_CASE_ID_ORDER_OF_PRINT_TRACES:

			if (line_number == UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_LINE_NUMBER) {
				common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_FILE_LINE, max_length);
				return common_tools_string_length(next_line);
			}

			if (line_number == UNITTSET_TEST_CASE_PRINT_TRACE_LONG_LINE_NUMBER) {
				common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_LONG_FILE_LINE, max_length);
				return common_tools_string_length(next_line);
			}

			if (line_number == UNITTSET_TEST_CASE_PRINT_TRACE_WORD_LINE_NUMBER) {
				common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_WORD_FILE_LINE, max_length);
				return common_tools_string_length(next_line);
			}

			if (line_number == UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_LINE_NUMBER) {
				common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_FILE_LINE, max_length);
				return common_tools_string_length(next_line);
			}

			if (line_number == UNITTSET_TEST_CASE_PRINT_TRACE_PASS_LINE_NUMBER) {
				common_tools_string_copy_string(next_line, UNITTSET_TEST_CASE_PRINT_TRACE_PASS_FILE_LINE, max_length);
				return common_tools_string_length(next_line);
			}
	}
	return 0;
}

u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line) {
	counter_FILE_APPEND_LINE += 1;
	DEBUG_TRACE_STR(new_line, "file_append_line()");
	return 0;
}

void console_write_line(const char* line) {
	DEBUG_TRACE_STR(line, "console_write_line()");

	if (counter_CONSOLE_WRITE_LINE < TRACER_PARSED_TRACE_OBJECT_QEUE_SIZE) {
		common_tools_string_copy_string(console_out_put_string_list[counter_CONSOLE_WRITE_LINE], line, CONSOLE_OUTPUT_MAX_LINE_LEGNTH);
	}
	counter_CONSOLE_WRITE_LINE += 1;
}

// --------------------------------------------------------------------------------

static void TEST_CASE_init(void) {

	UT_START_TEST_CASE("Initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INIT);
		unittest_reset_counter();

		CLI_ARGUMENT_FILE_SIGNAL_init();
		CLI_ARGUMENT_PATH_SIGNAL_init();
		CLI_CONSOLE_ACTIVATED_SIGNAL_init();

		PRINT_TRACE_OBJECT_THREAD_init();

		CLI_ARGUMENT_FILE_SIGNAL_send("unittest/trace_output_file.txt");
		CLI_ARGUMENT_PATH_SIGNAL_send("unittest/source_base_path");
		CLI_CONSOLE_ACTIVATED_SIGNAL_send(NULL);

		PRINT_TRACE_OBJECT_THREAD_start();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(50) == 0) {
			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(0, counter_CONSOLE_WRITE_LINE);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_print_trace_pass(void) {

	UT_START_TEST_CASE("Print Trace Pass")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PRINTE_TRACE_PASS);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 0,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_PASS_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_PASS_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_PASS,
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_PASS_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_print_trace_byte(void) {

	UT_START_TEST_CASE("Print Trace Byte")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PRINTE_TRACE_BYTE);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 1,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_BYTE,
			.data.byte = UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_TRACE_DATA
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_print_trace_word(void) {

	UT_START_TEST_CASE("Print Trace Word")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PRINTE_TRACE_WORD);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 2,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_WORD_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_WORD_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_WORD,
			.data.word = UNITTEST_TEST_CASE_PRINT_TRACE_WORD_TRACE_DATA
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_WORD_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_print_trace_long(void) {

	UT_START_TEST_CASE("Print Trace Long")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PRINTE_TRACE_LONG);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 4,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_LONG_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_LONG_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_LONG,
			.data.integer = UNITTEST_TEST_CASE_PRINT_TRACE_LONG_TRACE_DATA
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_LONG_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_print_trace_array(void) {

	UT_START_TEST_CASE("Print Trace Array")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PRINTE_TRACE_ARRAY);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 25,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_ARRAY,
			.data.array = { UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_TRACE_DATA }
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_source_file_not_found(void) {

	UT_START_TEST_CASE("Source-File not found")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SOURCE_FILE_NOT_FOUND);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 0,
			.file_name = UNITTEST_TEST_CASE_FILE_NOT_FOUND_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_FILE_NOT_FOUND_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_PASS
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_FILE_NOT_FOUND_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_source_line_not_found(void) {

	UT_START_TEST_CASE("Source-Line not found")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SOURCE_LINE_NOT_FOUND);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj = {
			.data_length = 0,
			.file_name = UNITTEST_TEST_CASE_LINE_NOT_FOUND_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_LINE_NOT_FOUND_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_PASS
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 1);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_LINE_NOT_FOUND_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_order_of_print_traces(void) {

	UT_START_TEST_CASE("Order of print Traces")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_ORDER_OF_PRINT_TRACES);
		unittest_reset_counter();

		TRACE_OBJECT trace_obj_array = {
			.data_length = 25,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_ARRAY_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_ARRAY,
			.data.array = { UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_TRACE_DATA }
		};

		TRACE_OBJECT trace_obj_long = {
			.data_length = 4,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_LONG_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_LONG_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_LONG,
			.data.integer = UNITTEST_TEST_CASE_PRINT_TRACE_LONG_TRACE_DATA
		};

		TRACE_OBJECT trace_obj_word = {
			.data_length = 2,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_WORD_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_WORD_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_WORD,
			.data.word = UNITTEST_TEST_CASE_PRINT_TRACE_WORD_TRACE_DATA
		};

		TRACE_OBJECT trace_obj_byte = {
			.data_length = 1,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_BYTE_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_BYTE,
			.data.byte = UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_TRACE_DATA
		};

		TRACE_OBJECT trace_obj_pass = {
			.data_length = 0,
			.file_name = UNITTEST_TEST_CASE_PRINT_TRACE_PASS_FILE_NAME,
			.line_number = UNITTSET_TEST_CASE_PRINT_TRACE_PASS_LINE_NUMBER,
			.type = TRACE_OBJECT_TYPE_PASS,
		};

		u8 mutex_claimed = 0;

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();

			if (TRACE_OBJECT_QEUE_mutex_get()) {
				mutex_claimed = 1;
				TRACE_OBJECT_QEUE_enqeue(&trace_obj_array);
				TRACE_OBJECT_QEUE_enqeue(&trace_obj_long);
				TRACE_OBJECT_QEUE_enqeue(&trace_obj_word);
				TRACE_OBJECT_QEUE_enqeue(&trace_obj_byte);
				TRACE_OBJECT_QEUE_enqeue(&trace_obj_pass);
				TRACE_OBJECT_QEUE_mutex_release();
				break;
			}
		}
		UNITTEST_TIMER_stop();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {

			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();


		UT_CHECK_IS_EQUAL(mutex_claimed, 1);
		UT_CHECK_IS_EQUAL(counter_CONSOLE_WRITE_LINE, 5);
		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 5);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 5);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 5);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_GET_SIZE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_DELETE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_RENAME, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CREATE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_SPECIFIC_LINE, 5);
		UT_CHECK_IS_EQUAL(counter_FILE_APPEND_LINE, 5);
		UT_COMPARE_STRING(console_out_put_string_list[0], UNITTEST_TEST_CASE_PRINT_TRACE_ARRAY_CONSOLE_STRING);
		UT_COMPARE_STRING(console_out_put_string_list[1], UNITTEST_TEST_CASE_PRINT_TRACE_LONG_CONSOLE_STRING);
		UT_COMPARE_STRING(console_out_put_string_list[2], UNITTEST_TEST_CASE_PRINT_TRACE_WORD_CONSOLE_STRING);
		UT_COMPARE_STRING(console_out_put_string_list[3], UNITTEST_TEST_CASE_PRINT_TRACE_BYTE_CONSOLE_STRING);
		UT_COMPARE_STRING(console_out_put_string_list[4], UNITTEST_TEST_CASE_PRINT_TRACE_PASS_CONSOLE_STRING);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_terminate(void) {

	UT_START_TEST_CASE("Terminate")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TERMINATE);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();

		PRINT_TRACE_OBJECT_THREAD_terminate();

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}
		UNITTEST_TIMER_stop();
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for print trace object 1.0")
	{
		TEST_CASE_init();
		TEST_CASE_print_trace_pass();
		TEST_CASE_print_trace_byte();
		TEST_CASE_print_trace_word();
		TEST_CASE_print_trace_long();
		TEST_CASE_print_trace_array();
		TEST_CASE_source_file_not_found();
		TEST_CASE_source_line_not_found();
		TEST_CASE_order_of_print_traces();
		TEST_CASE_terminate();

	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
