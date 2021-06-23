/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_msg_executer.c
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

#include "common/signal_slot_interface.h"
#include "common/common_tools_string.h"
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
#define TEST_CASE_ID_NEW_COMMAND_WHILE_REPORT			6
#define TEST_CASE_ID_INVALID_COMMAND_SYNTAX			7
#define TEST_CASE_ID_BAD_COMMAND_WHILE_REPORT			8

// --------------------------------------------------------------------------------

#define MAX_MSG_LENGTH						255
#define UT_MQTT_MESSAGE_LIST_LENGTH				10

#define TEST_CASE_MSG_RECEIVED_MSG_STRING			"cmd_light_01_off"
#define TEST_CASE_MSG_RECEIVED_EXECUTION_STR			"date"
#define TEST_CASE_MSG_RECEIVED_COMMAND_03_STR			"cmd_light_05_off"

#define UT_COMMAND_FILEPATH					"command_file.txt"
#define UT_REPORT_FILE_PATH					"report_file.txt"

#define RESPONSE_RECEIVED_01					"020100"
#define RESPONSE_RECEIVED_02					"06.08.1984 - 08:03:00"

#define RESPONSE_REPORT_EXECUTION_DATE				"rpt_date=06.08.1984 - 08:03:00"

#define NULL_STRING						"\0"

#define UT_CASE_INVALID_COMMAND_SYNTAX_COMMAND_MESSAGE_STR	"version"
#define UT_CASE_INVALID_COMMAND_SYNTAX_COMMAND_FILE_LINE	"version=COM:0101"

// --------------------------------------------------------------------------------

static u8 ut_cli_command_received = 0;

static COMMON_GENERIC_BUFFER_TYPE pending_pi_response;

char unittest_MSG_RECEIVED[MAX_MSG_LENGTH];
char unittest_RESPONSE_RECEIVED[MAX_MSG_LENGTH];
char unittest_CLI_COMMAND_RECEIVED[MAX_MSG_LENGTH];

char unittest_mqtt_message_to_send_list[UT_MQTT_MESSAGE_LIST_LENGTH][MAX_MSG_LENGTH];

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
u8 counter_FILE_LINE_REPORT_FILE = 0;
u8 counter_FILE_LINE_COMMAND_FILE = 0;
u8 counter_INVALID_COMMAND = 0;
u8 counter_INVALID_COMMAND_SYNTAX = 0;
u8 counter_CLI_COMMAND_RECEIVED = 0;
u8 counter_FILE_OPEN_FAILED = 0;
u8 counter_MQTT_MESSAGE_TO_SEND = 0;

// --------------------------------------------------------------------------------


static void unittest_reset_counter(void) {

	memset(unittest_RESPONSE_RECEIVED, '\0', MAX_MSG_LENGTH);
	memset(unittest_CLI_COMMAND_RECEIVED, '\0', MAX_MSG_LENGTH);

	u8 i = 0;
	for ( ; i < UT_MQTT_MESSAGE_LIST_LENGTH; i++) {
		memset(unittest_mqtt_message_to_send_list[i], '\0', MAX_MSG_LENGTH);
	}

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
	counter_FILE_LINE_REPORT_FILE = 0;
	counter_FILE_LINE_COMMAND_FILE = 0;
	counter_INVALID_COMMAND = 0;
	counter_INVALID_COMMAND_SYNTAX = 0;
	counter_CLI_COMMAND_RECEIVED = 0;
	counter_FILE_OPEN_FAILED = 0;
	counter_MQTT_MESSAGE_TO_SEND = 0;

	pending_pi_response.length = 0;
	memcpy(pending_pi_response.data, 0, pending_pi_response.length);
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

	DEBUG_PASS("file_is_existing()");
	counter_FILE_IS_EXISTING += 1;

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_FILE_NOT_EXISITING) {
		return 0;
	}
	
	return 1;
}

u8 file_is_open(FILE_INTERFACE* p_file) {
	return counter_FILE_OPEN != 0;
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

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_MESSAGE_RECIEVED) {
		if (memcmp(p_file->path, UT_COMMAND_FILEPATH, strlen(UT_COMMAND_FILEPATH)) != 0) {
			DEBUG_TRACE_STR(p_file->path, "file_open() - unexpected file for this test-case (TEST_CASE_ID_MESSAGE_RECIEVED)");
			return 0;
		}
	}

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_PROCESS_REPORT_LIST) {
		if (memcmp(p_file->path, UT_REPORT_FILE_PATH, strlen(UT_REPORT_FILE_PATH)) != 0) {
			DEBUG_TRACE_STR(p_file->path, "file_open() - unexpected file for this test-case (TEST_CASE_ID_PROCESS_REPORT_LIST)");
			return 0;
		}
	}

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

	DEBUG_TRACE_STR(p_file->path, "file_open() - File opened : ");

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

	DEBUG_TRACE_STR(p_file->path, "file_read_next_line() - Reading from file: ");
	DEBUG_TRACE_byte(counter_FILE_READ_NEXT_LINE, "file_read_next_line - Reading from Line:");

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_INVALID_COMMAND_SYNTAX) {

		switch (counter_FILE_READ_NEXT_LINE) {

			default:
				DEBUG_PASS("file_read_next_line() - END OF FILE");
				return -1;

			case 1: return common_tools_string_copy_string(line_to, "version=COM:0101", max_line_length);
		}
	}

	if (memcmp(p_file->path, UT_COMMAND_FILEPATH, strlen(UT_COMMAND_FILEPATH)) == 0) {
		counter_FILE_LINE_COMMAND_FILE += 1;

		switch (counter_FILE_LINE_COMMAND_FILE) {

			case 1:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 2:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 3:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 4:  return common_tools_string_copy_string(line_to, "#This is a comment line", max_line_length);
			case 5:  return common_tools_string_copy_string(line_to, "#This is a comment line", max_line_length);
			case 6:  return common_tools_string_copy_string(line_to, "This line has no key value pair", max_line_length);
			case 7:  return common_tools_string_copy_string(line_to, "This=line=has=multiple=equals", max_line_length);
			case 8:  return common_tools_string_copy_string(line_to, "\t\tThis lien starts with tabs", max_line_length);
			case 9:  return common_tools_string_copy_string(line_to, "This_line_has_no_value=", max_line_length);
			case 10: return common_tools_string_copy_string(line_to, "=This_line_has_no_key", max_line_length);
			case 11: return common_tools_string_copy_string(line_to, " starts_with_white_space=OK", max_line_length);

			case 12: return common_tools_string_copy_string(line_to, "version=com:0101", max_line_length);
			case 13: return common_tools_string_copy_string(line_to, "cmd_light_01_on=com:0B0401010000000000000000", max_line_length);
			case 14: return common_tools_string_copy_string(line_to, "cmd_light_01_off=com:0B0401000000000000000000", max_line_length);
			case 15: return common_tools_string_copy_string(line_to, "cmd_light_02_on=com:0B0402010000000000000000", max_line_length);
			case 16: return common_tools_string_copy_string(line_to, "cmd_light_02_off=com:0B0402000000000000000000", max_line_length);
			case 17: return common_tools_string_copy_string(line_to, "cmd_light_03_on=com:0B0403010000000000000000", max_line_length);
			case 18: return common_tools_string_copy_string(line_to, "cmd_light_03_off=com:0B0403000000000000000000", max_line_length);
			case 19: return common_tools_string_copy_string(line_to, "cmd_light_04_on=com:0B0404010000000000000000", max_line_length);
			case 20: return common_tools_string_copy_string(line_to, "cmd_light_04_off=com:0B0404000000000000000000", max_line_length);
			case 21: return common_tools_string_copy_string(line_to, "date=exe:date", max_line_length);

			default:
			case 22:

				DEBUG_PASS("file_read_next_line() - END OF FILE");

				return -1;
		}
	}

	if (memcmp(p_file->path, UT_REPORT_FILE_PATH, strlen(UT_REPORT_FILE_PATH)) == 0) {
		counter_FILE_LINE_REPORT_FILE += 1;
		
		switch (counter_FILE_LINE_REPORT_FILE) {

			case 1:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 2:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 3:  return common_tools_string_copy_string(line_to, "", max_line_length);
			case 4:  return common_tools_string_copy_string(line_to, "#This is a comment line", max_line_length);
			case 5:  return common_tools_string_copy_string(line_to, "#This is a comment line", max_line_length);
			case 6:  return common_tools_string_copy_string(line_to, "This line has no key value pair", max_line_length);
			case 7:  return common_tools_string_copy_string(line_to, "This=line=has=multiple=equals", max_line_length);
			case 8:  return common_tools_string_copy_string(line_to, "\t\tThis lien starts with tabs", max_line_length);
			case 9:  return common_tools_string_copy_string(line_to, "This_line_has_no_value=", max_line_length);
			case 10: return common_tools_string_copy_string(line_to, "=This_line_has_no_key", max_line_length);
			case 11: return common_tools_string_copy_string(line_to, " starts_with_white_space=OK", max_line_length);

			case 12: return common_tools_string_copy_string(line_to, "TEMPERATURE=com:0107", max_line_length);
			case 13: return common_tools_string_copy_string(line_to, "HUMIDITY=com:0108", max_line_length);
			case 14: return common_tools_string_copy_string(line_to, "OUTPUT_STATE_1=com:020501", max_line_length);
			case 15: return common_tools_string_copy_string(line_to, "OUTPUT_STATE_2=com:020502", max_line_length);
			case 16: return common_tools_string_copy_string(line_to, "OUTPUT_STATE_3=com:020503", max_line_length);
			case 17: return common_tools_string_copy_string(line_to, "OUTPUT_STATE_4=com:020504", max_line_length);
			case 18: return common_tools_string_copy_string(line_to, "AMBILIGHT=com:010A", max_line_length);
			case 19: return common_tools_string_copy_string(line_to, "SYSTEM_HOSTNAME=exe:hostname", max_line_length);
			case 20: return common_tools_string_copy_string(line_to, "SYSTEM_DATE=exe:date", max_line_length);
			case 21: return common_tools_string_copy_string(line_to, "SYSTEM_UPTIME=exe:uptime", max_line_length);

			default:
			case 22:

				DEBUG_PASS("file_read_next_line() - END OF FILE");

				return -1;
		}
	}	

	return -1;
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

	pending_pi_response.length = p_com_buffer->length;
	memcpy(pending_pi_response.data, p_com_buffer->data, p_com_buffer->length);
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

void unittest_MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_TRACE_STR((const char*)p_argument, "unittest_MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SLOT_CALLBACK()");
	counter_INVALID_COMMAND_SYNTAX += 1;
}

void unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK(const void* p_argument) {

	__UNUSED__ const char* response_msg = (const char*)p_argument;
	counter_FILE_OPEN_FAILED += 1;
	DEBUG_TRACE_STR(response_msg, "unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK() - File:");
}

void unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

	__UNUSED__ const char* response_msg = (const char*)p_argument;
	counter_CLI_COMMAND_RECEIVED += 1;
	DEBUG_TRACE_STR(response_msg, "unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK()");

	memset(unittest_CLI_COMMAND_RECEIVED, '\0', MAX_MSG_LENGTH);
	memcpy(unittest_CLI_COMMAND_RECEIVED, response_msg, strlen(response_msg));

	ut_cli_command_received = 1;
}

void unittest_MQTT_MESSAGE_TO_SEND_SLOT_CALLBACK(const void* p_argument) {

	DEBUG_TRACE_STR((const char*)p_argument, "unittest_MQTT_MESSAGE_TO_SEND_SLOT_CALLBACK()");

	if (counter_MQTT_MESSAGE_TO_SEND < UT_MQTT_MESSAGE_LIST_LENGTH) {

		const char* new_message = (const char*)p_argument;
		u16 string_length = common_tools_string_length(new_message);

		if (string_length > MAX_MSG_LENGTH) {
			memcpy(unittest_mqtt_message_to_send_list[counter_MQTT_MESSAGE_TO_SEND], new_message, MAX_MSG_LENGTH);
		} else {
			memcpy(unittest_mqtt_message_to_send_list[counter_MQTT_MESSAGE_TO_SEND], new_message, string_length);
		}
	}

	counter_MQTT_MESSAGE_TO_SEND += 1;
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, UT_RPI_HOST_COMMAND_RECEIVED_SLOT, unittest_RPI_HOST_COMMAND_RECEIVED_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL, UT_CLI_EXECUTER_COMMAND_RECEIVED_SLOT, unittest_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_TO_SEND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_MESSAGE_TO_SEND_SIGNAL, UT_MQTT_MESSAGE_TO_SEND_SLOT, unittest_MQTT_MESSAGE_TO_SEND_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL, UT_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT, unittest_MSG_EXECUTER_FILE_OPEN_FAILED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL, UT_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT, unittest_MSG_EXECUTER_RESPONSE_TIMEOUT_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_INVALID_COMMAND_SIGNAL, UT_MSG_EXECUTER_INVALID_COMMAND_SLOT, unittest_MSG_EXECUTER_INVALID_COMMAND_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL, UT_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT, unittest_MSG_EXECUTER_RESPONSE_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SIGNAL, UT_MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SLOT, unittest_MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SLOT_CALLBACK)


SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL()

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void ut_helper_generate_response(void) {

	if (pending_pi_response.length != 0) {

		DEBUG_TRACE_byte(pending_pi_response.data[0], "ut_helper_generate_response() - RPi response - Command:");

		switch (pending_pi_response.data[1]) {
			case 0x07 :

				pending_pi_response.length = 6;
				pending_pi_response.data[0] = 0x07;
				pending_pi_response.data[1] = 0x00;
				pending_pi_response.data[2] = 0x10;
				pending_pi_response.data[3] = 0x10;
				pending_pi_response.data[4] = 0x10;
				pending_pi_response.data[5] = 0x10;
				break;

			case 0x08 :

				pending_pi_response.length = 6;
				pending_pi_response.data[0] = 0x08;
				pending_pi_response.data[1] = 0x00;
				pending_pi_response.data[2] = 0x30;
				pending_pi_response.data[3] = 0x30;
				pending_pi_response.data[4] = 0x30;
				pending_pi_response.data[5] = 0x30;
				break;

			case 0x0A :

				pending_pi_response.length = 6;
				pending_pi_response.data[0] = 0x0A;
				pending_pi_response.data[1] = 0x00;
				pending_pi_response.data[2] = 0x40;
				pending_pi_response.data[3] = 0x40;
				pending_pi_response.data[4] = 0x40;
				pending_pi_response.data[5] = 0x40;
				break;

			case 0x05 :

				pending_pi_response.length = 0x0F;
				pending_pi_response.data[0] = 0x05;
				pending_pi_response.data[1] = 0x00;

				//pending_pi_response.data[2] = 0x01;
				pending_pi_response.data[3] = 0x01;

				pending_pi_response.data[4] = 0x50;
				pending_pi_response.data[5] = 0x50;
				pending_pi_response.data[6] = 0x50;
				pending_pi_response.data[7] = 0x50;

				pending_pi_response.data[8] = 0;
				pending_pi_response.data[9] = 0;
				pending_pi_response.data[10] = 0;
				pending_pi_response.data[11] = 0;

				pending_pi_response.data[12] = 0;
				pending_pi_response.data[13] = 0;
				pending_pi_response.data[14] = 0;
				pending_pi_response.data[15] = 0;
				break;

			default:
				pending_pi_response.length = 2;
				pending_pi_response.data[0] = pending_pi_response.data[1];
				pending_pi_response.data[1] = 0;
				break;
		}

		RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send((const void*) &pending_pi_response);

		pending_pi_response.length = 0;
		memcpy(pending_pi_response.data, 0, pending_pi_response.length);

		return;
	}

	if (common_tools_string_compare(unittest_CLI_COMMAND_RECEIVED, "uptime")) {
		CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send("5 days, 10 hurs, 5 minutes, 6 seconds");
	}

	if (common_tools_string_compare(unittest_CLI_COMMAND_RECEIVED, "hostname")) {
		if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_BAD_COMMAND_WHILE_REPORT) {
			DEBUG_PASS("ut_helper_generate_response() - Send BAD_COMMAND signal - - - - - - - - - - - - - - - -");
			CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL_send(NULL);
		} else {
			CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send("UNITTEST");
		}
	}

	if (common_tools_string_compare(unittest_CLI_COMMAND_RECEIVED, "date")) {
		CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send("06.08.1984 - 08:03");
	}	

	common_tools_string_clear(unittest_CLI_COMMAND_RECEIVED, MAX_MSG_LENGTH);	
}

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
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 0);
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
		CFG_FILE_PARSER_CFG_OBJECT_TYPE report_interval__cfg_obj 	= { .key = REPORT_INTERVAL_CFG_NAME	, .value = "50000"		};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&command_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&report_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&unknown_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&invalid_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&report_interval__cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(NULL);

		CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
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
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_message_received(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_MESSAGE_RECEIVED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_MSG_STRING);

		u8 signal_send = 0;

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();

			if (counter_COMMUNICATION_COMMAND_RECEIVED == 1 && signal_send == 0) {
				signal_send = 1;

				COMMON_GENERIC_BUFFER_TYPE response_buffer = {
					.length = 2,
					.data = {0x04, 0x00}
				};

				RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send(&response_buffer);
			}
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 14);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, "{\"RESPONSE\":{\"SET_OUTPUT_STATE\":{\"ERR\":\"OK\"}}}");
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 14);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_response_timeout(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_RESPONSE_TIMEOUT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_MSG_STRING);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 14);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 1);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 14);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 1);
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[0], "{\"RESPONSE\":{\"cmd_light_01_off\":{\"ERR\":\"TIMEOUT\"}}}");
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_execution_command(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_EXECUTION_COMMAND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_EXECUTION_STR);

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
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 21);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, "{\"RESPONSE\":{\"date\":\"06.08.1984 - 08:03:00\"}}" );
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 21);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_invalid_command(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_INVALID_COMMAND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_RECIEVED);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_COMMAND_03_STR);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 22);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 1);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, NULL_STRING);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 22);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_invalid_command_syntax(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_INVALID_COMMAND_SYNTAX")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INVALID_COMMAND_SYNTAX);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(UT_CASE_INVALID_COMMAND_SYNTAX_COMMAND_MESSAGE_STR);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 2);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 1);
		UT_COMPARE_STRING(unittest_RESPONSE_RECEIVED, NULL_STRING);
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_command_file_not_existing(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_COMMAND_FILE_NOT_EXISTING")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_FILE_NOT_EXISITING);

		unittest_reset_counter();

		MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_COMMAND_03_STR);

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
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_process_report_list(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_PROCESS_REPORT_LIST")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_PROCESS_REPORT_LIST);

		unittest_reset_counter();

		CFG_FILE_PARSER_CFG_OBJECT_TYPE report_interval__cfg_obj = {
			.key = REPORT_INTERVAL_CFG_NAME,
			.value = "5000"
		};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&report_interval__cfg_obj);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(6000) == 0) {
			mcu_task_controller_schedule();
			ut_helper_generate_response();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 22);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 7);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 10);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[0], "{\"REPORT\":{\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":16,\"MAX\":16,\"MIN\":16,\"MEAN\":16}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[1], "{\"REPORT\":{\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":48,\"MAX\":48,\"MIN\":48,\"MEAN\":48}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[2], "{\"REPORT\":{\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[3], "{\"REPORT\":{\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[4], "{\"REPORT\":{\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[5], "{\"REPORT\":{\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[6], "{\"REPORT\":{\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":64,\"MAX\":64,\"MIN\":64,\"MEAN\":64}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[7], "{\"REPORT\":{\"SYSTEM_HOSTNAME\":\"UNITTEST\"}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[8], "{\"REPORT\":{\"SYSTEM_DATE\":\"06.08.1984 - 08:03\"}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[9], "{\"REPORT\":{\"SYSTEM_UPTIME\":\"5 days, 10 hurs, 5 minutes, 6 seconds\"}}");
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 3);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 22);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 10);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_new_message_while_processing_report(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_NEW_COMMAND_WHILE_PROCESSING_REPORT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_NEW_COMMAND_WHILE_REPORT);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 command_send = 0;

		while (UNITTEST_TIMER_is_up(4500) == 0) {

			mcu_task_controller_schedule();
			ut_helper_generate_response();

			if (counter_COMMUNICATION_COMMAND_RECEIVED == 1 && command_send == 0) {

				DEBUG_PASS("UNITTEST_msg_executer_new_message_while_processing_report() - Sending Command - - - - - - - - - - - - ");

				command_send = 1;
				MQTT_MESSAGE_RECEIVED_SIGNAL_send(TEST_CASE_MSG_RECEIVED_EXECUTION_STR);
			}
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 2);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 2);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 2);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_READ_NEXT_LINE, 43);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 7);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 11);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 0);
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[0], "{\"REPORT\":{\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":16,\"MAX\":16,\"MIN\":16,\"MEAN\":16}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[1], "{\"RESPONSE\":{\"date\":\"06.08.1984 - 08:03\"}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[2], "{\"REPORT\":{\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":48,\"MAX\":48,\"MIN\":48,\"MEAN\":48}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[3], "{\"REPORT\":{\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[4], "{\"REPORT\":{\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[5], "{\"REPORT\":{\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[6], "{\"REPORT\":{\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[7], "{\"REPORT\":{\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":64,\"MAX\":64,\"MIN\":64,\"MEAN\":64}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[8], "{\"REPORT\":{\"SYSTEM_HOSTNAME\":\"UNITTEST\"}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[9], "{\"REPORT\":{\"SYSTEM_DATE\":\"06.08.1984 - 08:03\"}}");
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 4);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 22);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 21);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 11);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_msg_executer_bad_command_while_processing_report(void) {
	
	UT_START_TEST_CASE("MSG_EXECUTER_BAD_COMMAND_WHILE_PROCESSING_REPORT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_BAD_COMMAND_WHILE_REPORT);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(10000) == 0) {

			mcu_task_controller_schedule();
			ut_helper_generate_response();
		}

		UT_CHECK_IS_EQUAL(counter_FILE_SET_PATH, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_HAS_CHANGED, 0);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_READABLE, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_IS_EXISTING, 1);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN, 1);
		UT_CHECK_IS_GREATER(counter_FILE_CLOSE, 0);
		UT_CHECK_IS_BETWEEN(counter_FILE_READ_NEXT_LINE, 24, 21);//		UT_CHECK_IS_EQUAL(, 22);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_COMMAND_RECEIVED, 7);
		UT_CHECK_IS_EQUAL(counter_COMMUNICATION_RESPONSE_RECEIVED, 10);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND, 1);
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[0], "{\"REPORT\":{\"TEMPERATURE\":{\"ERR\":\"OK\",\"ACT\":16,\"MAX\":16,\"MIN\":16,\"MEAN\":16}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[1], "{\"REPORT\":{\"HUMIDITY\":{\"ERR\":\"OK\",\"ACT\":48,\"MAX\":48,\"MIN\":48,\"MEAN\":48}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[2], "{\"REPORT\":{\"OUTPUT_STATE_1\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[3], "{\"REPORT\":{\"OUTPUT_STATE_2\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[4], "{\"REPORT\":{\"OUTPUT_STATE_3\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[5], "{\"REPORT\":{\"OUTPUT_STATE_4\":{\"ERR\":\"OK\",\"PIN_STATE\":\"ON\",\"ON_TIME_MS\":1347440720,\"DURATION_MS\":0,\"PERIOD_MS\":0}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[6], "{\"REPORT\":{\"AMBILIGHT\":{\"ERR\":\"OK\",\"ACT\":64,\"MAX\":64,\"MIN\":64,\"MEAN\":64}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[7], "{\"REPORT\":{\"SYSTEM_HOSTNAME\":{\"ERR\":\"BAD_COMMAND\"}}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[8], "{\"REPORT\":{\"SYSTEM_DATE\":\"06.08.1984 - 08:03\"}}");
		UT_COMPARE_STRING(unittest_mqtt_message_to_send_list[9], "{\"REPORT\":{\"SYSTEM_UPTIME\":\"5 days, 10 hurs, 5 minutes, 6 seconds\"}}");
		UT_CHECK_IS_EQUAL(counter_CLI_COMMAND_RECEIVED, 3);
		UT_CHECK_IS_EQUAL(counter_FILE_OPEN_FAILED, 0);
		UT_CHECK_IS_BETWEEN(counter_FILE_LINE_REPORT_FILE, 24, 21);//		UT_CHECK_IS_EQUAL(counter_FILE_LINE_REPORT_FILE, 22);
		UT_CHECK_IS_EQUAL(counter_FILE_LINE_COMMAND_FILE, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_SYNTAX, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MESSAGE_TO_SEND, 10);
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

	MQTT_MESSAGE_RECEIVED_SIGNAL_init();

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
		UT_MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SLOT_connect();
		UT_MQTT_MESSAGE_TO_SEND_SLOT_connect();

		UNITTEST_msg_executer_configure();
		UNITTEST_msg_executer_message_received();
		UNITTEST_msg_executer_message_received();
		UNITTEST_msg_executer_response_timeout();
		UNITTEST_msg_executer_execution_command();
		UNITTEST_msg_executer_invalid_command();
		UNITTEST_msg_executer_invalid_command_syntax();
		UNITTEST_msg_executer_command_file_not_existing();
		UNITTEST_msg_executer_process_report_list();
		UNITTEST_msg_executer_new_message_while_processing_report();
		UNITTEST_msg_executer_bad_command_while_processing_report();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
