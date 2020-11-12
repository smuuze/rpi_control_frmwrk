/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_msg_executer.c
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

#include "common/signal_slot_interface.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "app_tasks/message_executer_task.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_CONFIGURE					1
#define TEST_CASE_ID_FILE_NOT_EXISITING				2
#define TEST_CASE_ID_FILE_NOT_OPEN				3
#define TEST_CASE_ID_MESSAGE_RECIEVED				4
#define TEST_CASE_ID_PROCESS_REPORT_LIST			5

// --------------------------------------------------------------------------------

#define MAX_MSG_LENGTH						255
#define TEST_CASE_MSG_RECEIVED_MSG_STRING			"light_01_off"
#define TEST_CASE_MSG_RECEIVED_EXECUTION_STR			"date"
#define TEST_CASE_MSG_RECEIVED_COMMAND_03_STR			"light_03_off"

#define UT_COMMAND_FILEPATH					"command_file.txt"
#define UT_REPORT_FILE_PATH					"report_file.txt"

#define RESPONSE_RECEIVED_01					"020100"
#define RESPONSE_RECEIVED_02					"06.06.1984 - 08:03:00"

#define NULL_STRING						"\0\0"

// --------------------------------------------------------------------------------

static u8 ut_cli_command_received = 0;

char unittest_MSG_RECEIVED[MAX_MSG_LENGTH];
char unittest_RESPONSE_RECEIVED[MAX_MSG_LENGTH];

u8 counter_FILE_SET_PATH = 0;
u8 counter_FILE_HAS_CHANGED = 0;
u8 counter_FILE_IS_READABLE = 0;
u8 counter_FILE_IS_EXISTING = 0;
u8 counter_FILE_OPEN = 0;
u8 counter_FILE_CLOSE = 0;
u8 counter_FILE_READ_NEXT_LINE = 0;
u8 counter_COMMUNICATION_COMMAND_RECEIVED = 0;
u8 counter_COMMUNICATION_RESPONSE_RECEIVED = 0;
u8 counter_RESPONSE_TIMEOUT = 0;
u8 counter_FILE_LINE = 0;
u8 counter_INVALID_COMMAND = 0;
u8 counter_CLI_COMMAND_RECEIVED = 0;
u8 counter_FILE_OPEN_FAILED = 0;

// --------------------------------------------------------------------------------


static void unittest_reset_counter(void) {

	memset(unittest_RESPONSE_RECEIVED, '\0', MAX_MSG_LENGTH);

	counter_FILE_SET_PATH = 0;
	counter_FILE_HAS_CHANGED = 0;
	counter_FILE_IS_READABLE = 0;
	counter_FILE_IS_EXISTING = 0;
	counter_FILE_OPEN = 0;
	counter_FILE_CLOSE = 0;
	counter_FILE_READ_NEXT_LINE = 0;
	counter_COMMUNICATION_COMMAND_RECEIVED = 0;
	counter_COMMUNICATION_RESPONSE_RECEIVED = 0;
	counter_RESPONSE_TIMEOUT = 0;
	counter_FILE_LINE = 0;
	counter_INVALID_COMMAND = 0;
	counter_CLI_COMMAND_RECEIVED = 0;
	counter_FILE_OPEN_FAILED = 0;
}

static int preapre_line(char* line_to, char* line_from, int line_length, int max_line_length) {

	if (line_length > max_line_length) {
		line_length = max_line_length;
	}

	memset(line_to, '\0', max_line_length);
	memcpy(line_to, line_from, line_length);
	return line_length;
}

// --------------------------------------------------------------------------------

// stubs

u8 cfg_file_parser_match_cfg_key(const char* reference, const char* cfg_key) {

	if (strlen(reference) != strlen(cfg_key)) {
		return 0;
	}
	
	if (memcmp(reference, cfg_key, strlen(reference)) != 0) {
		return 0;
	}

	return 1;
}

void file_set_path(FILE_INTERFACE* p_file, const char* path) {

	DEBUG_PASS("file_set_path()");
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

	DEBUG_PASS("file_is_existing()");
	counter_FILE_IS_EXISTING += 1;

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_FILE_NOT_EXISITING) {
		return 0;
	}
	
	return 1;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
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

const char* file_get_path(FILE_INTERFACE* p_file) {
	return (const char*)p_file->path;
}

u8 file_create(FILE_INTERFACE* p_file) {

	DEBUG_PASS("file_create()");
	return 0;
}

u8 file_open(FILE_INTERFACE* p_file) {

	counter_FILE_OPEN += 1;

	if (file_is_existing(p_file) == 0) {
		return 0;
	}

	if (file_is_readable(p_file) == 0) {
		return 0;
	}

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_FILE_NOT_OPEN) {

		DEBUG_PASS("file_open() - FAILED !!! ---");
		console_write_line(p_file->path);
		return 0;
	}

	DEBUG_TRACE_byte(UT_GET_TEST_CASE_ID(), "file_open() - UT_GET_TEST_CASE_ID()");

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

	counter_FILE_READ_NEXT_LINE += 1;
	counter_FILE_LINE += 1;

	static char line_is_comment[] = "#This is a comment line";
	static char line_no_key_value_pair[] = "This line has no key value pair";
	static char line_has_multiple_equals[] = "i=have=multiple=equals";
	static char line_has_no_key[] = "=ihavenokey";
	static char line_starts_with_tab[] = "\ttab_line=ok";
	static char line_has_no_value[] = "ihavenovalue=";
	static char line_starts_with_whitespace[] = " whitespace_line=ok";
	static char line_light_off_command[] = " light_01_off=COM:0101";
	static char line_invalid_command[] = " light_03_off=COM:";
	static char line_get_date_command[] = " date=EXE:date";
	static char line_rpt_date_command[] = " rpt_date=EXE:date";

	DEBUG_TRACE_byte(counter_FILE_LINE, "file_read_next_line() - Line Number");

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_PROCESS_REPORT_LIST) {

		switch (counter_FILE_LINE) {
			default:
			case 1:
			case 2:
			case 3: return 0;
			case 4:
			case 5:  return preapre_line(line_to, line_is_comment, sizeof(line_is_comment), max_line_length);
			case 6:  return preapre_line(line_to, line_no_key_value_pair, sizeof(line_no_key_value_pair), max_line_length);
			case 7:  return preapre_line(line_to, line_has_multiple_equals, sizeof(line_has_multiple_equals), max_line_length);
			case 8:  return preapre_line(line_to, line_starts_with_tab, sizeof(line_starts_with_tab), max_line_length);
			case 9:  return preapre_line(line_to, line_has_no_value, sizeof(line_has_no_value), max_line_length);
			case 10: return preapre_line(line_to, line_has_no_key, sizeof(line_has_no_key), max_line_length);
			case 11: return preapre_line(line_to, line_starts_with_whitespace, sizeof(line_starts_with_whitespace), max_line_length);
			case 12: return preapre_line(line_to, line_light_off_command, sizeof(line_light_off_command), max_line_length);
			case 13: return preapre_line(line_to, line_invalid_command, sizeof(line_invalid_command), max_line_length);
			case 14: return preapre_line(line_to, line_get_date_command, sizeof(line_get_date_command), max_line_length);
			case 15: return preapre_line(line_to, line_rpt_date_command, sizeof(line_rpt_date_command), max_line_length);
			case 16: return preapre_line(line_to, line_rpt_date_command, sizeof(line_rpt_date_command), max_line_length);
			case 17: return preapre_line(line_to, line_rpt_date_command, sizeof(line_rpt_date_command), max_line_length);

			case 18:

				counter_FILE_LINE = 17;
				DEBUG_PASS("file_read_next_line() - END OF FILE");

				return -1;
		}

	} else {

		switch (counter_FILE_LINE) {
			default:
			case 1:
			case 2:
			case 3: return 0;
			case 4:
			case 5:  return preapre_line(line_to, line_is_comment, sizeof(line_is_comment), max_line_length);
			case 6:  return preapre_line(line_to, line_no_key_value_pair, sizeof(line_no_key_value_pair), max_line_length);
			case 7:  return preapre_line(line_to, line_has_multiple_equals, sizeof(line_has_multiple_equals), max_line_length);
			case 8:  return preapre_line(line_to, line_starts_with_tab, sizeof(line_starts_with_tab), max_line_length);
			case 9:  return preapre_line(line_to, line_has_no_value, sizeof(line_has_no_value), max_line_length);
			case 10: return preapre_line(line_to, line_has_no_key, sizeof(line_has_no_key), max_line_length);
			case 11: return preapre_line(line_to, line_starts_with_whitespace, sizeof(line_starts_with_whitespace), max_line_length);
			case 12: return preapre_line(line_to, line_light_off_command, sizeof(line_light_off_command), max_line_length);
			case 13: return preapre_line(line_to, line_get_date_command, sizeof(line_get_date_command), max_line_length);
			case 14: return preapre_line(line_to, line_invalid_command, sizeof(line_invalid_command), max_line_length);

			case 15:

				counter_FILE_LINE = 14;
				DEBUG_PASS("file_read_next_line() - END OF FILE");

				return -1;
		}
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

void unittest_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK()");
	counter_COMMUNICATION_COMMAND_RECEIVED += 1;

	__UNUSED__ COMMON_GENERIC_BUFFER_TYPE* p_com_buffer = (COMMON_GENERIC_BUFFER_TYPE*)p_argument;

	DEBUG_TRACE_N(p_com_buffer->length, p_com_buffer->data, "unittest_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK() - Command:");
}

void unittest_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

	__UNUSED__ const char* response_msg = (const char*)p_argument;
	counter_COMMUNICATION_RESPONSE_RECEIVED += 1;
	DEBUG_TRACE_STR(response_msg, "unittest_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT_CALLBACK()");

	memset(unittest_RESPONSE_RECEIVED, '\0', MAX_MSG_LENGTH);
	memcpy(unittest_RESPONSE_RECEIVED, response_msg, strlen(response_msg));
}

void unittest_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT_CALLBACK()");
	counter_RESPONSE_TIMEOUT += 1;
}

void unittest_MSG_EXECUTER_INVALID_COMMAND_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_MSG_EXECUTER_INVALID_COMMAND_SLOT_CALLBACK()");
	counter_INVALID_COMMAND += 1;
}

void unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

	__UNUSED__ const char* response_msg = (const char*)p_argument;
	counter_CLI_COMMAND_RECEIVED += 1;
	DEBUG_TRACE_STR(response_msg, "unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK()");

	memset(unittest_RESPONSE_RECEIVED, '\0', MAX_MSG_LENGTH);
	memcpy(unittest_RESPONSE_RECEIVED, response_msg, strlen(response_msg));

	ut_cli_command_received = 1;
}

void unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK(const void* p_argument) {

	__UNUSED__ const char* response_msg = (const char*)p_argument;
	counter_FILE_OPEN_FAILED += 1;
	DEBUG_TRACE_STR(response_msg, "unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK() - File:");
}



// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL, UT_CLI_EXECUTER_COMMAND_RECEIVED_SLOT, unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_RECEIVED)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, UT_RPI_HOST_COMMAND_RECEIVED_SLOT, unittest_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL, UT_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT, unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL, UT_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT, unittest_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_INVALID_COMMAND_SIGNAL, UT_MSG_EXECUTER_INVALID_COMMAND_SLOT, unittest_MSG_EXECUTER_INVALID_COMMAND_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL, UT_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT, unittest_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_msg_executer_init(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_INITIALIZE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		msg_executer_init();

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 2); // default file-path
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_configure(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_CONFIGURE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONFIGURE);

		unittest_reset_counter();

		CFG_FILE_PARSER_CFG_OBJECT_TYPE command_file_path_cfg_obj 	= { .key = COMMAND_FILE_PATH_CFG_NAME	, .value = UT_COMMAND_FILEPATH 	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE report_file_path_cfg_obj 	= { .key = REPORT_FILE_PATH_CFG_NAME	, .value = UT_REPORT_FILE_PATH 	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE unknown_cfg_obj 		= { .key = "UNKNOWN"			, .value = "I am unknown"	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE invalid_cfg_obj 		= { .key = "INVALID"			, .value = "I am invalid"	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE report_interval__cfg_obj 	= { .key = REPORT_INTERVAL_CFG_NAME	, .value = "5000"		};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&command_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&report_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&unknown_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&invalid_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&report_interval__cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(NULL);

		CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

		u8 number_of_task_run = 10;

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(MSG_EXECUTER_TASK_SCHEDULE_INTERVAL_MS * number_of_task_run) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 2);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_message_received(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_MESSAGE_RECEIVED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_send(TEST_CASE_MSG_RECEIVED_MSG_STRING);

		u8 number_of_task_run = 10;
		u8 signal_send = 0;

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(MSG_EXECUTER_TASK_SCHEDULE_INTERVAL_MS * number_of_task_run) == 0) {
			mcu_task_controller_schedule();

			if (counter_COMMUNICATION_COMMAND_RECEIVED == 1 && signal_send == 0) {
				signal_send = 1;

				COMMON_GENERIC_BUFFER_TYPE response_buffer = {
					.length = 3,
					.data = {0x02, 0x01, 0x00}
				};

				RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send(&response_buffer);
			}
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 12);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, RESPONSE_RECEIVED_01);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_response_timeout(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_RESPONSE_TIMEOUT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_send(TEST_CASE_MSG_RECEIVED_MSG_STRING);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 12);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 1);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_execution_command(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_EXECUTION_COMMAND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_send(TEST_CASE_MSG_RECEIVED_EXECUTION_STR);

		u8 signal_send = 0;
		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();

			if (counter_CLI_COMMAND_RECEIVED == 1 && signal_send == 0) {
				signal_send = 1;
				CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send(RESPONSE_RECEIVED_02);
			}
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 13);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, RESPONSE_RECEIVED_02);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_invalid_command(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_INVALID_COMMAND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_send(TEST_CASE_MSG_RECEIVED_COMMAND_03_STR);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 14);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 1);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, NULL_STRING);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_command_file_not_existing(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_COMMAND_FILE_NOT_EXISTING")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_FILE_NOT_EXISITING);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_send(TEST_CASE_MSG_RECEIVED_COMMAND_03_STR);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, NULL_STRING);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_process_report_list(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_PROCESS_REPORT_LIST")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PROCESS_REPORT_LIST);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(10000) == 0) {
			mcu_task_controller_schedule();

			if (ut_cli_command_received) {
				ut_cli_command_received = 0;
				CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send(RESPONSE_RECEIVED_02);
			}
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, NULL_STRING);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	TRACER_DISABLE();

	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_set_timeout(0);

	CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
	CFG_PARSER_CFG_COMPLETE_SIGNAL_set_timeout(0);

	MQTT_MESSAGE_RECEIVED_init();

	RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();
	UT_RPI_HOST_COMMAND_RECEIVED_SLOT_connect();

	RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init();

	CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_init();
	CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_init();
	UT_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_connect();

	UT_START_TESTBENCH("Welcome the the UNITTEST for msg-executer v1.0")
	{
		UNITTEST_msg_executer_init();

		UT_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT_connect();
		UT_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT_connect();
		UT_MSG_EXECUTER_INVALID_COMMAND_SLOT_connect();
		UT_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_connect();

		UNITTEST_msg_executer_configure();
		UNITTEST_msg_executer_message_received();
		UNITTEST_msg_executer_message_received();
		UNITTEST_msg_executer_response_timeout();
		UNITTEST_msg_executer_execution_command();
		UNITTEST_msg_executer_invalid_command();
		UNITTEST_msg_executer_command_file_not_existing();

		TRACER_ENABLE();
		UNITTEST_msg_executer_process_report_list();
		TRACER_DISABLE();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------