/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/thread_print_trace_object.c
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

#include "cpu.h"
#include <stdio.h>

// --------------------------------------------------------------------------------

#include "common/common_types.h"
#include "common/qeue_interface.h"
#include "common/local_module_status.h"
#include "common/common_tools_string.h"
#include "common/signal_slot_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/console/ui_console.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "tracer/trace_object.h"
#include "tracer/trace_parser.h"

// --------------------------------------------------------------------------------

#define PRINT_TRACE_OBJECT_MESSAGE_LINE_NOT_FOUND		"LINE NOT FOUND ! - "
#define PRINT_TRACE_OBJECT_MESSAGE_FILE_NOT_FOUND		"FILE NOT FOUND ! - "

// --------------------------------------------------------------------------------

#define PRINT_TRACE_OBJECT_STATUS_CONSOLE_ACTIVE	(1 << 0)
#define PRINT_TRACE_OBJECT_STATUS_MQTT_ACTIVE		(1 << 1)
#define PRINT_TRACE_OBJECT_STATUS_FILE_ACTIVE		(1 << 2)
#define PRINT_TRACE_OBJECT_STATUS_TERMINATED		(1 << 3)

BUILD_MODULE_STATUS_U8(PRINT_TRACE_OBJECT_STATUS)

// --------------------------------------------------------------------------------

#ifndef PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH
#define PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH			2048
#endif

#ifndef PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH
#define PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH		255
#endif

// --------------------------------------------------------------------------------

QEUE_INTERFACE_INCLUDE_QEUE(TRACE_OBJECT_QEUE)
//MQTT_INTERFACE_INCLUDE_HOST(MQTT_TRACER)
//TIME_MGMN_BUILD_TIMER(MQTT_CONNECT_TIMER)

// --------------------------------------------------------------------------------

static void print_trace_object_CLI_CONSOLE_ACTIVATED_SLOT_CALLBACK(const void* p_argument);
static void print_trace_object_CLI_ARGUMENT_FILE_SLOT_CALLBACK(const void* p_argument);
static void print_trace_object_CLI_ARGUMENT_PATH_SLOT_CALLBACK(const void* p_argument);

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_CONSOLE_ACTIVATED_SIGNAL, PRINT_TRACE_OBJECT_CLI_CONSOLE_ACTIVATED_SLOT, print_trace_object_CLI_CONSOLE_ACTIVATED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_FILE_SIGNAL, PRINT_TRACE_OBJECT_CLI_ARGUMENT_FILE_SLOT, print_trace_object_CLI_ARGUMENT_FILE_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_PATH_SIGNAL, PRINT_TRACE_OBJECT_CLI_ARGUMENT_PATH_SLOT, print_trace_object_CLI_ARGUMENT_PATH_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(TRACE_OUTPUT_FILE)
FILE_INTERFACE_CREATE_FILE(SOURCE_FILE)

// --------------------------------------------------------------------------------

static char src_base_path[PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH];

// --------------------------------------------------------------------------------

static void main_get_traceoutput_from_source_line(char* p_string_to, char* p_string_from) {

	DEBUG_PASS("main_get_traceoutput_from_source_line()");

	u16 length = common_tools_string_length(p_string_from);
	u16 i = 0;

	u16 start = 0;

	for ( ; i < length; i++) {
		if (p_string_from[i] == '\"') {
			start = i + 1;
			break;
		}
	}

	u16 end = start;

	for ( i = start + 1 ; i < length; i++) {
		if (p_string_from[i] == '\"') {
			end = i;
			break;
		}
	}

	length = end - start + 1;
	common_tools_string_copy_string(p_string_to, p_string_from + start, length);

	DEBUG_TRACE_STR(p_string_to, "main_get_traceoutput_from_source_line() - DEBUG-STRING: ");
}

/**
 * @brief 
 * 
 * @param base_path 
 * @param p_trace_obj 
 * @return u8 	0 - file found and line was read
 * 		1 - file as not found
 * 		2 - file was found but line was not read
 */
static u8 main_read_source_file_line(char* base_path, TRACE_OBJECT* p_trace_obj) {

	DEBUG_PASS("main_read_source_file_line");

	char source_file_path[PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH];

	common_tools_string_clear(source_file_path, PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);
	common_tools_string_copy_string(source_file_path, base_path, PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);
	common_tools_string_append(source_file_path, p_trace_obj->file_name, PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);

	SOURCE_FILE_set_path(source_file_path);

	if (SOURCE_FILE_open() == 0) {
		DEBUG_TRACE_STR(source_file_path, "main_read_source_file_line() - File not found !!!");
		return 1;
	}

	char trace_line[TRACE_OBJECT_SOURCE_LINE_LENGTH];
	common_tools_string_clear(trace_line, TRACE_OBJECT_SOURCE_LINE_LENGTH);

	u16 length_of_line = SOURCE_FILE_read_line(trace_line, p_trace_obj->line_number, TRACE_OBJECT_SOURCE_LINE_LENGTH);

	SOURCE_FILE_close();

	if (length_of_line == 0) {
		DEBUG_TRACE_STR(source_file_path, "main_read_source_file_line() - Line not found in file !!!");
		return 2;
	}

	main_get_traceoutput_from_source_line(p_trace_obj->source_line, trace_line);

	DEBUG_TRACE_STR(trace_line, "main_read_source_file_line() - Trace-Line");
	DEBUG_TRACE_STR(p_trace_obj->source_line, "main_read_source_file_line() - Source-Line");

	return 0;
}

static void thread_print_trace_object_get_hex_dump(char* p_string, const void *src, size_t length, size_t line_size, u16 p_string_max_length) {
	
	int i = 0;
	u16 index = 0;
	const unsigned char *address = src;
	const unsigned char *line = address;
	//char* p_string_addr = p_string;

	//sprintf(p_string_addr, "   "); p_string_addr += 3;
	common_tools_string_append(p_string, "\n ", p_string_max_length);
	common_tools_string_append_hex_number(p_string, index, 16, p_string_max_length);
	common_tools_string_append(p_string, " |  ", p_string_max_length);
	
	while (length-- > 0) {
	
		common_tools_string_append_hex_number(p_string, *address++, 8, p_string_max_length);
		common_tools_string_append(p_string, " ", p_string_max_length);
		//sprintf(p_string_addr, "%02X ", *address++); p_string_addr += 3;

		if (!(++i % line_size) || (length == 0 && i % line_size)) {

			if (length == 0) {			
				while (i++ % line_size) {

					common_tools_string_append(p_string, "__ ", p_string_max_length);
					//sprintf(p_string_addr, "__ "); p_string_addr += 3;
					
				}
			}
			
			// right close
			common_tools_string_append(p_string, " | ", p_string_max_length);
			//sprintf(p_string_addr, " | "); p_string_addr += 3; // right close 

			while (line < address) {
				
				unsigned char c = *line++;

				//sprintf(p_string_addr, "%c", (c < 33 || c == 255) ? 0x2E : c); p_string_addr += 1;
				common_tools_string_append_character(p_string, (c < 33 || c == 255) ? 0x2E : c, p_string_max_length);
			}

			//sprintf(p_string_addr, "\n"); p_string_addr += 1;

			if (length > 0 ) {

				index += line_size;

				common_tools_string_append(p_string, "\n ", p_string_max_length);
				common_tools_string_append_hex_number(p_string, index, 16, p_string_max_length);
				common_tools_string_append(p_string, " |  ", p_string_max_length); // left open

				//common_tools_string_append(p_string, "   ", p_string_max_length);
				//sprintf(p_string_addr, "   "); p_string_addr += 3;
			}
		}
	}
}

void thread_print_trace_object_prepare_output(TRACE_OBJECT* p_trace_obj, char* p_string, u16 max_length_of_trace_string) {

	// Resulting String:
	// <FILE_NAME>:<FILE_LINE> - <TRACE_OUTPUT>
	// Data: <DE_NUM> (<HEX_NUM>)

	common_tools_string_copy_string(p_string, p_trace_obj->file_name, max_length_of_trace_string);			// Path + Filename
	common_tools_string_append(p_string, ":", max_length_of_trace_string);						// ":" Seperator
	common_tools_string_append_number(p_string, (u32) p_trace_obj->line_number, max_length_of_trace_string);	// Linenumber
	common_tools_string_append(p_string, " - ", max_length_of_trace_string);					// "-" Seperator
	common_tools_string_append(p_string, p_trace_obj->source_line, max_length_of_trace_string);			// Line from Sourcefile

	switch (p_trace_obj->type) {
		default:
		case TRACE_OBJECT_TYPE_PASS:
			break;

		case TRACE_OBJECT_TYPE_BYTE:
			common_tools_string_append(p_string, "\n\t- Data: ", max_length_of_trace_string);
			common_tools_string_append_number(p_string, (u32) p_trace_obj->data.byte, max_length_of_trace_string);
			common_tools_string_append(p_string, " (", max_length_of_trace_string);
			common_tools_string_append_hex_number(p_string, (u32) p_trace_obj->data.byte, 8, max_length_of_trace_string);
			common_tools_string_append(p_string, ")", max_length_of_trace_string);
			break;

		case TRACE_OBJECT_TYPE_WORD:
			common_tools_string_append(p_string, "\n\t- Data: ", max_length_of_trace_string);
			common_tools_string_append_number(p_string, (u32) p_trace_obj->data.word, max_length_of_trace_string);
			common_tools_string_append(p_string, " (", max_length_of_trace_string);
			common_tools_string_append_hex_number(p_string, (u32) p_trace_obj->data.word, 8, max_length_of_trace_string);
			common_tools_string_append(p_string, ")", max_length_of_trace_string);
			break;

		case TRACE_OBJECT_TYPE_LONG:
			common_tools_string_append(p_string, "\n\t- Data: ", max_length_of_trace_string);
			common_tools_string_append_number(p_string, (u32) p_trace_obj->data.integer, max_length_of_trace_string);
			common_tools_string_append(p_string, " (", max_length_of_trace_string);
			common_tools_string_append_hex_number(p_string, (u32) p_trace_obj->data.integer, 8, max_length_of_trace_string);
			common_tools_string_append(p_string, ")", max_length_of_trace_string);
			break;

		case TRACE_OBJECT_TYPE_ARRAY:
			thread_print_trace_object_get_hex_dump(p_string + common_tools_string_length(p_string), (const void*) p_trace_obj->data.array, p_trace_obj->data_length, 16, max_length_of_trace_string);
			break;
	}
}

void* thread_print_trace_object_run(void* p_arg) {

	(void) p_arg;

	DEBUG_PASS("thread_print_trace_object_run() - Thread started");

	PRINT_TRACE_OBJECT_STATUS_clear_all();

	PRINT_TRACE_OBJECT_CLI_CONSOLE_ACTIVATED_SLOT_connect();
	PRINT_TRACE_OBJECT_CLI_ARGUMENT_FILE_SLOT_connect();
	PRINT_TRACE_OBJECT_CLI_ARGUMENT_PATH_SLOT_connect();

	if (PRINT_TRACE_OBJECT_STATUS_is_set(PRINT_TRACE_OBJECT_STATUS_FILE_ACTIVE)) {
		if (TRACE_OUTPUT_FILE_is_existing() == 0) {
			TRACE_OUTPUT_FILE_create();
		}
	}
	        
	TRACE_OBJECT trace_obj;

	PRINT_TRACE_OBJECT_STATUS_unset(PRINT_TRACE_OBJECT_STATUS_TERMINATED);

	while (1) {

		usleep(50000); // reduce cpu-load

		if (PRINT_TRACE_OBJECT_STATUS_is_set(PRINT_TRACE_OBJECT_STATUS_TERMINATED)) {
			DEBUG_PASS("thread_print_trace_object_run() - TERMINATE SIGNAL RECEIVED");
			break;
		}

		if (TRACE_OBJECT_QEUE_is_empty()) {
			continue;
		}

		if (TRACE_OBJECT_QEUE_mutex_get() == 0) {
			continue;
		}
			
		u8 object_available = TRACE_OBJECT_QEUE_deqeue(&trace_obj);
		TRACE_OBJECT_QEUE_mutex_release();

		if (object_available == 0) {
			continue;
		}

		char trace_line[PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH];
		common_tools_string_clear(trace_line, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);

		u8 file_stat = main_read_source_file_line(src_base_path, &trace_obj);
		if ( file_stat == 1 ) {

			// file not found
			common_tools_string_copy_string(trace_line, PRINT_TRACE_OBJECT_MESSAGE_FILE_NOT_FOUND, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);
			common_tools_string_append(trace_line, src_base_path, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);
			common_tools_string_append(trace_line, trace_obj.file_name, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);

		} else if ( file_stat == 2 ) {

			// Line not found
			common_tools_string_copy_string(trace_line, PRINT_TRACE_OBJECT_MESSAGE_LINE_NOT_FOUND, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);
			common_tools_string_append(trace_line, src_base_path, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);
			common_tools_string_append(trace_line, trace_obj.file_name, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);

		} else {
			thread_print_trace_object_prepare_output(&trace_obj, trace_line, PRINT_TRACE_OBJECT_TRACE_OUTPUT_LINE_MAX_LENGTH);
		}

		if (PRINT_TRACE_OBJECT_STATUS_is_set(PRINT_TRACE_OBJECT_STATUS_CONSOLE_ACTIVE)) {
			console_write_line(trace_line);
		}

		if (PRINT_TRACE_OBJECT_STATUS_is_set(PRINT_TRACE_OBJECT_STATUS_FILE_ACTIVE)) {
			
			TRACE_OUTPUT_FILE_open();
			TRACE_OUTPUT_FILE_append_line(trace_line);
			TRACE_OUTPUT_FILE_close();
		}

		if (PRINT_TRACE_OBJECT_STATUS_is_set(PRINT_TRACE_OBJECT_STATUS_MQTT_ACTIVE)) {
			//MQTT_TRACER_send_message();

			//while ( !MQTT_TRACER_delivery_complete() ) { usleep(50000); };
		}
	}

	DEBUG_PASS("thread_print_trace_object_run() - THREAD FINISHED");

	return NULL;
}

void thread_print_trace_object_terminate(void) {
	DEBUG_PASS("thread_print_trace_object_terminate()");
	PRINT_TRACE_OBJECT_STATUS_set(PRINT_TRACE_OBJECT_STATUS_TERMINATED);
}

// ------------------------------------------------------------------------------

static void print_trace_object_CLI_CONSOLE_ACTIVATED_SLOT_CALLBACK(const void* p_argument) {

	DEBUG_PASS("print_trace_object_CLI_CONSOLE_ACTIVATED_SLOT_CALLBACK()");
	PRINT_TRACE_OBJECT_STATUS_set(PRINT_TRACE_OBJECT_STATUS_CONSOLE_ACTIVE);
}


static void print_trace_object_CLI_ARGUMENT_FILE_SLOT_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("print_trace_object_CLI_ARGUMENT_FILE_SLOT_CALLBACK() - NULL_POINTER_EXCEPTION");
		return;
	}

	DEBUG_PASS("print_trace_object_CLI_CONSOLE_ACTIVATED_SLOT_CALLBACK()");
	PRINT_TRACE_OBJECT_STATUS_set(PRINT_TRACE_OBJECT_STATUS_FILE_ACTIVE);

	const char* p_argument_str = (const char*)p_argument;

	TRACE_OUTPUT_FILE_set_path(p_argument_str);
}

static void print_trace_object_CLI_ARGUMENT_PATH_SLOT_CALLBACK(const void* p_argument) {
	
	if (p_argument == NULL) {
		DEBUG_PASS("print_trace_object_CLI_ARGUMENT_PATH_SLOT_CALLBACK() - NULL_POINTER_EXCEPTION");
		return;
	}

	const char* p_argument_str = (const char*)p_argument;

	common_tools_string_clear(src_base_path, PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);
	common_tools_string_copy_string( src_base_path, p_argument_str, PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);

	if (common_tools_string_ends_with(src_base_path, '/') == 0) {
		common_tools_string_append(src_base_path, "/", PRINT_TRACE_OBJECT_TRACE_OUTPUT_FILE_NAME_MAX_LENGTH);
	}

	DEBUG_TRACE_STR(src_base_path, "print_trace_object_CLI_ARGUMENT_PATH_SLOT_CALLBACK");
}

// ------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(PRINT_TRACE_OBJECT_THREAD, THREAD_PRIORITY_MIDDLE, thread_print_trace_object_run, thread_print_trace_object_terminate)
