/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_cfg_file_parser.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
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

#include "common/signal_slot_interface.h"

#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/console/ui_console.h"
#include "ui/file_interface/file_interface.h"

// --------------------------------------------------------------------------------

#define UNITTEST_CFG_FILE_PATH				"unittest_cfg_file.cfg"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_FILE_NOT_EXISITING			0
#define TEST_CASE_ID_FILE_NOT_READABLE			1
#define TEST_CASE_ID_FILE_NOT_OPEN			2

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_LINE_IS_EMPTY_01			3
#define TEST_CASE_ID_LINE_IS_EMPTY_02			4
#define TEST_CASE_ID_LINE_IS_COMMENT_01			5
#define TEST_CASE_ID_LINE_IS_EMPTY_03			6
#define TEST_CASE_ID_LINE_IS_COMMENT_02			7
#define TEST_CASE_ID_LINE_IS_NO_CFG_PAIR_01		8
#define TEST_CASE_ID_LINE_IS_TOO_LONG_01		9
#define TEST_CASE_ID_LINE_CONTAINS_MULTIPLE_EQUALS	10
#define TEST_CASE_ID_LINE_KEY_NOT_AVAILABLE_01		11
#define TEST_CASE_ID_LINE_VALUE_NOT_AVAILABLE_01	12
#define TEST_CASE_ID_LINE_STARTS_WITH_WHITESPACE	13
#define TEST_CASE_ID_LINE_STARTS_WITH_TAB		14

#define TEST_CASE_ID_END_OF_FILE			15

// --------------------------------------------------------------------------------

static int preapre_line(char* line_to, char* line_from, int line_length, int max_line_length) {

	if (line_length > max_line_length) {
		line_length = max_line_length;
	}

	memset(line_to, '\0', max_line_length);
	memcpy(line_to, line_from, line_length);
	return line_length;
}

// --------------------------------------------------------------------------------

static u16 test_case_counter = 0xFFFF;

// stubs

void file_set_path(FILE_INTERFACE* p_file, const char* path) {

	DEBUG_PASS("file_set_path()");

	memcpy(p_file->path, path, strlen(path));
}

const char* file_get_path(FILE_INTERFACE* p_file) {
	return (const char*)p_file->path;
}

u8 file_has_changed(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_has_changed()");
	return 0;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
	if (test_case_counter == TEST_CASE_ID_FILE_NOT_OPEN) {
		return 0;
	}

	return 1;
}

u8 file_is_readable(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_is_readable()");
	return 1;
}

u8 file_is_existing(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_is_existing()");

	if (test_case_counter == TEST_CASE_ID_FILE_NOT_EXISITING) {
		return 0;
	}
	
	return 1;
}

u32 file_get_size(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_get_size()");
	return 0;
}

u8 file_delete(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_delete()");
	return 0;
}

u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path) {

	DEBUG_PASS("file_rename()");
	return 0;
}

u8 file_create(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_create()");
	return 0;
}

u8 file_open(FILE_INTERFACE* p_file) {

	if (file_is_existing(p_file) == 0) {
		return 0;
	}

	if (file_is_readable(p_file) == 0) {
		return 0;
	}

	if (test_case_counter == TEST_CASE_ID_FILE_NOT_OPEN) {

		DEBUG_TRACE_STR(p_file->path, "file_open() - FAILED !!! ---");
		return 0;
	}

	DEBUG_TRACE_byte(test_case_counter, "file_open() - test_case_counter");

	return 1;
}

void file_close(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_close()");
}

i16 file_read_line(FILE* file_handle, char* line_to, u16 num_max_bytes) {

	DEBUG_PASS("file_read_line()");
	return 0;
}

i16 file_read_next_line(FILE_INTERFACE* p_file, char* line_to, u16 max_line_length) {

	static u8 line_counter = 0;

	line_counter += 1;

	static char line_is_comment[] = "#This is a comment line";
	static char line_no_key_value_pair[] = "This line has no key value pair";
	static char line_has_multiple_equals[] = "i=have=multiple=equals";
	static char line_has_no_key[] = "=ihavenokey";
	static char line_starts_with_tab[] = "\ttab_line=ok";
	static char line_has_no_value[] = "ihavenovalue=";
	static char line_starts_with_whitespace[] = " whitespace_line=ok";

	//DEBUG_TRACE_byte(line_counter, "file_read_next_line() - Line Number");

	switch (line_counter) {
		default:
		case 1:
		case 2:
		case 3:

			return 0;

		case 4:
		case 5:

			return preapre_line(line_to, line_is_comment, sizeof(line_is_comment), max_line_length);

		case 6:

			return preapre_line(line_to, line_no_key_value_pair, sizeof(line_no_key_value_pair), max_line_length);

		case 7:

			return preapre_line(line_to, line_has_multiple_equals, sizeof(line_has_multiple_equals), max_line_length);

		case 8:

			return preapre_line(line_to, line_starts_with_tab, sizeof(line_starts_with_tab), max_line_length);

		case 9:

			return preapre_line(line_to, line_has_no_value, sizeof(line_has_no_value), max_line_length);

		case 10:

			return preapre_line(line_to, line_has_no_key, sizeof(line_has_no_key), max_line_length);

		case 11:

			return preapre_line(line_to, line_starts_with_whitespace, sizeof(line_starts_with_whitespace), max_line_length);

		case 12:

			line_counter = 11;
			DEBUG_PASS("file_read_next_line() - END OF FILE");

			return -1;
	}
}

u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length) {
	return 0;
}

u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line) {
	return 0;
}

// --------------------------------------------------------------------------------

// slots

static u8 counter_NEW_CFG_OBJECT_RECEIVED = 0;

static void unittest_CFG_PARSER_NEW_OBJECT_CALLBACK(const void* p_argument) {

	CFG_FILE_PARSER_CFG_OBJECT_TYPE* cfg_obj = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*)p_argument;

	DEBUG_PASS("unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK()");
	DEBUG_TRACE_STR(cfg_obj->key, "unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK() - KEY: ");
	DEBUG_TRACE_STR(cfg_obj->value, "unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK() - VALUE:");

	counter_NEW_CFG_OBJECT_RECEIVED += 1;
}

static void unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK(const void* p_argument) {
	(void) p_argument;

	DEBUG_PASS("unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK()");
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, UNITTEST_NEW_CFG_OBJECT_SLOT, unittest_CFG_PARSER_NEW_OBJECT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_CFG_COMPLETE_SIGNAL, UNITTEST_CFG_COMPLETE_SLOT, unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONFIGURATION_SIGNAL)

// --------------------------------------------------------------------------------

static void TEST_CASE_file_not_existing(void) {

	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND_UNKNOWN")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_FILE_NOT_OPEN);

		const char cfg_file_path[] = UNITTEST_CFG_FILE_PATH;

		CLI_CONFIGURATION_SIGNAL_send((void*)cfg_file_path);
		cfg_file_parser_task_get_state();
		cfg_file_parser_task_run();

		//UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 0);
		//UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		//UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_file_existing(void) {

	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND_UNKNOWN")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_LINE_IS_EMPTY_01);

		const char cfg_file_path[] = UNITTEST_CFG_FILE_PATH;

		CLI_CONFIGURATION_SIGNAL_send((void*)cfg_file_path);
		cfg_file_parser_task_get_state();
		cfg_file_parser_task_run();

		//UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 0);
		//UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		//UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for cfg-file-parser v2.0")
	{
		cfg_file_parser_init();
		cfg_file_parser_task_init();
		UNITTEST_NEW_CFG_OBJECT_SLOT_connect();
		UNITTEST_CFG_COMPLETE_SLOT_connect();

		TEST_CASE_file_not_existing();

		TEST_CASE_file_existing();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
