/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    mqtt_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 21
 * @brief   Realizes an interface to create an MQTT-Client that can connect to
 *          a MQTT-Broker subscribe a topic and send and receive message
 *          from /to this topic
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_mqtt_interface_
#define _H_mqtt_interface_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "common/qeue_interface.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#include <MQTTClient.h>

// --------------------------------------------------------------------------------

/**
 * @brief Default values to be used on instantiating
 * and configure an instance of the mqtt-interface
 * 
 */
#define MQTT_DEFAULT_HOST_ADDR              "tcp://192.168.110.12:1883"
#define MQTT_DEFAULT_CLIENT_ID              "Smart_Home_Dev_Client"
#define MQTT_DEFAULT_TOPIC                  "myflat/bedroom"
#define MQTT_DEFAULT_WELCOME_MSG            "Hello Smart Home!"
#define MQTT_QOS                            1
#define MQTT_DEFAULT_TIMEOUT                1000L

#define MQTT_EXIT_STRING                    "cmd_exit"
#define MQTT_EXIT_STRING_LEN                8

#define MQTT_HOST_ADDRESS_STRING_LENGTH     26
#define MQTT_TOPIC_NAME_STRING_LENGTH       64
#define MQTT_CLIENT_ID_STRING_LENGTH        26
#define MQTT_WELCOME_MSG_STRING_LENGTH      26

// --------------------------------------------------------------------------------

/**
 * @brief Return values used by the mqtt-interface
 * 
 */
#define MQTT_NO_ERROR                       0 // everything is fine
#define MQTT_ERROR_QEUE_EMPTY               1 // no new mqtt message available
#define MQTT_ERROR_QEUE_ERROR               2 // failure using the mqtt-message queue
#define MQTT_ERROR_TIMEOUT                  3

// --------------------------------------------------------------------------------

#define MQTT_CFG_PREFIX_STRING              "MQTT_"

#define MQTT_HOST_CFG_STRING                "MQTT_HOST_ADDRESS"
#define MQTT_TOPIC_CFG_STRING               "MQTT_TOPIC_NAME"
#define MQTT_CLIENT_ID_CFG_STRING           "MQTT_CLIENT_ID"
#define MQTT_TIMEOUT_CFG_STRING             "MQTT_TIMEOUT_MS"
#define MQTT_WELCOME_MSG_CFG_STRING         "MQTT_WELCOME_MESSAGE"
#define MQTT_KEEP_ALIVE_INTERVAL_CFG_STRING "MQTT_KEEP_ALIVE_INTERVAL_MS"
#define MQTT_RECONNECT_INTERVAL_CFG_STRING  "MQTT_RECONNECT_INTERVAL_MS"

// --------------------------------------------------------------------------------

#ifndef MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS
#define MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS         10000
#endif

#ifndef MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS
#define MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS      2000
#endif

#ifndef MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS
#define MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS       30000
#endif

#ifndef MQTT_INTERFACE_MAX_MSG_LENGTH
#define MQTT_INTERFACE_MAX_MSG_LENGTH                       255
#endif

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_TO_SEND_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_SEND_SUCCEED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_MESSAGE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_CONNECTION_ESTABLISHED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MQTT_CONNECTION_LOST_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Callback to initialize the mqtt-message queue
 * 
 */
typedef void (*MQTT_INTERFACE_QEUE_INIT_CALLBACK) (void);

/**
 * @brief Callback to put a new mqtt-message into the queue
 * @param p_msg_from \0 terminated string reference to the message to be added to the queue
 * @return 
 * 
 */
typedef u8 (*MQTT_INTERFACE_QEUE_ENQEUE_CALLBACK) (const char* p_msg_from);

/**
 * @brief Callback to load the next available message from the mqtt-message queue
 * If a message is available it will be stored into.
 * @param p_msg_to reference to the buffer where to store the mqtt-message into
 * @return 
 * 
 */
typedef u8 (*MQTT_INTERFACE_QEUE_DEQEUE_CALLBACK) (char* p_msg_to);

/**
 * @brief Callback to check if the mqtt-mesage quee is full
 * @return 1 if the que is full, otherwise 0.
 * 
 */
typedef u8 (*MQTT_INTERFACE_QEUE_IS_FULL_CALLBACK) (void);

/**
 * @brief Callback to check if the mqtt-message queue is empty
 * @return 1 the queue is empty, otherwise 0
 * 
 */
typedef u8 (*MQTT_INTERFACE_QEUE_IS_EMPTY_CALLBACK) (void);

/**
 * @brief Callback to get the mutex of the mqtt-message queue.
 * operations on the mqtt-message queue shall only be performed if the
 * queue has been ocupied.
 * @return 1 the mutex has been ocupied, 0 otherwise
 * 
 */
typedef u8 (*MQTT_INTERFACE_QEUE_MUTEX_GET_CALLBACK) (void);

/**
 * @brief Callback to release the mqtt-message queue mutex that has been occupied before.
 * 
 */
typedef void (*MQTT_INTERFACE_QEUE_MUTEX_RELEASE_CALLBACK) (void);

// --------------------------------------------------------------------------------

/**
 * @brief Structure to realize a interface
 * to a message-queue for incomming and outgoing mqtt-mesages
 * 
 */
typedef struct {

    /**
     * @brief Maximum number of characters a mesage can have
     * 
     */
    u16 max_message_length;

    /**
     * @brief Number of message actual inside of the queue
     * 
     */
    u8  max_message_count;

    /**
     * @brief callback to initialize the queue
     * @see MQTT_INTERFACE_QEUE_INIT_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_INIT_CALLBACK init;

    /**
     * @brief callback to add an message to the queue
     * @see MQTT_INTERFACE_QEUE_ENQEUE_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_ENQEUE_CALLBACK enqeue;

    /**
     * @brief callback to get a message from the queue
     * @see MQTT_INTERFACE_QEUE_DEQEUE_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_DEQEUE_CALLBACK deqeue;

    /**
     * @brief callback to check if the queue is full
     * @see MQTT_INTERFACE_QEUE_IS_FULL_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_IS_FULL_CALLBACK is_full;

    /**
     * @brief callback to check if the queue is empty
     * @see MQTT_INTERFACE_QEUE_IS_EMPTY_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_IS_EMPTY_CALLBACK is_empty;

    /**
     * @brief callback to get the mutex of this queue
     * @see MQTT_INTERFACE_QEUE_MUTEX_GET_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_MUTEX_GET_CALLBACK mutex_get;

    /**
     * @brief callback to release the mutex of this queue
     * @see MQTT_INTERFACE_QEUE_MUTEX_RELEASE_CALLBACK
     * 
     */
    MQTT_INTERFACE_QEUE_MUTEX_RELEASE_CALLBACK mutex_release;
} MQTT_QEUE_INTERFACE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Structure represents an instance of the mqtt-interface.
 * Instances are created using the MQTT_INTERFACE_BUILD_CLIENT macro.
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
    
    /**
     * @brief address of the MQTT-Broker
     * to which this instance will connect tot
     * 
     */
    char host_address[MQTT_HOST_ADDRESS_STRING_LENGTH];

    /**
     * @brief Topic to subscribe after connection
     * to MQTT-Broker has been established
     * 
     */
    char topic_name[MQTT_TOPIC_NAME_STRING_LENGTH];

    /**
     * @brief Identification of this instance to use for connection.
     * Must be unique for all clients connected to the same MQTT-Broker
     * 
     */
    char client_id[MQTT_CLIENT_ID_STRING_LENGTH];

    /**
     * @brief Message to send to the tpoic after
     * connection to MQTT-broker has been established
     * 
     */
    char welcome_msg[MQTT_WELCOME_MSG_STRING_LENGTH];
    
    char* message[MQTT_INTERFACE_MAX_MSG_LENGTH];
    MQTTClient client;

    /**
     * @brief receive queue of the mqtt-interface instance
     * 
     */
    const MQTT_QEUE_INTERFACE_TYPE rx_qeue;

    /**
     * @brief send queue of this mqtt-interface instance
     * 
     */
    const MQTT_QEUE_INTERFACE_TYPE tx_qeue;

    /**
     * @brief 
     * 
     */
    char connection_lost_cause[MQTT_INTERFACE_MAX_MSG_LENGTH];
    
} MQTT_INTERFACE;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @param p_host_addr 
 * @param p_topic_name 
 * @param p_client_name 
 */
void mqtt_configure(MQTT_INTERFACE* p_mqtt_interface, const char* p_host_addr, const char* p_topic_name, const char* p_client_name);

/**
 * @brief Initializes the given MQTT-Instance.
 * Will set interval and timeout parameter to default values.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return MQTT_NO_ERROR
 */
u8 mqtt_init(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Tries to establish a connection to the specified MQTT-Broker.
 * If the connection is established the specified topic is subscribed.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return  MQTT_ERROR_TIMEOUT  timeout, connection was not established
 *          MQTT_NO_ERROR       connection successful established
 */
u8 mqtt_connect(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Unsubscribes from the actual topic and disconnects from the
 * actual connected MQTT-Broker.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return MQTT_NO_ERROR in every case
 */
u8 mqtt_disconnect(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Try to send the next message that is stored within the TX-queue.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return  MQTT_ERROR_QEUE_EMPTY   No message available
 *          MQTT_ERROR_TIMEOUT      Timeout on sending the message
 *          MQTT_NO_ERROR           Message successful send
 */
u8 mqtt_send_next_message(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Check if there is actual a message pending within the RX-queue.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return 1 another message is available, otherwise 0
 */
u8 mqtt_msg_pending(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Checks if there is a message pending within the TX-queue
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return 1 another message is available, otherwise 0
 */
u8 mqtt_tx_msg_pending(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Adds a new message to the TX-queue of the given mqtt-instance.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @param p_msg_from message to be added
 * @return 1 message to queue successfullt aded, oherwise 0
 */
u8 mqtt_enqeue_message(MQTT_INTERFACE* p_mqtt_interface, const char* p_msg_from);

/**
 * @brief Check if this mqtt-instance connection is lost or not.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @return 1 the connection to the MQTT-broker has been lost, 0 otherwise
 */
u8 mqtt_connection_lost(MQTT_INTERFACE* p_mqtt_interface);

/**
 * @brief Reads the next message from the receive queue.
 * If the given buffer is too short only max_length bytes will be copied into p_msg_to.
 * The rest of the message is discarded and cannot be read later.
 * 
 * @param p_mqtt_interface reference to the instance of the mqtt-interface to use
 * @param p_msg_to buffer where the message is stored into
 * @param max_length maximum length of p_msg_to
 * @return length of the message that has been read, maximum is MQTT_INTERFACE_MAX_MSG_LENGTH
 */
u16 mqtt_get_next_msg(MQTT_INTERFACE* p_mqtt_interface, char* p_msg_to, u16 max_length);

// --------------------------------------------------------------------------------

/**
 * @brief Callback to keep this mqtt-interface alive
 * Must be called periodically within the actual
 * configured alive-interval.
 * 
 */
void mqtt_keep_alive(void);

/**
 * @brief Callback for the underlying MQTT-application.
 * Is called in case the connection to the MQTT-Broker has been lost
 * 
 * @param context reference to the MQTT-interface instance that has lost its connection (@see MQTT_INTERFACE)
 * @param cause reason why the connection has been lost as a string.
 */
void connectionLost_Callback(void *context, char* cause);

/**
 * @brief 
 * 
 * @param context reference to the MQTT-interface instance that has lost its connection (@see MQTT_INTERFACE)
 * @param topicName 
 * @param topcLength 
 * @param message 
 * @return int 
 */
int messageArrived_Callback(void* context, char* topicName, int topcLength, MQTTClient_message* message);

/**
 * @brief Callback of the underlying MQTT-application.
 * Is called in case the last message has been send successful.
 * 
 * @param context reference to the MQTT-interface the message has been delivered (@see MQTT_INTERFACE)
 * @param token optional token
 */
void deliveryComplete_Callback(void* context, MQTTClient_deliveryToken token);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define MQTT_INTERFACE_BUILD_CLIENT(name, max_msg_length, tx_qeue_size, rx_qeue_size)                        \
                                                                \
    QEUE_INTERFACE_BUILD_QEUE(__##name##_RX_QEUE, char, max_msg_length, rx_qeue_size)                    \
    QEUE_INTERFACE_BUILD_QEUE(__##name##_TX_QEUE, char, max_msg_length, tx_qeue_size)                    \
                                                                \
    static MQTT_INTERFACE __##name##_mqtt_interface = {                                    \
                                                                \
        .connection_timeout_ms = MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS,                    \
        .reconnect_interval_ms = MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS,                        \
        .keep_alive_interval_ms = MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS,                        \
                                                                \
        .rx_qeue = {                                                    \
            .max_message_length = max_msg_length,                                    \
            .max_message_count = rx_qeue_size,                                    \
            .init = &__##name##_RX_QEUE_init,                                    \
            .enqeue = &__##name##_RX_QEUE_enqeue,                                    \
            .deqeue = &__##name##_RX_QEUE_deqeue,                                    \
            .is_full = &__##name##_RX_QEUE_is_full,                                    \
            .is_empty = &__##name##_RX_QEUE_is_empty,                                \
            .mutex_get = &__##name##_RX_QEUE_mutex_get,                                \
            .mutex_release = &__##name##_RX_QEUE_mutex_release                            \
        },                                                        \
        .tx_qeue = {                                                    \
            .max_message_length = max_msg_length,                                    \
            .max_message_count = tx_qeue_size,                                    \
            .init = &__##name##_TX_QEUE_init,                                    \
            .enqeue = &__##name##_TX_QEUE_enqeue,                                    \
            .deqeue = &__##name##_TX_QEUE_deqeue,                                    \
            .is_full = &__##name##_TX_QEUE_is_full,                                    \
            .is_empty = &__##name##_TX_QEUE_is_empty,                                \
            .mutex_get = &__##name##_TX_QEUE_mutex_get,                                \
            .mutex_release = &__##name##_TX_QEUE_mutex_release                            \
        }                                                        \
    };                                                            \
                                                                \
    void name##_configure(const char* p_host_addr, const char* p_topic_name, const char* p_client_name) {   \
        mqtt_configure(&__##name##_mqtt_interface, p_host_addr, p_topic_name, p_client_name);               \
    }                                                                                                       \
                                                                                                            \
    u8 name##_init(void) {                                                                                  \
        return mqtt_init(&__##name##_mqtt_interface);                                                       \
    }                                                                                                       \
                                                                                                            \
    u8 name##_connect(void) {                                                                               \
        return mqtt_connect(&__##name##_mqtt_interface);                                                    \
    }                                                                                                       \
                                                                                                            \
    u8 name##_send_next_message() {                                                                         \
        return mqtt_send_next_message(&__##name##_mqtt_interface);                                          \
    }                                                                                                        \
                                                                                                            \
    u8 name##_delivery_complete(void) {                                                                            \
        return __##name##_mqtt_interface.msg_delivered != 0 ? 1 : 0;                            \
    }                                                                                                        \
                                                                                                            \
    u8 name##_connection_lost(void) {                                                                            \
        return mqtt_connection_lost(&__##name##_mqtt_interface);                            \
    }                                                                                                        \
                                                                                                           \
    u8 name##_is_initialized(void) {                                                                            \
        return __##name##_mqtt_interface.initialized != 0 ? 1 : 0;                            \
    }                                                                                                        \
                                                                                                            \
    u8 name##_msg_pending(void) {                                                                                \
        return mqtt_msg_pending(&__##name##_mqtt_interface);                                \
    }                                                                                                        \
                                                                                                            \
    u8 name##_tx_msg_pending(void) {                                                                           \
        return mqtt_tx_msg_pending(&__##name##_mqtt_interface);                                \
    }                                                                                                        \
                                                                                                            \
    u8 name##_get_msg(char* p_msg_to, u16 max_length) {                                    \
        return mqtt_get_next_msg(&__##name##_mqtt_interface, p_msg_to, max_length);                    \
    }                                                                                                        \
                                                                                                            \
    u8 name##_disconnect(void) {                                                                               \
        return mqtt_disconnect(&__##name##_mqtt_interface);                                \
    }                                                                                                        \
                                                                                                            \
    u8 name##_enqeue_message(const char* message) {                                        \
        return mqtt_enqeue_message(&__##name##_mqtt_interface, message);                        \
    }                                                                                                        \
                                                                                                            \
    const char* name##_get_connection_lost_cause(void) {                                    \
        return (const char*)__##name##_mqtt_interface.connection_lost_cause;                        \
    }                                                                                                        \
                                                                                                            \
    void name##_keep_alive(void) {                                                                               \
        mqtt_keep_alive();                                                                               \
    }                                                                                                        \
                                                                                                            \
    void name##_set_reconnect_interval(u32 reconnect_interval_ms) {                                \
        __##name##_mqtt_interface.reconnect_interval_ms = reconnect_interval_ms;                    \
    }                                                                                                        \
                                                                                                            \
    void name##_set_connection_timeout(u32 connection_timeout_ms) {                                \
        __##name##_mqtt_interface.connection_timeout_ms = connection_timeout_ms;                    \
    }                                                                                                       \
                                                                                                            \
    void name##_set_keep_alive_interval(u32 keep_alive_interval_ms) {                            \
        __##name##_mqtt_interface.keep_alive_interval_ms = keep_alive_interval_ms;                    \
    }                                                                                                        \
                                                                                                            \
    u32 name##_get_reconnect_interval(void) {                                                                        \
        return __##name##_mqtt_interface.reconnect_interval_ms;                                \
    }                                                                                                        \
                                                                                                            \
    u32 name##_get_connection_timeout(void) {                                                                       \
        return __##name##_mqtt_interface.connection_timeout_ms;                                \
    }                                                                                                        \
                                                                                                            \
    u32 name##_get_keep_alive_interval(void) {                                                                        \
        return __##name##_mqtt_interface.keep_alive_interval_ms;                            \
    }

// --------------------------------------------------------------------------------

#define MQTT_INTERFACE_INCLUDE_HOST(name)                                            \
    void name##_configure(const char* p_host_addr, const char* p_topic_name, const char* p_client_name);            \
    u8 name##_init(void);                                                    \
    u8 name##_connect(void);                                                \
    u8 name##_send_next_message(;                                                \
    u8 name##_enqeue_message(const char* message);                                        \
    u8 name##_delivery_complete(void);                                            \
    u8 name##_connection_lost(void);                                            \
    u8 name##_tx_msg_pending(void);                                                \
    u8 name##_msg_pending(void);                                                \
    u8 name##_is_initialized(void);                                                \
    const char* name##_get_connection_lost_cause(void) ;                                    \
    void name##_keep_alive(void);                                                \
    void name##_set_reconnect_interval(u32 reconnect_interval_ms) ;                                \
    void name##_set_connection_timeout(u32 connection_timeout_ms);                                \
    void name##_set_keep_alive_interval(u32 keep_alive_interval_ms);                                                

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void mqtt_interface_init(void);

// --------------------------------------------------------------------------------

#endif // _H_mqtt_interface_

// --------------------------------------------------------------------------------
