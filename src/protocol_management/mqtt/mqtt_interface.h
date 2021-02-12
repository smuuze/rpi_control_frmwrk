/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	mqtt_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _MQTT_INTERFACE_H_
#define _MQTT_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "common/qeue_interface.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#include <MQTTClient.h>

// --------------------------------------------------------------------------------


#define MQTT_DEFAULT_HOST_ADDR			"tcp://192.168.110.12:1883"
#define MQTT_DEFAULT_CLIENT_ID			"Smart_Home_Dev_Client"
#define MQTT_DEFAULT_TOPIC			"myflat/bedroom"
#define MQTT_DEFAULT_WELCOME_MSG		"Hello Smart Home!"
#define MQTT_QOS				1
#define MQTT_DEFAULT_TIMEOUT			1000L

#define MQTT_EXIT_STRING			"cmd_exit"
#define MQTT_EXIT_STRING_LEN			8

#define MQTT_HOST_ADDRESS_STRING_LENGTH		26
#define MQTT_TOPIC_NAME_STRING_LENGTH		64
#define MQTT_CLIENT_ID_STRING_LENGTH		26
#define MQTT_WELCOME_MSG_STRING_LENGTH		26

// --------------------------------------------------------------------------------

#define MQTT_NO_ERROR				0
#define MQTT_ERROR_QEUE_EMPTY			1
#define MQTT_ERROR_QEUE_ERROR			2
#define MQTT_ERROR_TIMEOUT			3

// --------------------------------------------------------------------------------

#define MQTT_CFG_PREFIX_STRING			"MQTT_"

#define MQTT_HOST_CFG_STRING			"MQTT_HOST_ADDRESS"
#define MQTT_TOPIC_CFG_STRING			"MQTT_TOPIC_NAME"
#define MQTT_CLIENT_ID_CFG_STRING		"MQTT_CLIENT_ID"
#define MQTT_TIMEOUT_CFG_STRING			"MQTT_TIMEOUT_MS"
#define MQTT_WELCOME_MSG_CFG_STRING		"MQTT_WELCOME_MESSAGE"
#define MQTT_KEEP_ALIVE_INTERVAL_CFG_STRING	"MQTT_KEEP_ALIVE_INTERVAL_MS"
#define MQTT_RECONNECT_INTERVAL_CFG_STRING	"MQTT_RECONNECT_INTERVAL_MS"

// --------------------------------------------------------------------------------

#ifndef MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS
#define MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS			10000
#endif

#ifndef MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS
#define MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS			2000
#endif

#ifndef MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS
#define MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS			30000
#endif

#ifndef MQTT_INTERFACE_MAX_MSG_LENGTH
#define MQTT_INTERFACE_MAX_MSG_LENGTH					255
#endif

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_TO_SEND_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_SEND_SUCCEED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_CONNECTION_ESTABLISHED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_CONNECTION_LOST_SIGNAL)

// --------------------------------------------------------------------------------

typedef void (*MQTT_INTERFACE_QEUE_INIT_CALLBACK)			(void);

typedef u8   (*MQTT_INTERFACE_QEUE_ENQEUE_CALLBACK)			(const char* p_msg_from);
typedef u8   (*MQTT_INTERFACE_QEUE_DEQEUE_CALLBACK)			(char* p_msg_to);

typedef u8   (*MQTT_INTERFACE_QEUE_IS_FULL_CALLBACK)			(void);
typedef u8   (*MQTT_INTERFACE_QEUE_IS_EMPTY_CALLBACK)			(void);

typedef u8   (*MQTT_INTERFACE_QEUE_MUTEX_GET_CALLBACK)			(void);
typedef void (*MQTT_INTERFACE_QEUE_MUTEX_RELEASE_CALLBACK)		(void);


// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {
	u16 max_message_length;
	u8  max_message_count;
	MQTT_INTERFACE_QEUE_INIT_CALLBACK init;
	MQTT_INTERFACE_QEUE_ENQEUE_CALLBACK enqeue;
	MQTT_INTERFACE_QEUE_DEQEUE_CALLBACK deqeue;
	MQTT_INTERFACE_QEUE_IS_FULL_CALLBACK is_full;
	MQTT_INTERFACE_QEUE_IS_EMPTY_CALLBACK is_empty;
	MQTT_INTERFACE_QEUE_MUTEX_GET_CALLBACK mutex_get;
	MQTT_INTERFACE_QEUE_MUTEX_RELEASE_CALLBACK mutex_release;
} MQTT_QEUE_INTERFACE_TYPE;

/*!
 *
 */
typedef struct {

	u8 msg_arrived;
	u8 msg_delivered;
	u8 connection_lost;	
	u8 quality_of_service;
	u8 initialized;

	u32 connection_timeout_ms;
	u32 reconnect_interval_ms;
	u32 keep_alive_interval_ms;
	
	char host_address[MQTT_HOST_ADDRESS_STRING_LENGTH];
	char topic_name[MQTT_TOPIC_NAME_STRING_LENGTH];
	char client_id[MQTT_CLIENT_ID_STRING_LENGTH];
	char welcome_msg[MQTT_WELCOME_MSG_STRING_LENGTH];	
	
	char* message[MQTT_INTERFACE_MAX_MSG_LENGTH];
	MQTTClient client;

	const MQTT_QEUE_INTERFACE_TYPE rx_qeue;
	const MQTT_QEUE_INTERFACE_TYPE tx_qeue;

	char connection_lost_cause[MQTT_INTERFACE_MAX_MSG_LENGTH];
	
} MQTT_INTERFACE;

// --------------------------------------------------------------------------------

/*!
 *
 */
void mqtt_configure(MQTT_INTERFACE* p_mqtt_interface, const char* p_host_addr, const char* p_topic_name, const char* p_client_name);

/*!
 *
 */
u8 mqtt_init(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_connect(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_disconnect(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_send_next_message(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_msg_pending(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_tx_msg_pending(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u8 mqtt_enqeue_message(MQTT_INTERFACE* p_mqtt_interface, const char* p_msg_from);

/*!
 *
 */
u8 mqtt_connection_lost(MQTT_INTERFACE* p_mqtt_interface);

/*!
 *
 */
u16 mqtt_get_next_msg(MQTT_INTERFACE* p_mqtt_interface, char* p_msg_to, u16 max_length);

/*!
 *
 */
void mqtt_keep_alive(void);

/*!
 *
 */
void connectionLost_Callback(void *context, char* cause);

/*!
 *
 */
int messageArrived_Callback(void* context, char* topicName, int topcLength, MQTTClient_message* message);

/*!
 *
 */
void deliveryComplete_Callback(void* context, MQTTClient_deliveryToken token);

// --------------------------------------------------------------------------------

#define MQTT_INTERFACE_BUILD_CLIENT(name, max_msg_length, tx_qeue_size, rx_qeue_size)						\
																\
	QEUE_INTERFACE_BUILD_QEUE(__##name##_RX_QEUE, char, max_msg_length, rx_qeue_size)					\
	QEUE_INTERFACE_BUILD_QEUE(__##name##_TX_QEUE, char, max_msg_length, tx_qeue_size)					\
																\
	static MQTT_INTERFACE __##name##_mqtt_interface = {									\
																\
		.connection_timeout_ms = MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS,					\
		.reconnect_interval_ms = MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS,						\
		.keep_alive_interval_ms = MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS,						\
																\
		.rx_qeue = {													\
			.max_message_length = max_msg_length,									\
			.max_message_count = rx_qeue_size,									\
			.init = &__##name##_RX_QEUE_init,									\
			.enqeue = &__##name##_RX_QEUE_enqeue,									\
			.deqeue = &__##name##_RX_QEUE_deqeue,									\
			.is_full = &__##name##_RX_QEUE_is_full,									\
			.is_empty = &__##name##_RX_QEUE_is_empty,								\
			.mutex_get = &__##name##_RX_QEUE_mutex_get,								\
			.mutex_release = &__##name##_RX_QEUE_mutex_release							\
		},														\
		.tx_qeue = {													\
			.max_message_length = max_msg_length,									\
			.max_message_count = tx_qeue_size,									\
			.init = &__##name##_TX_QEUE_init,									\
			.enqeue = &__##name##_TX_QEUE_enqeue,									\
			.deqeue = &__##name##_TX_QEUE_deqeue,									\
			.is_full = &__##name##_TX_QEUE_is_full,									\
			.is_empty = &__##name##_TX_QEUE_is_empty,								\
			.mutex_get = &__##name##_TX_QEUE_mutex_get,								\
			.mutex_release = &__##name##_TX_QEUE_mutex_release							\
		}														\
	};															\
																\
	void name##_configure(const char* p_host_addr, const char* p_topic_name, const char* p_client_name) {			\
		mqtt_configure(&__##name##_mqtt_interface, p_host_addr, p_topic_name, p_client_name);				\
	}															\
																\
	u8 name##_init(void) {													\
		return mqtt_init(&__##name##_mqtt_interface);									\
	}															\
																\
	u8 name##_connect(void) {												\
		return mqtt_connect(&__##name##_mqtt_interface);								\
	}															\
																\
	u8 name##_send_next_message() {												\
		return mqtt_send_next_message(&__##name##_mqtt_interface);							\
	}															\
																\
	u8 name##_delivery_complete(void) {											\
		return __##name##_mqtt_interface.msg_delivered != 0 ? 1 : 0;							\
	}															\
																\
	u8 name##_connection_lost(void) {											\
		return mqtt_connection_lost(&__##name##_mqtt_interface);							\
	}															\
																\
	u8 name##_is_initialized(void) {											\
		return __##name##_mqtt_interface.initialized != 0 ? 1 : 0;							\
	}															\
																\
	u8 name##_msg_pending(void) {												\
		return mqtt_msg_pending(&__##name##_mqtt_interface);								\
	}															\
																\
	u8 name##_tx_msg_pending(void) {											\
		return mqtt_tx_msg_pending(&__##name##_mqtt_interface);								\
	}															\
																\
	u8 name##_get_msg(char* p_msg_to, u16 max_length) {									\
		return mqtt_get_next_msg(&__##name##_mqtt_interface, p_msg_to, max_length);					\
	}															\
																\
	u8 name##_disconnect(void) {												\
		return mqtt_disconnect(&__##name##_mqtt_interface);								\
	}															\
																\
	u8 name##_enqeue_message(const char* message) {										\
		return mqtt_enqeue_message(&__##name##_mqtt_interface, message);						\
	}															\
																\
	const char* name##_get_connection_lost_cause(void) {									\
		return (const char*)__##name##_mqtt_interface.connection_lost_cause;						\
	}															\
																\
	void name##_keep_alive(void) {												\
		mqtt_keep_alive();												\
	}															\
																\
	void name##_set_reconnect_interval(u32 reconnect_interval_ms) {								\
		__##name##_mqtt_interface.reconnect_interval_ms = reconnect_interval_ms;					\
	}															\
																\
	void name##_set_connection_timeout(u32 connection_timeout_ms) {								\
		__##name##_mqtt_interface.connection_timeout_ms = connection_timeout_ms;					\
	}															\
																\
	void name##_set_keep_alive_interval(u32 keep_alive_interval_ms) {							\
		__##name##_mqtt_interface.keep_alive_interval_ms = keep_alive_interval_ms;					\
	}															\
																\
	u32 name##_get_reconnect_interval(void) {										\
		return __##name##_mqtt_interface.reconnect_interval_ms;								\
	}															\
																\
	u32 name##_get_connection_timeout(void) {										\
		return __##name##_mqtt_interface.connection_timeout_ms;								\
	}															\
																\
	u32 name##_get_keep_alive_interval(void) {										\
		return __##name##_mqtt_interface.keep_alive_interval_ms;							\
	}

// --------------------------------------------------------------------------------

#define MQTT_INTERFACE_INCLUDE_HOST(name)											\
	void name##_configure(const char* p_host_addr, const char* p_topic_name, const char* p_client_name);			\
	u8 name##_init(void);													\
	u8 name##_connect(void);												\
	u8 name##_send_next_message(;												\
	u8 name##_enqeue_message(const char* message);										\
	u8 name##_delivery_complete(void);											\
	u8 name##_connection_lost(void);											\
	u8 name##_tx_msg_pending(void);												\
	u8 name##_msg_pending(void);												\
	u8 name##_is_initialized(void);												\
	const char* name##_get_connection_lost_cause(void) ;									\
	void name##_keep_alive(void);												\
	void name##_set_reconnect_interval(u32 reconnect_interval_ms) ;								\
	void name##_set_connection_timeout(u32 connection_timeout_ms);								\
	void name##_set_keep_alive_interval(u32 keep_alive_interval_ms);												


// --------------------------------------------------------------------------------

/*!
 *
 */
void mqtt_interface_init(void);

// --------------------------------------------------------------------------------


#endif // _MQTT_INTERFACE_H_
