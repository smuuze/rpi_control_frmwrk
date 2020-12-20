/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ui/cfg_file_parser/cfg_file_parser.c
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

#include "mcu_task_management/mcu_task_interface.h"
#include "common/local_module_status.h"
#include "common/signal_slot_interface.h"
#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

#include "ui/file_interface/file_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

// --------------------------------------------------------------------------------

#ifndef CFG_FILE_PARSER_MAX_LINE_LENGTH
#define CFG_FILE_PARSER_MAX_LINE_LENGTH			255
#endif

#ifndef CFG_FILE_PARSER_KEY_VALUE_SPLITTER
#define CFG_FILE_PARSER_KEY_VALUE_SPLITTER		'='
#endif

#ifndef CFG_FILE_PARSER_COMMENT_CHARACTER
#define CFG_FILE_PARSER_COMMENT_CHARACTER		'#'
#endif

// --------------------------------------------------------------------------------

#define CFG_FILE_PARSER_NEW_CFG_FILE_SET		(1 << 0)

BUILD_MODULE_STATUS_U8(CFG_FILE_PARSER_STATUS)

// --------------------------------------------------------------------------------

typedef enum {
	CFG_FILE_PARSER_TASK_STATE_IDLE,
	CFG_FILE_PARSER_TASK_STATE_OPEN_FILE,
	CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION
} CFG_FILE_PARSER_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/*
 *
 */
static void cfg_file_parser_CFG_FILE_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_CONFIGURATION_SIGNAL, CFG_PARSER_CFG_FILE_SLOT, cfg_file_parser_CFG_FILE_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(CFG_FILE)

// --------------------------------------------------------------------------------

static MCU_TASK_INTERFACE cfg_file_parser_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&cfg_file_parser_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&cfg_file_parser_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&cfg_file_parser_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK	get_sate,
	&cfg_file_parser_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&cfg_file_parser_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&cfg_file_parser_task_terminate, 			// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};

// --------------------------------------------------------------------------------

/*
 *
 */
static char cfg_file_path[CFG_FILE_PARSER_MAX_LINE_LENGTH];

/*
 *
 */
static CFG_FILE_PARSER_TASK_STATE_TYPE app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;

// --------------------------------------------------------------------------------

static void cfg_file_parser_parse_configuration_line(char* line) {

	DEBUG_TRACE_STR(line, "cfg_file_parser_parse_configuration_line()");

	if (strlen(line) == 0) {
		DEBUG_PASS("cfg_file_parser_parse_configuration_line() - Length of line is zero");
		return;
	}

	if (line[0] == CFG_FILE_PARSER_COMMENT_CHARACTER) {
		DEBUG_PASS("cfg_file_parser_parse_configuration_line() - This is a comment-line");
		return;
	}

	if (common_tools_string_get_char_count(CFG_FILE_PARSER_KEY_VALUE_SPLITTER, line) != 1) {
		DEBUG_PASS("cfg_file_parser_parse_configuration_line() - not a configuration line");
		return;
	}

	CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_object;

	common_tools_string_split(CFG_FILE_PARSER_KEY_VALUE_SPLITTER, line, cfg_object.key, CFG_FILE_PARSER_MAX_LINE_LENGTH, cfg_object.value, CFG_FILE_PARSER_MAX_LINE_LENGTH);

	if (strlen(cfg_object.key) == 0) {
		DEBUG_PASS("cfg_file_parser_parse_configuration_line() - no key in line");
		return;
	}

	if (strlen(cfg_object.value) == 0) {
		DEBUG_PASS("cfg_file_parser_parse_configuration_line() - no value in line");
		return;
	}

	DEBUG_TRACE_STR(cfg_object.key, "cfg_file_parser_parse_configuration_line() - cfg-key: ");
	DEBUG_TRACE_STR(cfg_object.value, "cfg_file_parser_parse_configuration_line() - cfg-value: ");

	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&cfg_object);
}

static void cfg_file_parser_parse_configuration_file(void) {

	DEBUG_PASS("cfg_file_parser_parse_configuration_file()");

	i16 num_bytes_line = 0;
	char line[CFG_FILE_PARSER_MAX_LINE_LENGTH];	

	do {
		num_bytes_line = CFG_FILE_read_next_line(line, CFG_FILE_PARSER_MAX_LINE_LENGTH);

		if (num_bytes_line > 0) {

			common_tools_string_trim(line);
			cfg_file_parser_parse_configuration_line(line);

		}

	} while (num_bytes_line != -1);

	CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);
}

// --------------------------------------------------------------------------------

void cfg_file_parser_init(void) {
	
	DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init()");
	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_set_timeout(0);

	DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_CFG_COMPLETE_SIGNAL_init()");
	CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
	CFG_PARSER_CFG_COMPLETE_SIGNAL_set_timeout(0);

	DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_CFG_FILE_SLOT_connect()");
	CFG_PARSER_CFG_FILE_SLOT_connect();

	DEBUG_PASS("cfg_file_parser_init() - mcu_task_controller_register_task()");
	mcu_task_controller_register_task(&cfg_file_parser_task);
}

void cfg_file_parser_task_init(void) {
	
	DEBUG_PASS("cfg_file_parser_task_init()");
	app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;
}

u16 cfg_file_parser_task_get_schedule_interval(void) {

	if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET)) {
		return 0;
	} else {
		return CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS;
	}
}

MCU_TASK_INTERFACE_TASK_STATE cfg_file_parser_task_get_state(void) {

	if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET)) {
		DEBUG_PASS("cfg_file_parser_task_get_state() - New file has been set");
		return MCU_TASK_RUNNING;
	}
	
	return MCU_TASK_SLEEPING;
}

void cfg_file_parser_task_run(void) {
	
	DEBUG_PASS("cfg_file_parser_task_run()");

	switch (app_task_state) {
		
		default:

			app_task_state = CFG_FILE_PARSER_TASK_STATE_OPEN_FILE;
			// no break;

		case CFG_FILE_PARSER_TASK_STATE_IDLE :

			if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET) == 0) {
				break;
			}

			CFG_FILE_PARSER_STATUS_unset(CFG_FILE_PARSER_NEW_CFG_FILE_SET);
			DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_IDLE > CFG_FILE_PARSER_TASK_STATE_OPEN_FILE");
			app_task_state = CFG_FILE_PARSER_TASK_STATE_OPEN_FILE;
			// no break;

		case CFG_FILE_PARSER_TASK_STATE_OPEN_FILE :

			if (CFG_FILE_open_path(cfg_file_path) == 0) {
				DEBUG_PASS("cfg_file_parser_task_run() - Open new cfg-file has FAILED !!! ---");
				app_task_state = CFG_FILE_PARSER_TASK_STATE_OPEN_FILE;
				break;
			}

			DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_OPEN_FILE > CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION");

			app_task_state = CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION;
			// no break;

		case CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION :

			cfg_file_parser_parse_configuration_file();

			DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION > CFG_FILE_PARSER_TASK_STATE_IDLE");
			app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;

			break;
	}

}

void cfg_file_parser_task_background_run(void) {

}

void cfg_file_parser_task_sleep(void) {

}

void cfg_file_parser_task_wakeup(void) {

}

void cfg_file_parser_task_finish(void) {

}

void cfg_file_parser_task_terminate(void) {

}

// --------------------------------------------------------------------------------

static void cfg_file_parser_CFG_FILE_SLOT_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - NULL_POINTER_EXEPTION");
		return;
	}

	const char* file_path = (const char*)p_argument;

	DEBUG_TRACE_STR(file_path, "cfg_file_parser_CFG_FILE_SLOT_CALLBACK()");

	if (strlen(file_path) > CFG_FILE_PARSER_MAX_LENGTH_PATH) {
		DEBUG_TRACE_byte(strlen(file_path), "cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - Path-Length too long");
		return;
	}

	if (strlen(file_path) == 0) {
		DEBUG_TRACE_byte(strlen(file_path), "cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - Path-Length is zero");
		return;
	}

	memset(cfg_file_path, '\0', CFG_FILE_PARSER_MAX_LINE_LENGTH);
	memcpy((u8*)&cfg_file_path, (u8*)file_path, strlen(file_path));

	CFG_FILE_PARSER_STATUS_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET);
}
