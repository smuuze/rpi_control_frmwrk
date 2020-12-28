/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	protocol_management/mqtt/mqtt_application.c
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
#include "time_management/time_management.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

// --------------------------------------------------------------------------------

#include "protocol_management/mqtt/mqtt_interface.h"

// --------------------------------------------------------------------------------

#ifndef MQTT_APPLICATION_CONNECT_INTERVAL_TIMEOUT_MS			
#define MQTT_APPLICATION_CONNECT_INTERVAL_TIMEOUT_MS			30000
#endif

#ifndef MQTT_APPLICATION_MAX_MSG_LENGTH
#define MQTT_APPLICATION_MAX_MSG_LENGTH					511
#endif

#ifndef MQTT_APPLICATION_IDLE_SCHEDULE_INTERVAL_MS			
#define MQTT_APPLICATION_IDLE_SCHEDULE_INTERVAL_MS			32
#endif

#ifndef MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS
#define MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS			1000
#endif

// --------------------------------------------------------------------------------

typedef enum {

	MQTT_APPLICATION_TASK_STATE_IDLE,
	MQTT_APPLICATION_TASK_STATE_WAIT_FOR_USER_CONFIGURATION,
	MQTT_APPLICATION_TASK_STATE_APPLY_USER_CFG,
	MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION,
	MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST,
	MQTT_APPLICATION_TASK_STATE_PROCESS_MESSAGE,
	MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE,
	MQTT_APPLICATION_TASK_STATE_TERMINATED

} MQTT_APPLICATION_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/*!
 *
 */
static void mqtt_interface_task_init(void);

/*!
 *
 */
static u16 mqtt_interface_task_get_schedule_interval(void);

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE mqtt_interface_task_get_state(void);

/*!
 *
 */
static void mqtt_interface_task_run(void);

/*!
 *
 */
static void mqtt_interface_task_finish(void);

/*!
 *
 */
static void mqtt_interface_task_terminate(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
static void mqtt_new_cfg_object_CALLBACK(const void* p_argument);

/*!
 *
 */
static void mqtt_cfg_complete_CALLBACK(const void* p_argument);

/*!
 *
 */
static void mqtt_message_to_send_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_CONNECTION_ESTABLISHED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_CONNECTION_LOST_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_TO_SEND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_SEND_FAILED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MQTT_MESSAGE_SEND_SUCCEED_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, MQTT_NEW_CFG_OBJECT_SLOT, mqtt_new_cfg_object_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_CFG_COMPLETE_SIGNAL, MQTT_CFG_COMPLETE_SLOT, mqtt_cfg_complete_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_MESSAGE_TO_SEND_SIGNAL, MQTT_MESSAGE_TO_SEND_SLOT, mqtt_message_to_send_CALLBACK)

// --------------------------------------------------------------------------------

MQTT_INTERFACE_BUILD_HOST(MQTT_HOST)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MQTT_OP_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U32(MQTT_CONNECT_INTERVAL_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U32(MQTT_KEEP_ALIVE_TIMER)

// --------------------------------------------------------------------------------

#define MQTT_STATUS_IS_CONNECTED	(1 << 0)
#define MQTT_STATUS_USER_CFG_SET	(1 << 1)

BUILD_MODULE_STATUS_U8(MQTT_STATUS)

// --------------------------------------------------------------------------------

/*!
 *
 */
static MCU_TASK_INTERFACE mqtt_interface_task = {

	0, 						// 	identifier,
	0, 						// 	new_run_timeout,
	0, 						// 	last_run_time,
	&mqtt_interface_task_init, 			// 	init,
	&mqtt_interface_task_get_schedule_interval,	// 	get_schedule_interval,
	&mqtt_interface_task_get_state, 		// 	get_sate,
	&mqtt_interface_task_run, 			// 	run,
	0,						// 	background_run,
	0, 						// 	sleep,
	0, 						// 	wakeup,
	&mqtt_interface_task_finish, 			// 	finish,
	&mqtt_interface_task_terminate, 		// 	terminate,
	0						// 	next-task
};

static MQTT_APPLICATION_TASK_STATE_TYPE mqtt_task_state = MQTT_APPLICATION_TASK_STATE_WAIT_FOR_USER_CONFIGURATION;

/*!
 *
 */
static char mqtt_task_welcome_message[MQTT_APPLICATION_MAX_MSG_LENGTH];

/*!
 *
 */
static u32 mqtt_connect_interval_timeout_ms = 0;

// --------------------------------------------------------------------------------

void mqtt_interface_init(void) {
	
	DEBUG_PASS("mqtt_interface_init() - MQTT_STATUS_init()");
	MQTT_STATUS_clear_all();

	mqtt_connect_interval_timeout_ms = MQTT_APPLICATION_CONNECT_INTERVAL_TIMEOUT_MS;

	DEBUG_PASS("mqtt_interface_init() - MQTT_CONNECTION_ESTABLISHED_SIGNNAL_init()");
	MQTT_CONNECTION_ESTABLISHED_SIGNAL_init();

	DEBUG_PASS("mqtt_interface_init() - MQTT_CONNECTION_LOST_SIGNNAL_init()");
	MQTT_CONNECTION_LOST_SIGNAL_init();

	DEBUG_PASS("mqtt_interface_init() - MQTT_CONNECTION_LOST_SIGNNAL_init()");
	MQTT_MESSAGE_SEND_FAILED_SIGNAL_init();

	DEBUG_PASS("mqtt_interface_init() - MQTT_MESSAGE_RECEIVED_SIGNAL_init()");
	MQTT_MESSAGE_RECEIVED_SIGNAL_init();

	DEBUG_PASS("mqtt_interface_init() - MQTT_MESSAGE_TO_SEND_SIGNAL_init()");
	MQTT_MESSAGE_TO_SEND_SIGNAL_init();

	DEBUG_PASS("mqtt_interface_init() - MQTT_MESSAGE_SEND_SUCCEED_SIGNAL_init()");
	MQTT_MESSAGE_SEND_SUCCEED_SIGNAL_init();
	
	DEBUG_PASS("cfg_file_parser_init() - MQTT_NEW_CFG_OBJECT_SLOT_connect()");
	MQTT_NEW_CFG_OBJECT_SLOT_connect();

	DEBUG_PASS("cfg_file_parser_init() - MQTT_CFG_COMPLETE_SLOT_connect()");
	MQTT_CFG_COMPLETE_SLOT_connect();

	DEBUG_PASS("cfg_file_parser_init() - MQTT_CFG_COMPLETE_SLOT_connect()");
	MQTT_MESSAGE_TO_SEND_SLOT_connect();

	MQTT_HOST_configure(MQTT_DEFAULT_HOST_ADDR, MQTT_DEFAULT_TOPIC, MQTT_DEFAULT_CLIENT_ID);

	DEBUG_PASS("cfg_file_parser_init() - mcu_task_controller_register_task(mqtt_interface_task)");
	mcu_task_controller_register_task(&mqtt_interface_task);

	MQTT_HOST_init();

	MQTT_KEEP_ALIVE_TIMER_stop();
	MQTT_CONNECT_INTERVAL_TIMER_stop();
}

// --------------------------------------------------------------------------------

static void mqtt_process_msg(void) {

	char new_msg[MQTT_APPLICATION_MAX_MSG_LENGTH];
	memset(new_msg, '\0', MQTT_APPLICATION_MAX_MSG_LENGTH);

	u16 msg_length = MQTT_HOST_get_msg(new_msg, MQTT_APPLICATION_MAX_MSG_LENGTH - 1);

	if (msg_length != 0) {
		MQTT_MESSAGE_RECEIVED_SIGNAL_send((const void*)new_msg);
	}
}

// --------------------------------------------------------------------------------

/*!
 *
 */
static void mqtt_interface_task_init(void) {
	
	DEBUG_PASS("mqtt_interface_task_init()");
	mqtt_task_state = MQTT_APPLICATION_TASK_STATE_WAIT_FOR_USER_CONFIGURATION;
}

/*!
 *
 */
static u16 mqtt_interface_task_get_schedule_interval(void) {
	
	//DEBUG_PASS("mqtt_interface_task_get_schedule_interval()");

	if (mqtt_task_state == MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST) {

		//DEBUG_PASS("mqtt_interface_task_get_schedule_interval() - connect to host");
		return MQTT_APPLICATION_IDLE_SCHEDULE_INTERVAL_MS;

	} else if (mqtt_task_state == MQTT_APPLICATION_TASK_STATE_IDLE) {

		return MQTT_APPLICATION_IDLE_SCHEDULE_INTERVAL_MS;

	} else  if (mqtt_task_state == MQTT_APPLICATION_TASK_STATE_TERMINATED) {

		return MQTT_APPLICATION_IDLE_SCHEDULE_INTERVAL_MS;

	} else {

		return 0;
	}
}

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE mqtt_interface_task_get_state(void) {

	if (mqtt_task_state == MQTT_APPLICATION_TASK_STATE_TERMINATED) {
		return MCU_TASK_SLEEPING;
	}

	if (mqtt_task_state == MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST) {

		// reduce cpu-load while waiting for reconnect timeout
		if (MQTT_CONNECT_INTERVAL_TIMER_is_active() && MQTT_CONNECT_INTERVAL_TIMER_is_up(mqtt_connect_interval_timeout_ms) == 0) {
			return MCU_TASK_SLEEPING;
		}
	}
	
	if (mqtt_task_state != MQTT_APPLICATION_TASK_STATE_IDLE) {
		DEBUG_PASS("mqtt_interface_task_get_state() - RUNNING (not sleeping)");
		return MCU_TASK_RUNNING;
	}

	if (MQTT_HOST_connection_lost()) {
		DEBUG_PASS("mqtt_interface_task_get_state() - RUNNING (connection-lost)");
		return MCU_TASK_RUNNING;
	}

	if (MQTT_HOST_msg_pending()) {
		DEBUG_PASS("mqtt_interface_task_get_state() - RUNNING (rx-msg pending)");
		return MCU_TASK_RUNNING;
	}

	if (MQTT_HOST_tx_msg_pending()) {
		DEBUG_PASS("mqtt_interface_task_get_state() - RUNNING (tx-msg pending)");
		return MCU_TASK_RUNNING;
	}

	if (MQTT_KEEP_ALIVE_TIMER_is_up(MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS)) {
		DEBUG_PASS("mqtt_interface_task_get_state() - RUNNING (keep-alive timeout)");
		return MCU_TASK_RUNNING;
	}

	//DEBUG_PASS("mqtt_interface_task_get_state() - SLEEPING");
	return MCU_TASK_SLEEPING;
}

/*!
 *
 */
static void mqtt_interface_task_run(void) {

	switch (mqtt_task_state) {

		default:

			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_IDLE;
			// no break;

		case MQTT_APPLICATION_TASK_STATE_WAIT_FOR_USER_CONFIGURATION :

			if (MQTT_STATUS_is_set(MQTT_STATUS_USER_CFG_SET) == 0) {
				break;
			}
	
			DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_WAIT_FOR_USER_CONFIGURATION >> MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION");

			MQTT_CONNECT_INTERVAL_TIMER_stop();
			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION;

			// no break;

		case MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION :

			if (MQTT_STATUS_is_set(MQTT_STATUS_IS_CONNECTED)) {
				MQTT_HOST_disconnect();
				MQTT_STATUS_unset(MQTT_STATUS_IS_CONNECTED);
			}

			DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION >> MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST");
			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST;

			MQTT_OP_TIMER_stop();

			break;

		case MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST :

			if (MQTT_CONNECT_INTERVAL_TIMER_is_active() && MQTT_CONNECT_INTERVAL_TIMER_is_up(mqtt_connect_interval_timeout_ms) == 0) {
				//DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST - wait for connect-interval");
				break;
			}

			MQTT_CONNECT_INTERVAL_TIMER_start();

			if (MQTT_HOST_connect() != MQTT_NO_ERROR) {
				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST - connect to host has FAILED !!! ---");
				MQTT_OP_TIMER_start();
				break;
			}

			MQTT_CONNECTION_ESTABLISHED_SIGNAL_send(NULL);
			MQTT_HOST_enqeue_message(mqtt_task_welcome_message);
			MQTT_STATUS_set(MQTT_STATUS_IS_CONNECTED);

			MQTT_KEEP_ALIVE_TIMER_start();

			DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_CONNECT_TO_HOST >> MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE");
			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE;

			break;

		case MQTT_APPLICATION_TASK_STATE_IDLE :

			if (MQTT_HOST_connection_lost()) {

				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_IDLE >> MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION");
				MQTT_CONNECTION_LOST_SIGNAL_send(NULL);
				mqtt_task_state = MQTT_APPLICATION_TASK_STATE_CLOSE_CONNECTION;
				break;
			}

			if (MQTT_HOST_msg_pending()) {

				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_IDLE >> MQTT_APPLICATION_TASK_STATE_PROCESS_MESSAGE");
				mqtt_task_state = MQTT_APPLICATION_TASK_STATE_PROCESS_MESSAGE;
				break;
			}

			if (MQTT_HOST_tx_msg_pending()) {

				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_IDLE >> MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE");
				mqtt_task_state = MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE;
				break;
			}

			if (MQTT_KEEP_ALIVE_TIMER_is_up(MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS)) {

				DEBUG_PASS("mqtt_interface_task_run() - MQTT_HOST_keep_alive()");
				MQTT_KEEP_ALIVE_TIMER_start();
				MQTT_HOST_keep_alive();
			}

			break;

		case MQTT_APPLICATION_TASK_STATE_PROCESS_MESSAGE :

			mqtt_process_msg();

			DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_PROCESS_MESSAGE >> MQTT_APPLICATION_TASK_STATE_IDLE");
			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_IDLE;

			break;

		case MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE :

			if (MQTT_HOST_send_next_message() == MQTT_NO_ERROR) {
				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE - SUCCESS");
				MQTT_MESSAGE_SEND_SUCCEED_SIGNAL_send(NULL);
			} else {
				DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE - FAILED");
				MQTT_MESSAGE_SEND_FAILED_SIGNAL_send(NULL);
			}

			DEBUG_PASS("mqtt_interface_task_run() - MQTT_APPLICATION_TASK_STATE_SEND_MESSAGE >> MQTT_APPLICATION_TASK_STATE_IDLE");
			mqtt_task_state = MQTT_APPLICATION_TASK_STATE_IDLE;
			break;

		case MQTT_APPLICATION_TASK_STATE_TERMINATED :
			break;
	}
}

/*!
 *
 */
static void mqtt_interface_task_finish(void) {

}

/*!
 *
 */
static void mqtt_interface_task_terminate(void) {
	MQTT_HOST_disconnect();
	mqtt_task_state = MQTT_APPLICATION_TASK_STATE_TERMINATED;
}

// --------------------------------------------------------------------------------

static void mqtt_message_to_send_CALLBACK(const void* p_argument) {

	if (p_argument == NULL) {
		DEBUG_PASS("mqtt_message_to_send_CALLBACK() - NULL_POINTER_EXCEPTION !!! ---");
		return;
	}

	const char* msg_to_send = (const char*) p_argument;

	MQTT_HOST_enqeue_message(msg_to_send);

	DEBUG_TRACE_STR(msg_to_send, "mqtt_message_to_send_CALLBACK() - Message:");
}

static u8 mqtt_match_cfg_key(const char* reference, const char* cfg_key) {

	if (strlen(reference) != strlen(cfg_key)) {
		return 0;
	}
	
	if (memcmp(reference, cfg_key, strlen(reference)) != 0) {
		return 0;
	}

	return 1;
}

static void mqtt_new_cfg_object_CALLBACK(const void* p_argument) {

	CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

	if (strlen(MQTT_CFG_PREFIX_STRING) > strlen(p_cfg_object->key)) {
		DEBUG_TRACE_STR(p_cfg_object->key, "mqtt_new_cfg_object_CALLBACK() - invalid cfg_object - length");
		return;
	}

	if (memcmp(MQTT_CFG_PREFIX_STRING, p_cfg_object->key, strlen(MQTT_CFG_PREFIX_STRING)) != 0) {
		DEBUG_TRACE_STR(p_cfg_object->key, "mqtt_new_cfg_object_CALLBACK() - Unknown cfg_object - prefix");
		return;
	}

	if (mqtt_match_cfg_key(MQTT_HOST_CFG_STRING, p_cfg_object->key)) {

		DEBUG_PASS("mqtt_new_cfg_object_CALLBACK() - MQTT_HOST cfg-object");
		MQTT_HOST_configure(p_cfg_object->value, NULL, NULL);
		return;
	}

	if (mqtt_match_cfg_key(MQTT_TOPIC_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_PASS("mqtt_new_cfg_object_CALLBACK() - MQTT_TOPIC cfg-object");
		MQTT_HOST_configure(NULL, p_cfg_object->value, NULL);
		return;
	}

	if (mqtt_match_cfg_key(MQTT_CLIENT_ID_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_PASS("mqtt_new_cfg_object_CALLBACK() - MQTT_CLIENT_ID cfg-object");
		MQTT_HOST_configure(NULL, NULL, p_cfg_object->value);
		return;
	}

	if (mqtt_match_cfg_key(MQTT_TIMEOUT_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_PASS("mqtt_new_cfg_object_CALLBACK() - MQTT_TIMEOUT cfg-object");
		mqtt_connect_interval_timeout_ms = common_tools_string_to_u32(p_cfg_object->value);
		return;
	}

	if (mqtt_match_cfg_key(MQTT_WELCOME_MSG_CFG_STRING, p_cfg_object->key)) {
		
		DEBUG_PASS("mqtt_new_cfg_object_CALLBACK() - MQTT_WELCOME_MSG cfg-object");
		u16 length = strlen(p_cfg_object->value) > MQTT_APPLICATION_MAX_MSG_LENGTH ? MQTT_APPLICATION_MAX_MSG_LENGTH : strlen(p_cfg_object->value);
		memset(mqtt_task_welcome_message, '\0', MQTT_APPLICATION_MAX_MSG_LENGTH);
		memcpy(mqtt_task_welcome_message, p_cfg_object->value, length);
		return;
	}

	DEBUG_TRACE_STR(p_cfg_object->key, "mqtt_new_cfg_object_CALLBACK() - Unknown cfg_object");
}

static void mqtt_cfg_complete_CALLBACK(const void* p_argument) {

	DEBUG_PASS("mqtt_cfg_complete_CALLBACK()");
	MQTT_STATUS_set(MQTT_STATUS_USER_CFG_SET);
}
