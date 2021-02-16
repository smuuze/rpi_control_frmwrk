/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/message_executer_task.c
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
#include "common/common_types.h"
#include "time_management/time_management.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/file_interface/file_interface.h"
#include "protocol_management/json/protocol_json_parser.h"

// --------------------------------------------------------------------------------

#include "protocol_management/mqtt/mqtt_interface.h"

// --------------------------------------------------------------------------------

#include "app_tasks/message_executer_task.h"

// --------------------------------------------------------------------------------

// must be more than the timeout of com / exe module !
#ifndef MSG_EXECUTER_RESPONSE_TIMEOUT_MS
#define MSG_EXECUTER_RESPONSE_TIMEOUT_MS				600
#endif

#ifndef MSG_EXECUTER_MAX_MESSAGE_LENGTH
#define MSG_EXECUTER_MAX_MESSAGE_LENGTH					255
#endif

#ifndef MSG_EXECUTER_MAX_FILE_LINE_LENGTH
#define MSG_EXECUTER_MAX_FILE_LINE_LENGTH				255
#endif

#ifndef MSG_EXECUTER_KEY_VALUE_SPLITTER
#define MSG_EXECUTER_KEY_VALUE_SPLITTER					'='
#endif

#ifndef MSG_EXECUTER_COMMAND_TYPE_SPLITTER
#define MSG_EXECUTER_COMMAND_TYPE_SPLITTER				':'
#endif

#ifndef MSG_EXECUTER_COMMENT_CHARACTER
#define MSG_EXECUTER_COMMENT_CHARACTER					'#'
#endif

#ifndef MSG_EXECUTER_COMMAND_TYPE_EXE_STR
#define MSG_EXECUTER_COMMAND_TYPE_EXE_STR				"exe"
#endif

#ifndef MSG_EXECUTER_COMMAND_TYPE_COM_STR
#define MSG_EXECUTER_COMMAND_TYPE_COM_STR				"com"
#endif

#ifndef MSG_EXECUTER_REPORT_JSON_GROUP_STRING
#define MSG_EXECUTER_REPORT_JSON_GROUP_STRING				"REPORT"
#endif

#ifndef MSG_EXECUTER_RESPONSE_JSON_GROUP_STRING
#define MSG_EXECUTER_RESPONSE_JSON_GROUP_STRING				"RESPONSE"
#endif

#ifndef MSG_EXECUTER_DEFAULT_REPORT_INTERVAL_MS
#define MSG_EXECUTER_DEFAULT_REPORT_INTERVAL_MS				300000
#endif

#ifndef MSG_EXECUTER_DEFAULT_REPORT_FILE_PATH
#define MSG_EXECUTER_DEFAULT_REPORT_FILE_PATH				"default_report_file.txt"
#endif

#ifndef MSG_EXECUTER_DEFAULT_COMMAND_FILE_PATH
#define MSG_EXECUTER_DEFAULT_COMMAND_FILE_PATH				"default_command_file.txt"
#endif

// --------------------------------------------------------------------------------

typedef enum {

	MSG_EXECUTER_TASK_STATE_IDLE,
	MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG,
	MSG_EXECUTER_TASK_STATE_PARSE_MSG,
	MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE,
	MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG,
	MSG_EXECUTER_TASK_STATE_PREPARE_COM,
	MSG_EXECUTER_TASK_STATE_PREPARE_EXE,
	MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE,
	MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE,
	MSG_EXECUTER_TASK_STATE_SEND_RESPONSE,
	MSG_EXECUTER_TASK_STATE_START_REPORT,
	MSG_EXECUTER_PROCESS_REPORT_LIST,
	MSG_EXECUTER_TASK_TERMINATED,
	MSG_EXECUTER_TASK_STATE_SEND_REPORT,
	MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT,
	MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND
} MSG_EXECUTER_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

// TASK INTERFACE

/*!
 *
 */
static void msg_executer_task_init(void);

/*!
 *
 */
static u16 msg_executer_task_get_schedule_interval(void);

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE msg_executer_task_get_state(void);

/*!
 *
 */
static void msg_executer_task_run(void);

/*!
 *
 */
static void msg_executer_task_finish(void);

/*!
 *
 */
static void msg_executer_task_terminate(void);

// --------------------------------------------------------------------------------

// SLOT CALLBACKS

/*!
 *
 */
static void msg_executer_cfg_object_CALLBACK(const void* p_argument);

/*!
 *
 */
static void msg_executer_cfg_complete_CALLBACK(const void* p_argument);

/*!
 *
 */
static void msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK(const void* p_argument);

/*!
 *
 */
static void msg_executer_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void msg_executer_RPI_HOST_RESPONSE_TIMEOUT_SLOT_CALLBACK(const void* p_argument);

/*!
 *
 */
static void msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK(const void* p_argument);

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void msg_executer_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_INVALID_COMMAND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_EXECUTION_SUCCEEDED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MSG_EXECUTER_EXECUTION_FAILED_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, MSG_EXECUTER_NEW_CFG_OBJECT_SLOT, msg_executer_cfg_object_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_CFG_COMPLETE_SIGNAL, MSG_EXECUTER_CFG_COMPLETE_SLOT, msg_executer_cfg_complete_CALLBACK)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_TO_SEND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_MESSAGE_RECEIVED_SIGNAL, MSG_EXECUTER_MQTT_MSG_RECEIVED_SLOT, msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_RECEIVED_SIGNAL, MSG_EXECUTER_RPI_HOST_RESPONSE_RECEIVED_SLOT, msg_executer_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL, MSG_EXECUTER_RPI_HOST_RESPONSE_TIMEOUT_SLOT, msg_executer_RPI_HOST_RESPONSE_TIMEOUT_SLOT_CALLBACK)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL, MSG_EXECUTER_CLI_EXECUTER_COMMAND_RESPONSE_SLOT, msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL, MSG_EXECUTER_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT, msg_executer_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

#define MSG_EXECUTER_STATUS_USER_CFG_SET			(1 << 0)
#define MSG_EXECUTER_STATUS_MSG_RECEIVED			(1 << 1)
#define MSG_EXECUTER_STATUS_RESPONSE_RECEIVED			(1 << 2)
#define MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING		(1 << 3)
#define MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING		(1 << 4)
#define MSG_EXECUTER_STATUS_REPORT_ACTIVE			(1 << 5)
#define MSG_EXECUTER_STATUS_RESPONSE_TIMEOUT			(1 << 6)
#define MSG_EXECUTER_STATUS_COMMAND_NOT_FOUND			(1 << 7)

BUILD_MODULE_STATUS_U8(MSG_EXECUTER_STATUS)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U32(MSG_EXECUTER_REPORT_INTERVAL_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(MSG_EXECUTER_OP_TIMER)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(COMMAND_FILE)
FILE_INTERFACE_CREATE_FILE(REPORT_FILE)

// --------------------------------------------------------------------------------

JSON_PARSER_CREATE_OBJECT(RESPONSE_JSON_OBJECT)
JSON_PARSER_CREATE_OBJECT(REPORT_JSON_OBJECT)

// --------------------------------------------------------------------------------

/*!
 *
 */
static MCU_TASK_INTERFACE msg_executer_task = {

	0, 						// 	identifier,
	0, 						// 	new_run_timeout,
	0, 						// 	last_run_time,
	&msg_executer_task_init, 			// 	init,
	&msg_executer_task_get_schedule_interval,	// 	get_schedule_interval,
	&msg_executer_task_get_state, 			// 	get_sate,
	&msg_executer_task_run, 			// 	run,
	0,						// 	background_run,
	0, 						// 	sleep,
	0, 						// 	wakeup,
	&msg_executer_task_finish, 			// 	finish,
	&msg_executer_task_terminate, 			// 	terminate,
	0						// 	next-task
};

/*!
 *
 */
static MSG_EXECUTER_TASK_STATE_TYPE msg_executer_task_state = MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG;

/**
 * @brief Name of command received via MQTT. Module will look for this name in Command-/Report-File
 * 
 */
static char msg_executer_received_command_name[MSG_EXECUTER_MAX_MESSAGE_LENGTH];

/*!
 *
 */
static char msg_executer_pending_command[MSG_EXECUTER_MAX_MESSAGE_LENGTH];

/*!
 *
 */
static char pending_response_cli_command[MSG_EXECUTER_MAX_MESSAGE_LENGTH];

/*!
 *
 */
static char msg_executer_pending_report_name[MSG_EXECUTER_MAX_MESSAGE_LENGTH];

/*!
 *
 */
static u32 msg_executer_report_interval_timeout_ms = 0;

/**
 * @brief holds the lasst response of a rpi-command
 * 
 */
static COMMON_GENERIC_BUFFER_TYPE pending_response_rpi_command;

// --------------------------------------------------------------------------------

/*!
 *
 */
static void msg_executer_parse_command_message(const char* p_command_msg, char* p_command_data);

/*!
 *
 */
static u8 msg_executer_parse_communication_command(const char* p_com_command);

/*!
 *
 */
static u8 msg_executer_parse_execution_command(const char* p_exe_command);

/*!
 *
 */
static i8 msg_executer_parse_report_command(char* p_report_name, char* p_command_data);

/*!
 * 
 */
static void msg_executer_prepare_report(char* p_response_message, char* p_response_buffer, char* p_report_name, u16 max_response_length);

// --------------------------------------------------------------------------------

void msg_executer_init(void) {
	
	DEBUG_PASS("msg_executer_init()");

	MSG_EXECUTER_STATUS_clear_all();

	DEBUG_PASS("msg_executer_init() - Initialize signals");

	MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL_init();
	MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL_init();
	MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL_init();
	MSG_EXECUTER_EXECUTION_SUCCEEDED_SIGNAL_init();
	MSG_EXECUTER_EXECUTION_FAILED_SIGNAL_init();
	MSG_EXECUTER_INVALID_COMMAND_SIGNAL_init();
	MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SIGNAL_init();

	DEBUG_PASS("msg_executer_init() - Connect slots");

	MSG_EXECUTER_NEW_CFG_OBJECT_SLOT_connect();
	MSG_EXECUTER_CFG_COMPLETE_SLOT_connect();
	MSG_EXECUTER_MQTT_MSG_RECEIVED_SLOT_connect();

	MSG_EXECUTER_RPI_HOST_RESPONSE_RECEIVED_SLOT_connect();
	MSG_EXECUTER_RPI_HOST_RESPONSE_TIMEOUT_SLOT_connect();

	MSG_EXECUTER_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_connect();
	MSG_EXECUTER_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_connect();

	DEBUG_PASS("cfg_file_parser_init() - Register task");

	REPORT_FILE_set_path(MSG_EXECUTER_DEFAULT_REPORT_FILE_PATH);
	COMMAND_FILE_set_path(MSG_EXECUTER_DEFAULT_COMMAND_FILE_PATH);

	msg_executer_report_interval_timeout_ms = MSG_EXECUTER_DEFAULT_REPORT_INTERVAL_MS;

	mcu_task_controller_register_task(&msg_executer_task);
}

// --------------------------------------------------------------------------------

/*!
 *
 */
static void msg_executer_task_init(void) {
	
	DEBUG_PASS("msg_executer_task_init()");
	msg_executer_task_state = MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG;
	MSG_EXECUTER_REPORT_INTERVAL_TIMER_start();
}

/*!
 *
 */
static u16 msg_executer_task_get_schedule_interval(void) {
	
	//DEBUG_PASS("msg_executer_task_get_schedule_interval()");
	return MSG_EXECUTER_TASK_SCHEDULE_INTERVAL_MS;
}

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE msg_executer_task_get_state(void) {

	if (msg_executer_task_state == MSG_EXECUTER_TASK_TERMINATED) {
		return MCU_TASK_SLEEPING;
	}
	
	if (msg_executer_task_state != MSG_EXECUTER_TASK_STATE_IDLE) {
		//DEBUG_PASS("msg_executer_task_get_state() - RUNNING");
		return MCU_TASK_RUNNING;
	}

	if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_MSG_RECEIVED)) {
		DEBUG_PASS("msg_executer_task_get_state() - MESSAGE RECEIVED");
		return MCU_TASK_RUNNING;
	}

	if (MSG_EXECUTER_REPORT_INTERVAL_TIMER_is_up(msg_executer_report_interval_timeout_ms)) {
		DEBUG_PASS("msg_executer_task_get_state() - REPORT-INTERVAL TIMEOUT");
		return MCU_TASK_RUNNING;
	}

	return MCU_TASK_SLEEPING;
}

/*!
 *
 */
static void msg_executer_task_run(void) {

	switch (msg_executer_task_state) {

		default:

			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG;
			// no break;

		case MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG :

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_USER_CFG_SET) == 0) {
				break;
			}
	
			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_USER_CFG >> MSG_EXECUTER_TASK_STATE_IDLE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;

			// no break;

		case MSG_EXECUTER_TASK_STATE_IDLE :

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_MSG_RECEIVED)) {

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_MSG_RECEIVED | MSG_EXECUTER_STATUS_REPORT_ACTIVE | MSG_EXECUTER_STATUS_RESPONSE_RECEIVED);

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_IDLE >> MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE;
				break;
			}

			if (MSG_EXECUTER_REPORT_INTERVAL_TIMER_is_up(msg_executer_report_interval_timeout_ms)) {

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED);

				if (REPORT_JSON_OBJECT_is_active() == 0) {
					DEBUG_PASS("msg_executer_task_run() - Activating Json-Report-Object");
					REPORT_JSON_OBJECT_initialize();
					REPORT_JSON_OBJECT_start_group(MSG_EXECUTER_REPORT_JSON_GROUP_STRING);
				}

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_IDLE >> MSG_EXECUTER_PROCESS_REPORT_LIST");
				msg_executer_task_state = MSG_EXECUTER_PROCESS_REPORT_LIST;
				break;
			}

			break;

		case MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE :

			if ( ! COMMAND_FILE_open() ) {

				MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL_send(COMMAND_FILE_get_path());

				DEBUG_PASS("msg_executer_task_run() - Open command-file has FAILED !!! ---");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE >> MSG_EXECUTER_TASK_STATE_IDLE");
				
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
				break;
			}

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_OPEN_COMMAND_FILE >> MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG;
			break;

		case MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG :
			
			msg_executer_parse_command_message(msg_executer_received_command_name, msg_executer_pending_command);

			COMMAND_FILE_close();

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING)) {
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG >> MSG_EXECUTER_TASK_STATE_PREPARE_COM");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_COM;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING)) {
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG >> MSG_EXECUTER_TASK_STATE_PREPARE_EXE");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_EXE;
				break;
			}

			DEBUG_PASS("msg_executer_task_run() - command not found!");
			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PARSE_COMMAND_MSG >> MSG_EXECUTER_TASK_STATE_IDLE");

			MSG_EXECUTER_INVALID_COMMAND_SIGNAL_send(msg_executer_received_command_name);
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
			break;

		case MSG_EXECUTER_TASK_STATE_PREPARE_COM :

			if ( ! msg_executer_parse_communication_command(msg_executer_pending_command)) {
				DEBUG_PASS("msg_executer_task_run() - Invalid Command!");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_COM >> MSG_EXECUTER_TASK_STATE_IDLE");

				MSG_EXECUTER_INVALID_COMMAND_SIGNAL_send(NULL);

				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
				break;
			}

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_COM >> MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE");

			MSG_EXECUTER_OP_TIMER_start();
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE;

			break;

		case MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE :

			if (MSG_EXECUTER_OP_TIMER_is_up(MSG_EXECUTER_RESPONSE_TIMEOUT_MS)) {

				DEBUG_PASS("msg_executer_task_run() - Timeout on waiting for communication response");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE >> MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT");

				MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL_send(msg_executer_pending_command);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_RESPONSE_TIMEOUT)) {

				DEBUG_PASS("msg_executer_task_run() - Response-Timeout received");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE >> MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_RESPONSE_TIMEOUT);
				MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL_send(msg_executer_pending_command);
				
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED) == 0) {
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_REPORT_ACTIVE)) {

				REPORT_JSON_OBJECT_add_rpi_response(&pending_response_rpi_command);

				//DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE >> MSG_EXECUTER_TASK_STATE_IDLE");
				//msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE >> MSG_EXECUTER_TASK_STATE_SEND_REPORT");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_REPORT;

				break;
			}
			
			RESPONSE_JSON_OBJECT_initialize();
			RESPONSE_JSON_OBJECT_start_group(MSG_EXECUTER_RESPONSE_JSON_GROUP_STRING);
			RESPONSE_JSON_OBJECT_add_rpi_response(&pending_response_rpi_command);
			RESPONSE_JSON_OBJECT_finish();

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE >> MSG_EXECUTER_TASK_STATE_SEND_RESPONSE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_RESPONSE;

			break;

		case MSG_EXECUTER_TASK_STATE_PREPARE_EXE :

			if ( ! msg_executer_parse_execution_command(msg_executer_pending_command)) {

				DEBUG_PASS("msg_executer_task_run() - Invalid Command!");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_EXE >> MSG_EXECUTER_TASK_STATE_IDLE");

				MSG_EXECUTER_INVALID_COMMAND_SIGNAL_send(msg_executer_pending_command);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
				break;
			}

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_EXE >> MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE");

			MSG_EXECUTER_OP_TIMER_start();
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE;

			break;

		case MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE :

			if (MSG_EXECUTER_OP_TIMER_is_up(MSG_EXECUTER_RESPONSE_TIMEOUT_MS)) {

				DEBUG_PASS("msg_executer_task_run() - Timeout on waiting for execution response");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE >> MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT");

				MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL_send(msg_executer_pending_command);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_COMMAND_NOT_FOUND)) {
				
				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_COMMAND_NOT_FOUND);

				DEBUG_PASS("msg_executer_task_run() - Command was not found!");
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE >> MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND");

				MSG_EXECUTER_INVALID_COMMAND_SIGNAL_send(msg_executer_pending_command);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED) == 0) {
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_REPORT_ACTIVE)) {
		
				msg_executer_prepare_report(pending_response_cli_command, pending_response_cli_command, msg_executer_pending_report_name, MSG_EXECUTER_MAX_MESSAGE_LENGTH);

				DEBUG_PASS("msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK() - Adding response to json-object --- --- --- ---");
				REPORT_JSON_OBJECT_add_cli_response(pending_response_cli_command);

				//DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE >> MSG_EXECUTER_TASK_STATE_IDLE");
				//msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE >> MSG_EXECUTER_TASK_STATE_SEND_REPORT");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_REPORT;

				break;
			}

			msg_executer_prepare_report(pending_response_cli_command, pending_response_cli_command, msg_executer_received_command_name, MSG_EXECUTER_MAX_MESSAGE_LENGTH);
			
			RESPONSE_JSON_OBJECT_initialize();
			RESPONSE_JSON_OBJECT_start_group(MSG_EXECUTER_RESPONSE_JSON_GROUP_STRING);
			RESPONSE_JSON_OBJECT_add_cli_response(pending_response_cli_command);
			RESPONSE_JSON_OBJECT_finish();

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE >> MSG_EXECUTER_TASK_STATE_SEND_RESPONSE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_RESPONSE;
			break;

		case MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT :

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_REPORT_ACTIVE)) {

				REPORT_JSON_OBJECT_start_group(msg_executer_pending_report_name);
				REPORT_JSON_OBJECT_add_string("ERR", "TIMEOUT");

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT >> MSG_EXECUTER_TASK_STATE_SEND_REPORT");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_REPORT;
				
				break;
			}

			RESPONSE_JSON_OBJECT_initialize();
			RESPONSE_JSON_OBJECT_start_group(MSG_EXECUTER_RESPONSE_JSON_GROUP_STRING);
			RESPONSE_JSON_OBJECT_start_group(msg_executer_received_command_name);
			RESPONSE_JSON_OBJECT_add_string("ERR", "TIMEOUT");
			RESPONSE_JSON_OBJECT_finish();

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_TIMEOUT >> MSG_EXECUTER_TASK_STATE_SEND_RESPONSE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_RESPONSE;

			break;

		case MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND :

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_REPORT_ACTIVE)) {

				REPORT_JSON_OBJECT_start_group(msg_executer_pending_report_name);
				REPORT_JSON_OBJECT_add_string("ERR", "BAD_COMMAND");

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND >> MSG_EXECUTER_TASK_STATE_SEND_REPORT");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_REPORT;
				
				break;
			}

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_PREPARE_RESPONSE_COMMAND_NOT_FOUND >> MSG_EXECUTER_TASK_STATE_IDLE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
			break;

		case MSG_EXECUTER_TASK_STATE_SEND_RESPONSE :

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_SEND_RESPONSE >> MSG_EXECUTER_TASK_STATE_IDLE");

			MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED);
			MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL_send(RESPONSE_JSON_OBJECT_to_string());
			MQTT_MESSAGE_TO_SEND_SIGNAL_send(RESPONSE_JSON_OBJECT_to_string());

			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;
			break;

		case MSG_EXECUTER_PROCESS_REPORT_LIST :

			if (msg_executer_parse_report_command(msg_executer_pending_report_name, msg_executer_pending_command) < 0) {

				DEBUG_PASS("msg_executer_task_run() - Report-List complete");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_REPORT_ACTIVE);
				MSG_EXECUTER_REPORT_INTERVAL_TIMER_start();

				//DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_PROCESS_REPORT_LIST >> MSG_EXECUTER_TASK_STATE_SEND_REPORT");
				//msg_executer_task_state = MSG_EXECUTER_TASK_STATE_SEND_REPORT;

				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_PROCESS_REPORT_LIST >> MSG_EXECUTER_TASK_STATE_IDLE");
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;

				break;
			}

			// status may was unset because of new command-message
			MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_REPORT_ACTIVE);

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING)) {
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_PROCESS_REPORT_LIST >> MSG_EXECUTER_TASK_STATE_PREPARE_COM");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_COM;
				break;
			}

			if (MSG_EXECUTER_STATUS_is_set(MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING)) {
				DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_PROCESS_REPORT_LIST >> MSG_EXECUTER_TASK_STATE_PREPARE_EXE");

				MSG_EXECUTER_STATUS_unset(MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING);
				msg_executer_task_state = MSG_EXECUTER_TASK_STATE_PREPARE_EXE;
				break;
			}

			break;

		case MSG_EXECUTER_TASK_STATE_SEND_REPORT :

			REPORT_JSON_OBJECT_finish();

			MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL_send(REPORT_JSON_OBJECT_to_string());
			MQTT_MESSAGE_TO_SEND_SIGNAL_send(REPORT_JSON_OBJECT_to_string());

			DEBUG_PASS("msg_executer_task_run() - MSG_EXECUTER_TASK_STATE_SEND_REPORT >> MSG_EXECUTER_TASK_STATE_IDLE");
			msg_executer_task_state = MSG_EXECUTER_TASK_STATE_IDLE;

			break;

		case MSG_EXECUTER_TASK_TERMINATED :

			break;
	}
}

/*!
 *
 */
static void msg_executer_task_finish(void) {

}

/*!
 *
 */
static void msg_executer_task_terminate(void) {
	msg_executer_task_state = MSG_EXECUTER_TASK_TERMINATED;
}

// --------------------------------------------------------------------------------

static u8 msg_executer_parse_execution_command(const char* p_exe_command) {

	// get communication command from line
	char command_string[MSG_EXECUTER_MAX_FILE_LINE_LENGTH];

	common_tools_string_split(MSG_EXECUTER_COMMAND_TYPE_SPLITTER, p_exe_command, NULL, 0, command_string, MSG_EXECUTER_MAX_FILE_LINE_LENGTH);

	if (strlen(command_string) == 0) {
		DEBUG_PASS("msg_executer_parse_communication_command() - No command-data available");
		return 0;
	}

	DEBUG_TRACE_STR(command_string, "msg_executer_parse_communication_command() - Execution-Command: ");

	CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_send((const void*)command_string);
	return 1;
}

static u8 msg_executer_parse_communication_command(const char* p_com_command) {

	// get communication command from line
	char command_hex_string[MSG_EXECUTER_MAX_FILE_LINE_LENGTH];

	common_tools_string_split(MSG_EXECUTER_COMMAND_TYPE_SPLITTER, p_com_command, NULL, 0, command_hex_string, MSG_EXECUTER_MAX_FILE_LINE_LENGTH);

	if (strlen(command_hex_string) == 0) {
		DEBUG_PASS("msg_executer_parse_communication_command() - No command-data available");
		return 0;
	}

	DEBUG_TRACE_STR(command_hex_string, "msg_executer_parse_communication_command() - Communication-Command: ");

	COMMON_GENERIC_BUFFER_TYPE communication_buffer;
	communication_buffer.length = common_tools_hex_string_to_byte_array(command_hex_string, strlen(command_hex_string), communication_buffer.data, COMMON_TYPES_GENERIC_BUFFER_SIZE);

	RPI_HOST_COMMAND_RECEIVED_SIGNAL_send(&communication_buffer);

	return 1;
}

static u8 msg_executer_parse_line(char* line, const char* p_command_msg, char* p_command_data) {

	DEBUG_TRACE_STR(line, "msg_executer_parse_line()");

	if (strlen(line) == 0) {
		DEBUG_PASS("msg_executer_parse_line() - Length of line is zero");
		return 0;
	}

	if (line[0] == MSG_EXECUTER_COMMENT_CHARACTER) {
		DEBUG_PASS("msg_executer_parse_line() - This is a comment-line");
		return 0;
	}

	if (common_tools_string_get_char_count(MSG_EXECUTER_KEY_VALUE_SPLITTER, line) != 1) {
		DEBUG_PASS("msg_executer_parse_line() - Invalid number of message / command splitter");
		return 0;
	}

	if (common_tools_string_get_char_count(MSG_EXECUTER_COMMAND_TYPE_SPLITTER, line) != 1) {
		DEBUG_PASS("msg_executer_parse_line() - Invalid number of command-type / data splitter");
		return 0;
	}

	char command_str[MSG_EXECUTER_MAX_FILE_LINE_LENGTH];

	common_tools_string_split(MSG_EXECUTER_KEY_VALUE_SPLITTER, line, command_str, MSG_EXECUTER_MAX_MESSAGE_LENGTH, p_command_data, MSG_EXECUTER_MAX_MESSAGE_LENGTH);

	if (strlen(command_str) == 0) {
		DEBUG_PASS("msg_executer_parse_line() - no key in line");
		return 0;
	}

	if (strlen(p_command_data) == 0) {
		DEBUG_PASS("msg_executer_parse_line() - no value in line");
		return 0;
	}

	DEBUG_TRACE_STR(command_str, "msg_executer_parse_line() - command_name: ");
	DEBUG_TRACE_STR(p_command_data, "msg_executer_parse_line() - command_data: ");

	if (p_command_msg != NULL) {

		if (strlen(command_str) != strlen(p_command_msg)) {
			DEBUG_PASS("msg_executer_parse_line() - Length of command does not match");
			return 0;
		}

		if (memcmp(p_command_msg, command_str, strlen(p_command_msg)) != 0) {
			DEBUG_PASS("msg_executer_parse_line() - Line does not match command");
			return 0;
		}
	}

	if (memcmp(p_command_data, MSG_EXECUTER_COMMAND_TYPE_EXE_STR, strlen(MSG_EXECUTER_COMMAND_TYPE_EXE_STR)) == 0) {

		DEBUG_PASS("msg_executer_parse_line() - Execution-Command");
		MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_EXECUTION_MSG_PENDING);
		return 1;
	}

	if (memcmp(p_command_data, MSG_EXECUTER_COMMAND_TYPE_COM_STR, strlen(MSG_EXECUTER_COMMAND_TYPE_COM_STR)) == 0) {

		DEBUG_PASS("msg_executer_parse_line() - Communication-Command");
		MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_COMMUNICATAION_MSG_PENDING);
		return 1;
	}

	MSG_EXECUTER_INVALID_COMMAND_SYNTAX_SIGNAL_send(command_str);
	DEBUG_TRACE_STR(p_command_data, "msg_executer_parse_line() - Unknown command-type");

	return 0;
}

static void msg_executer_parse_command_message(const char* p_command_msg, char* p_command_data) {

	DEBUG_TRACE_STR(p_command_msg, "msg_executer_parse_command_message() - Command: ");

	i16 num_bytes_line = 0;
	char line[MSG_EXECUTER_MAX_FILE_LINE_LENGTH];	

	do {
		num_bytes_line = COMMAND_FILE_read_next_line(line, MSG_EXECUTER_MAX_FILE_LINE_LENGTH);

		if (num_bytes_line > 0) {

			common_tools_string_trim(line);
			if (msg_executer_parse_line(line, p_command_msg, p_command_data)) {
				break;
			}
		}

	} while (num_bytes_line != -1);
}

static i8 msg_executer_parse_report_command(char* p_report_name, char* p_command_data) {

	if ( ! REPORT_FILE_is_open()) {

		if ( ! REPORT_FILE_open() ) {

			DEBUG_PASS("msg_executer_parse_report_command() - Open report-file has FAILED !!! ---");
			DEBUG_PASS("msg_executer_parse_report_command() - MSG_EXECUTER_TASK_STATE_START_REPORT >> MSG_EXECUTER_TASK_STATE_IDLE");

			MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL_send(REPORT_FILE_get_path());
			return 0;
		}

		DEBUG_PASS("msg_executer_parse_report_command() - Report-File has been opened");
	}

	char line[MSG_EXECUTER_MAX_FILE_LINE_LENGTH];	
	i16 num_bytes_line = 0;

	do {
		num_bytes_line = REPORT_FILE_read_next_line(line, MSG_EXECUTER_MAX_FILE_LINE_LENGTH);

		if (num_bytes_line > 0) {

			common_tools_string_trim(line);

			if (msg_executer_parse_line(line, NULL, p_command_data)) {
				common_tools_string_split(MSG_EXECUTER_KEY_VALUE_SPLITTER, line, p_report_name, MSG_EXECUTER_MAX_MESSAGE_LENGTH, NULL, 0);
				DEBUG_TRACE_STR(p_report_name, "msg_executer_parse_report_command() - Valid Report-Command found");
				break;
			}
		}

	} while (num_bytes_line != -1);

	if (num_bytes_line < 0) {

		DEBUG_PASS("msg_executer_parse_report_command() - END-OF-FILE");

		REPORT_FILE_close();

		return -1;
	}

	return 1;
}

static void msg_executer_prepare_report(char* p_response_message, char* p_response_buffer, char* p_report_name, u16 max_response_length) {

	DEBUG_PASS("msg_executer_prepare_report()");

	char t_message[MSG_EXECUTER_MAX_MESSAGE_LENGTH];

	memset(t_message, '\0', MSG_EXECUTER_MAX_MESSAGE_LENGTH);

	memcpy(t_message, p_report_name, strlen(p_report_name));
	memcpy(t_message + strlen(p_report_name) + 1, p_response_buffer, strlen(p_response_buffer));

	t_message[strlen(p_report_name)] = MSG_EXECUTER_KEY_VALUE_SPLITTER;

	u16 length = strlen(t_message);

	if (length > max_response_length) {
		length = max_response_length;
	} 

	//memset(p_response_message, '\0', max_response_length);
	//memcpy(p_response_message, t_message, strlen(t_message));
	common_tools_string_copy_string(p_response_message, t_message, max_response_length);
}

// --------------------------------------------------------------------------------

static void msg_executer_cfg_object_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("msg_executer_cfg_object_CALLBACK() - NULL-POINTER-EXCEPTION");
		return;
	}

	CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

	if (common_tools_string_compare(REPORT_FILE_PATH_CFG_NAME, p_cfg_object->key)) {

		DEBUG_TRACE_STR(p_cfg_object->value, "msg_executer_cfg_object_CALLBACK() - REPORT_FILE_PATH_CFG_NAME cfg-object");
		REPORT_FILE_set_path(p_cfg_object->value);
		return;
	}

	if (common_tools_string_compare(COMMAND_FILE_PATH_CFG_NAME, p_cfg_object->key)) {
		
		DEBUG_TRACE_STR(p_cfg_object->value, "msg_executer_cfg_object_CALLBACK() - COMMAND_FILE_PATH_CFG_NAME cfg-object");
		COMMAND_FILE_set_path(p_cfg_object->value);
		return;
	}

	if (common_tools_string_compare(REPORT_INTERVAL_CFG_NAME, p_cfg_object->key)) {
		
		msg_executer_report_interval_timeout_ms = common_tools_string_to_u32(p_cfg_object->value);

		DEBUG_TRACE_STR(p_cfg_object->value, "msg_executer_cfg_object_CALLBACK() - REPORT_INTERVAL_CFG_NAME cfg-object");
		DEBUG_TRACE_word(msg_executer_report_interval_timeout_ms, "msg_executer_cfg_object_CALLBACK() - msg_executer_report_interval_timeout_ms : ");

		return;
	}

	DEBUG_TRACE_STR(p_cfg_object->key, "msg_executer_cfg_object_CALLBACK() - Unknown cfg_object");
}

static void msg_executer_cfg_complete_CALLBACK(const void* p_argument) {

	DEBUG_PASS("mqtt_cfg_complete_CALLBACK()");
	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_USER_CFG_SET);
}

static void msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK() - NULL-POINTER-EXCEPTION");
		return;
	}

	u8 msg_length = strlen((const char*) p_argument);

	if (msg_length > MSG_EXECUTER_MAX_MESSAGE_LENGTH) {
		DEBUG_PASS("msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK() - message to long");
		return;
	}

	//memset(msg_executer_received_command_name, '\0', MSG_EXECUTER_MAX_MESSAGE_LENGTH);
	//memcpy(msg_executer_received_command_name, (const char*) p_argument, msg_length);
	common_tools_string_copy_string(msg_executer_received_command_name, (const char*) p_argument, MSG_EXECUTER_MAX_MESSAGE_LENGTH);

	DEBUG_TRACE_STR(msg_executer_received_command_name, "msg_executer_MQTT_MESSAGE_RECEIVED_CALLBACK()");

	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_MSG_RECEIVED);
}


static void msg_executer_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

	if (msg_executer_task_state != MSG_EXECUTER_TASK_STATE_WAIT_FOR_COM_RESPONSE) {
		DEBUG_TRACE_byte(msg_executer_task_state, "msg_executer_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK() - Not waiting for COM-response !!!");
		return;
	}

	DEBUG_PASS("msg_executer_RPI_HOST_RESPONSE_RECEIVED_SLOT_CALLBACK()");

	COMMON_GENERIC_BUFFER_TYPE* p_com_buffer = (COMMON_GENERIC_BUFFER_TYPE*) p_argument;

	pending_response_rpi_command.length = p_com_buffer->length;
	memcpy(pending_response_rpi_command.data, p_com_buffer->data, p_com_buffer->length);

	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED);
}

static void msg_executer_RPI_HOST_RESPONSE_TIMEOUT_SLOT_CALLBACK(const void* p_argument) {

	DEBUG_PASS("msg_executer_RPI_HOST_RESPONSE_TIMEOUT_SLOT_CALLBACK()");
	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_RESPONSE_TIMEOUT);
}

static void msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK(const void* p_argument) {

	if (msg_executer_task_state != MSG_EXECUTER_TASK_STATE_WAIT_FOR_EXE_RESPONSE) {
		DEBUG_TRACE_byte(msg_executer_task_state, "msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK() - Not waiting for EXE-response !!!");
		return;
	}

	const char* p_response = (const char*) p_argument;
	common_tools_string_copy_string(pending_response_cli_command, p_response, MSG_EXECUTER_MAX_MESSAGE_LENGTH);

	DEBUG_TRACE_STR(p_response, "msg_executer_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK()");

	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_RESPONSE_RECEIVED);
}

static void msg_executer_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_CALLBACK(const void* p_argument) {

	DEBUG_PASS("msg_executer_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_CALLBACK()");
	MSG_EXECUTER_STATUS_set(MSG_EXECUTER_STATUS_COMMAND_NOT_FOUND);
}
