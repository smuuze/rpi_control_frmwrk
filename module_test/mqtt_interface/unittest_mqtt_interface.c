/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_mqtt_interface.c
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
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

#include "protocol_management/mqtt/mqtt_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_CONFIGURE					1
#define TEST_CASE_ID_CONNECT					2
#define TEST_CASE_ID_MSG_RECEIVED				3
#define TEST_CASE_ID_CONNECTION_LOST				4
#define TEST_CASE_ID_RECONNECT_FAILED				5
#define TEST_CASE_ID_MESSAGE_TO_SEND				6
#define TEST_CASE_ID_MESSAGE_TO_SEND_ON_CONNECTION_LSOT		7

// --------------------------------------------------------------------------------

#define MAX_MSG_LENGTH				255
#define TEST_CASE_MSG_RECEIVED_MSG_STRING	"light_01_off"
#define TEST_CASE_MSG_TO_SEND_MSG_STRING	"rpt_light_01=01010101"

// --------------------------------------------------------------------------------

char unittest_MSG_RECEIVED[MAX_MSG_LENGTH];
char unittest_MSG_TO_SEND[MAX_MSG_LENGTH];

u8 counter_MQTT_CONFIGURE_CALLBACK = 0;
u8 counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK = 0;
u8 counter_MQTT_CONFIGURE_TOPIC_CALLBACK = 0;
u8 counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK = 0;
u8 counter_MQTT_CONNECT_CALLBACK = 0;
u8 counter_MQTT_INIT_CALLBACK = 0;
u8 counter_MQTT_SEND_MSG_CALLBACK = 0;
u8 counter_MQTT_DISCONNECT_CALLBACK = 0;
u8 counter_MQTT_MSG_RECEIVED_CALLBACK = 0;
u8 counter_MQTT_GET_NEXT_MSG_CALLBACK = 0;
u8 counter_MQTT_NEW_MSG_SIGNAL = 0;
u8 counter_MQTT_MSG_OK = 0;
u8 counter_MQTT_CONNECTION_LOST_CALLBACK = 0;
u8 counter_MQTT_CONNECTION_LOST_SIGNAL = 0;
u8 counter_MQTT_CONNECTION_FAILED_SIGNAL = 0;
u8 counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL = 0;
u8 counter_MQTT_ENQUE_MESSAGE_CALLBACK = 0;
u8 counter_MQTT_TX_MSG_PENDING_CALLBACK = 0;
u8 counter_MQTT_KEEP_ALIVE_CALLBACK = 0;

static void unittest_mqtt_counter_reset(void) {

	counter_MQTT_CONNECT_CALLBACK = 0;
	counter_MQTT_CONFIGURE_CALLBACK = 0;
	counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK = 0;
	counter_MQTT_CONFIGURE_TOPIC_CALLBACK = 0;
	counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK = 0;
	counter_MQTT_INIT_CALLBACK = 0;
	counter_MQTT_SEND_MSG_CALLBACK = 0;
	counter_MQTT_DISCONNECT_CALLBACK = 0;
	counter_MQTT_MSG_RECEIVED_CALLBACK = 0;
	counter_MQTT_GET_NEXT_MSG_CALLBACK = 0;
	counter_MQTT_NEW_MSG_SIGNAL = 0;
	counter_MQTT_MSG_OK = 0;
	counter_MQTT_CONNECTION_LOST_CALLBACK = 0;
	counter_MQTT_CONNECTION_LOST_SIGNAL = 0;
	counter_MQTT_CONNECTION_FAILED_SIGNAL = 0;
	counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL = 0;
	counter_MQTT_ENQUE_MESSAGE_CALLBACK = 0;
	counter_MQTT_TX_MSG_PENDING_CALLBACK = 0;
	counter_MQTT_KEEP_ALIVE_CALLBACK = 0;
}

// stubs

void mqtt_configure(MQTT_INTERFACE* p_mqtt_interface, const char* p_host_addr, const char* p_topic_name, const char* p_client_name) {

	counter_MQTT_CONFIGURE_CALLBACK += 1;
	
	if (p_host_addr != NULL) {
		if (strlen(p_host_addr) < MQTT_HOST_ADDRESS_STRING_LENGTH) {

			DEBUG_TRACE_STR(p_host_addr, "UNITTEST - mqtt_configure() - HOST-ADDRESS");

			counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK += 1;
			memset(p_mqtt_interface->host_address, '\0', MQTT_HOST_ADDRESS_STRING_LENGTH);
			memcpy(p_mqtt_interface->host_address, p_host_addr, strlen(p_host_addr));
		}
	}
	
	if (p_topic_name != NULL) {
		if (strlen(p_topic_name) < MQTT_TOPIC_NAME_STRING_LENGTH) {
			DEBUG_TRACE_STR(p_topic_name, "UNITTEST - mqtt_configure() - TOPIC");

			counter_MQTT_CONFIGURE_TOPIC_CALLBACK += 1;
			memset(p_mqtt_interface->topic_name, '\0', MQTT_TOPIC_NAME_STRING_LENGTH);
			memcpy(p_mqtt_interface->topic_name, p_topic_name, strlen(p_topic_name));
		}
	}
	
	if (p_client_name != NULL) {
		if (strlen(p_client_name) < MQTT_CLIENT_ID_STRING_LENGTH) {
			DEBUG_TRACE_STR(p_client_name, "UNITTEST - mqtt_configure() - CLIENT_ID");

			counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK += 1;
			memset(p_mqtt_interface->client_id, '\0', MQTT_CLIENT_ID_STRING_LENGTH);
			memcpy(p_mqtt_interface->client_id, p_client_name, strlen(p_client_name));
		}
	}
}

u8 mqtt_init(MQTT_INTERFACE* p_mqtt_interface) {
	DEBUG_PASS("UNITTEST - mqtt_init()");
	counter_MQTT_INIT_CALLBACK += 1;
	return 0;
}

u8 mqtt_connect(MQTT_INTERFACE* p_mqtt_interface) {
	counter_MQTT_CONNECT_CALLBACK += 1;

	switch (UT_GET_TEST_CASE_ID()) {

		case TEST_CASE_ID_CONNECTION_LOST :
			DEBUG_PASS("UNITTEST - mqtt_connect() - TEST_CASE_ID_CONNECTION_LOST");
			return MQTT_ERROR_TIMEOUT;

		case TEST_CASE_ID_RECONNECT_FAILED :
			DEBUG_PASS("UNITTEST - mqtt_connect() - TEST_CASE_ID_RECONNECT_FAILED");
			return MQTT_ERROR_TIMEOUT;

		case TEST_CASE_ID_MESSAGE_TO_SEND_ON_CONNECTION_LSOT :
			DEBUG_PASS("UNITTEST - mqtt_connect() - TEST_CASE_ID_MESSAGE_TO_SEND_ON_CONNECTION_LSOT");
			return MQTT_ERROR_TIMEOUT;
	}
	
	DEBUG_PASS("UNITTEST - mqtt_connect() - OK");
	return MQTT_NO_ERROR;
}

u8 mqtt_send_next_message(MQTT_INTERFACE* p_mqtt_interface) {
	DEBUG_PASS("UNITTEST - mqtt_send_message()");
	counter_MQTT_SEND_MSG_CALLBACK += 1;
	return 0;
}

u8 mqtt_msg_pending(MQTT_INTERFACE* p_mqtt_interface) {

	DEBUG_PASS("UNITTEST - mqtt_msg_pending()");
	counter_MQTT_MSG_RECEIVED_CALLBACK += 1;

	if (UT_GET_TEST_CASE_ID() == TEST_CASE_ID_MSG_RECEIVED) {
		return counter_MQTT_GET_NEXT_MSG_CALLBACK == 0 ? 1 : 0;
	} else {
		return 0;
	}
}

u8 mqtt_tx_msg_pending(MQTT_INTERFACE* p_mqtt_interface) {

	DEBUG_PASS("UNITTEST - mqtt_tx_msg_pending()");
	counter_MQTT_TX_MSG_PENDING_CALLBACK += 1;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_MESSAGE_TO_SEND :
			return counter_MQTT_SEND_MSG_CALLBACK == 0 ? 1 : 0;
	}

	return 0;
}

u16 mqtt_get_next_msg(MQTT_INTERFACE* p_mqtt_interface, char* p_msg_to, u16 max_length) {

	counter_MQTT_GET_NEXT_MSG_CALLBACK += 1;

	char msg_received_ok[] = TEST_CASE_MSG_RECEIVED_MSG_STRING;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_MSG_RECEIVED:

			if (counter_MQTT_GET_NEXT_MSG_CALLBACK > 1) {
				return 0;
			}

			DEBUG_PASS("UNITTEST - mqtt_get_next_msg() - TEST_CASE_ID_MSG_RECEIVED");
			memcpy(p_msg_to, msg_received_ok, strlen(TEST_CASE_MSG_RECEIVED_MSG_STRING));
			return 10;
	}
	
	DEBUG_PASS("UNITTEST - mqtt_get_next_msg() - NO MESSAGE");
	return 0;
}

u8 mqtt_enqeue_message(MQTT_INTERFACE* p_mqtt_interface, const char* p_msg_from) {

	DEBUG_PASS("UNITTEST - mqtt_enqeue_message()");
	counter_MQTT_ENQUE_MESSAGE_CALLBACK += 1;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_MESSAGE_TO_SEND :

			memset(unittest_MSG_TO_SEND, '\0', MAX_MSG_LENGTH);
			memcpy(unittest_MSG_TO_SEND, p_msg_from, strlen(p_msg_from));
			return 1;
	}

	return 0;
}

u8 mqtt_disconnect(MQTT_INTERFACE* p_mqtt_interface) {
	DEBUG_PASS("UNITTEST - mqtt_disconnect()");
	counter_MQTT_DISCONNECT_CALLBACK += 1;
	return 1;
}

u8 mqtt_connection_lost(MQTT_INTERFACE* p_mqtt_interface) {
	DEBUG_PASS("UNITTEST - mqtt_connection_lost()");

	counter_MQTT_CONNECTION_LOST_CALLBACK += 1;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_CONNECTION_LOST :
			return 1;

		case TEST_CASE_ID_MESSAGE_TO_SEND_ON_CONNECTION_LSOT :
			return 1;
	}
	
	return 0;
}

void mqtt_keep_alive(void) {

	DEBUG_PASS("mqtt_keep_alive()");
	counter_MQTT_KEEP_ALIVE_CALLBACK += 1;
}

// --------------------------------------------------------------------------------

// slots

static void unittest_MQTT_CONNECTION_ESTABLISHED_CALLBACK(const void* p_argument) {

	(void) p_argument;
	DEBUG_PASS("unittest_MQTT_CONNECTION_ESTABLISHED_CALLBACK()");
	counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL += 1;
}

static void unittest_MQTT_CONNECTION_LOST_CALLBACK(const void* p_argument) {

	(void) p_argument;
	DEBUG_PASS("unittest_MQTT_CONNECTION_LOST_CALLBACK()");
	counter_MQTT_CONNECTION_LOST_SIGNAL += 1;
}

static void unittest_MQTT_CONNECTION_FAILED_CALLBACK(const void* p_argument) {

	(void) p_argument;
	DEBUG_PASS("unittest_MQTT_CONNECTION_FAILED_CALLBACK()");
	counter_MQTT_CONNECTION_FAILED_SIGNAL += 1;
}

static void unittest_MQTT_MESSAGE_RECEIVED_CALLBACK(const void* p_argument) {

	char* msg = (char*) p_argument;
	DEBUG_TRACE_STR(msg, "unittest_MQTT_MESSAGE_RECEIVED_CALLBACK()");

	counter_MQTT_NEW_MSG_SIGNAL += 1;

	switch (UT_GET_TEST_CASE_ID()) {
		case TEST_CASE_ID_MSG_RECEIVED :

			memset(unittest_MSG_RECEIVED, '\0', MAX_MSG_LENGTH);
			memcpy(unittest_MSG_RECEIVED, msg, strlen(msg));
			counter_MQTT_MSG_OK += 1;

			break;
	}
}

// --------------------------------------------------------------------------------

static void unittest_CFG_PARSER_NEW_CFG_OBJECT_CALLBACK(const void* p_argument) {

	(void) p_argument;
	//DEBUG_PASS("unittest_CFG_PARSER_NEW_CFG_OBJECT_CALLBACK()");
}

static void unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK(const void* p_argument) {

	(void) p_argument;
	//DEBUG_PASS("unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK()");
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL, UNITTEST_CFG_PARSER_NEW_CFG_OBJECT_SLOT, unittest_CFG_PARSER_NEW_CFG_OBJECT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CFG_PARSER_CFG_COMPLETE_SIGNAL,   UNITTEST_CFG_PARSER_CFG_COMPLETE_SLOT,   unittest_CFG_PARSER_CFG_COMPLETE_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_MESSAGE_RECEIVED_SIGNAL,     UNITTEST_MQTT_MESSAGE_RECEIVED_SLOT,     unittest_MQTT_MESSAGE_RECEIVED_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_CONNECTION_ESTABLISHED_SIGNAL, UNITTEST_MQTT_CONNECTION_ESTABLISHED_SLOT, unittest_MQTT_CONNECTION_ESTABLISHED_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_CONNECTION_LOST_SIGNAL,         UNITTEST_MQTT_CONNECTION_LOST_SLOT, unittest_MQTT_CONNECTION_LOST_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MQTT_CONNECTION_FAILED_SIGNAL,         UNITTEST_MQTT_CONNECTION_FAILED_SLOT, unittest_MQTT_CONNECTION_FAILED_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void){
	unittest_mqtt_counter_reset();
}

static void UNITTEST_mqtt_interface_init(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_INITIALIZE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		mqtt_interface_init();

		UNITTEST_MQTT_MESSAGE_RECEIVED_SLOT_connect();
		UNITTEST_MQTT_CONNECTION_ESTABLISHED_SLOT_connect();
		UNITTEST_MQTT_CONNECTION_LOST_SLOT_connect();
		UNITTEST_MQTT_CONNECTION_FAILED_SLOT_connect();

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_configure(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_CONFIGURE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONFIGURE);

		unittest_reset_counter();

		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_host_cfg_obj 		= { .key = MQTT_HOST_CFG_STRING			, .value = "localhost" 			};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_topic_cfg_obj 		= { .key = MQTT_TOPIC_CFG_STRING		, .value = "unittest" 			};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_client_id_cfg_obj 		= { .key = MQTT_CLIENT_ID_CFG_STRING		, .value = "unittest_client" 		};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_timeout_cfg_obj 		= { .key = MQTT_TIMEOUT_CFG_STRING		, .value = "1000"	 		};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_welcome_msg_cfg_obj 	= { .key = MQTT_WELCOME_MSG_CFG_STRING		, .value = "Hello Mqtt Unittest"	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_reconnect_interval_cfg_obj = { .key = MQTT_RECONNECT_INTERVAL_CFG_STRING	, .value = "1000"			};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE mqtt_unknown_cfg_obj 		= { .key = "MQTT_UNKNOWN"			, .value = "I am unknown"		};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE invalid_cfg_obj 		= { .key = "INVALID"				, .value = "I am invalid"		};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_host_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_topic_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_client_id_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_timeout_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_reconnect_interval_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_welcome_msg_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&mqtt_unknown_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((const void*)&invalid_cfg_obj);

		CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 3);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_connect(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_CONNECT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONNECT);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (counter_MQTT_SEND_MSG_CALLBACK == 1) {
				break;
			}
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_msg_received(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_MSG_RECEIVED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MSG_RECEIVED);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 1);
		UT_CHECK_IS_GREATER(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_COMPARE_STRING(unittest_MSG_RECEIVED, TEST_CASE_MSG_RECEIVED_MSG_STRING);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_msg_to_send(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_MSG_TO_SEND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_TO_SEND);

		unittest_reset_counter();

		MQTT_MESSAGE_TO_SEND_SIGNAL_send(TEST_CASE_MSG_TO_SEND_MSG_STRING);

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_MSG_RECEIVED_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_CONNECTION_LOST_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 1);
		UT_CHECK_IS_GREATER(counter_MQTT_TX_MSG_PENDING_CALLBACK, 1);
		UT_COMPARE_STRING(unittest_MSG_TO_SEND, TEST_CASE_MSG_TO_SEND_MSG_STRING);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_connection_lost(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_CONNECTION_LOST")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONNECTION_LOST);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(500) == 0) {
			mcu_task_controller_schedule(); // connection lost signal

			if (counter_MQTT_CONNECTION_LOST_SIGNAL != 0) {
				mcu_task_controller_schedule(); // close connection

				while (UNITTEST_TIMER_is_up(250) == 0) {
					mcu_task_controller_schedule(); // try reconnect

					if (counter_MQTT_CONNECT_CALLBACK != 0) {
						break;
					}
				}
				
				break;
			}
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 1); // task will try to reconnect one time directly after connection has been lost
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 2);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_wait_for_reconnect_timeout(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_WAIT_FOR_RECONNECT_TIMEOUT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_RECONNECT_FAILED);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(700) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_reconnect_failed(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_RECONNECT_FAILED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_RECONNECT_FAILED);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(500) == 0) {
			mcu_task_controller_schedule();

			if (counter_MQTT_CONNECT_CALLBACK != 0) {
				break;
			}
		}

		//printf("UNITTEST_TIMER_elapsed() = %d\n", UNITTEST_TIMER_elapsed());

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_reconnect_succeeded(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_RECONNECT_SUCCEEDED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONNECT);

		unittest_reset_counter();

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(500) == 0) {
			mcu_task_controller_schedule();

			if (counter_MQTT_SEND_MSG_CALLBACK != 0) {
				break;
			}
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_MSG_RECEIVED_CALLBACK, 0); // check for message-received
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_GREATER(counter_MQTT_CONNECTION_LOST_CALLBACK, 0); // check for connection-lost
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_mqtt_interface_msg_to_send_while_disconnected(void) {
	
	UT_START_TEST_CASE("MQTT_INTERFACE_MSG_TO_SEND_WHILE_DISCONNECTED")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_MESSAGE_TO_SEND_ON_CONNECTION_LSOT);

		unittest_reset_counter();

		MQTT_MESSAGE_TO_SEND_SIGNAL_send(TEST_CASE_MSG_TO_SEND_MSG_STRING);

		UNITTEST_TIMER_start();
		printf(" - TEST-CASE start time (ms) : %d (%x04)\n", UNITTEST_TIMER_start_time(), UNITTEST_TIMER_start_time());

		while (UNITTEST_TIMER_is_up(1500) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_HOST_ADDR_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_TOPIC_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CLIENT_ID_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONFIGURE_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_INIT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECT_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_SEND_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_DISCONNECT_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_RECEIVED_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_GET_NEXT_MSG_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_NEW_MSG_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_MSG_OK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_CALLBACK, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_LOST_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_ESTABLISHED_SIGNNAL, 0);
		UT_CHECK_IS_EQUAL(counter_MQTT_ENQUE_MESSAGE_CALLBACK, 1);
		UT_CHECK_IS_EQUAL(counter_MQTT_TX_MSG_PENDING_CALLBACK, 0);
		UT_COMPARE_STRING(unittest_MSG_TO_SEND, TEST_CASE_MSG_TO_SEND_MSG_STRING);
		UT_CHECK_IS_EQUAL(counter_MQTT_CONNECTION_FAILED_SIGNAL, 1);
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

	UNITTEST_CFG_PARSER_NEW_CFG_OBJECT_SLOT_connect();
	UNITTEST_CFG_PARSER_CFG_COMPLETE_SLOT_connect();

	UT_START_TESTBENCH("Welcome the the UNITTEST for mqtt-interface v1.0")
	{
		UNITTEST_mqtt_interface_init();
		UNITTEST_mqtt_interface_configure();
		UNITTEST_mqtt_interface_connect();
		UNITTEST_mqtt_interface_msg_received();
		UNITTEST_mqtt_interface_msg_to_send();
		UNITTEST_mqtt_interface_connection_lost();
		UNITTEST_mqtt_interface_wait_for_reconnect_timeout();
		UNITTEST_mqtt_interface_reconnect_failed();
		UNITTEST_mqtt_interface_wait_for_reconnect_timeout();
		UNITTEST_mqtt_interface_reconnect_succeeded();
		UNITTEST_mqtt_interface_msg_received();
		UNITTEST_mqtt_interface_msg_to_send();
		UNITTEST_mqtt_interface_connection_lost();
		UNITTEST_mqtt_interface_msg_to_send_while_disconnected();
	}
	UT_END_TESTBENCH()
}

// --------------------------------------------------------------------------------
