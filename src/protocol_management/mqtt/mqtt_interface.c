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
 * @file    mqtt_interface.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 23
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_string.h"
#include "protocol_management/mqtt/mqtt_interface.h"

// --------------------------------------------------------------------------------

void trace_Callback(enum MQTTCLIENT_TRACE_LEVELS level, char* message);

// --------------------------------------------------------------------------------

/**
 * @brief Here we store the last message sent.
 * THe message we maybe receive this message, because
 * we are conencted to the same topic this message was send.
 * Comparing the received msg with the last message sent will
 * avoid incorrect message handling.
 */
static char last_msg_send[MQTT_INTERFACE_MAX_MSG_LENGTH];

// --------------------------------------------------------------------------------

/**
 * @see mqtt_interface.h#mqtt_configure
 */
void mqtt_configure(MQTT_INTERFACE* p_mqtt_interface, const char* p_host_addr, const char* p_topic_name, const char* p_client_name) {
    
    if (p_host_addr != NULL) {
        if (strlen(p_host_addr) < MQTT_HOST_ADDRESS_STRING_LENGTH) {
            DEBUG_TRACE_STR(p_host_addr, "mqtt_configure() - HOST-ADDRESS");
            common_tools_string_copy_string(p_mqtt_interface->host_address, p_host_addr, MQTT_HOST_ADDRESS_STRING_LENGTH);
        }
    }
    
    if (p_topic_name != NULL) {
        if (strlen(p_topic_name) < MQTT_TOPIC_NAME_STRING_LENGTH) {
            DEBUG_TRACE_STR(p_topic_name, "mqtt_configure() - TOPIC");
            common_tools_string_copy_string(p_mqtt_interface->topic_name, p_topic_name, MQTT_TOPIC_NAME_STRING_LENGTH);
        }
    }
    
    if (p_client_name != NULL) {
        if (strlen(p_client_name) < MQTT_CLIENT_ID_STRING_LENGTH) {
            DEBUG_TRACE_STR(p_client_name, "mqtt_configure() - CLIENT_ID");
            common_tools_string_copy_string(p_mqtt_interface->client_id, p_client_name, MQTT_CLIENT_ID_STRING_LENGTH);
        }
    }
}

/**
 * @see mqtt_interface.h#mqtt_init
 */
u8 mqtt_init(MQTT_INTERFACE* p_mqtt_interface) {

    #ifdef DEBUG_ENABLED
    {
        MQTTClient_setTraceCallback(trace_Callback);
        MQTTClient_setTraceLevel(MQTTCLIENT_TRACE_ERROR);
    }
    #endif

    p_mqtt_interface->connection_lost = 1;
    p_mqtt_interface->initialized = 0;
    p_mqtt_interface->connection_timeout_ms = MQTT_APPLICATION_DEFAULT_CONNECTION_TIMEOUT_MS;
    p_mqtt_interface->keep_alive_interval_ms = MQTT_APPLICATION_DEFAULT_KEEP_ALIVE_TIME_MS;
    p_mqtt_interface->reconnect_interval_ms = MQTT_APPLICATION_DEFAULT_RECONNECT_TIMEOUT_MS;

    return MQTT_NO_ERROR;
}

/**
 * @see mqtt_interface.h#mqtt_connect
 */
u8 mqtt_connect(MQTT_INTERFACE* p_mqtt_interface) {
        
    DEBUG_PASS("mqtt_connect()");

    if (p_mqtt_interface->initialized == 0) {
        DEBUG_TRACE_STR(p_mqtt_interface->client_id, "mqtt_connect() - Initialize MQTT-Client");
        MQTTClient_create(&p_mqtt_interface->client, p_mqtt_interface->host_address, p_mqtt_interface->client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);

        DEBUG_PASS("mqtt_connect() - Set MQTT-Callbacks");
        MQTTClient_setCallbacks(p_mqtt_interface->client, (void*)p_mqtt_interface, connectionLost_Callback, messageArrived_Callback, deliveryComplete_Callback);
    }

    MQTTClient_connectOptions smartHomeConParam = MQTTClient_connectOptions_initializer;
    smartHomeConParam.keepAliveInterval = p_mqtt_interface->keep_alive_interval_ms / 1000;
    smartHomeConParam.cleansession = 1;

    u8 err_code = MQTTClient_connect(p_mqtt_interface->client, &smartHomeConParam);
    if (err_code != MQTTCLIENT_SUCCESS) {
        DEBUG_TRACE_byte(err_code, "mqtt_connect() - MQTTClient_connect() has FAILED !!! (err_code = %d)");
        return MQTT_ERROR_TIMEOUT;

    } else {
        DEBUG_PASS("mqtt_connect() - Connection established.");
    }

    MQTTClient_subscribe(p_mqtt_interface->client, p_mqtt_interface->topic_name, p_mqtt_interface->quality_of_service);
    p_mqtt_interface->connection_lost = 0;

    return MQTT_NO_ERROR;
}

/**
 * @see mqtt_interface.h#mqtt_disconnect
 */
u8 mqtt_disconnect(MQTT_INTERFACE* p_mqtt_interface) {

    DEBUG_PASS("mqtt_disconnect()");

    u8 err_code = MQTTClient_unsubscribe(p_mqtt_interface->client, p_mqtt_interface->topic_name);
    if (err_code != MQTTCLIENT_SUCCESS) {
        DEBUG_TRACE_byte(err_code, "mqtt_disconnect() - MQTTClient_unsubscribe() has FAILED !!! (err_code = %d)");
    } 

    err_code = MQTTClient_disconnect(p_mqtt_interface->client, 5000);
    if (err_code != MQTTCLIENT_SUCCESS) {
        DEBUG_TRACE_byte(err_code, "mqtt_disconnect() - MQTTClient_disconnect() has FAILED !!! (err_code = %d)");
    } 

    //MQTTClient_destroy(p_mqtt_interface->client);

    return MQTT_NO_ERROR;
}

/**
 * @see mqtt_interface.h#mqtt_send_next_message
 */
u8 mqtt_send_next_message(MQTT_INTERFACE* p_mqtt_interface) {

    if (p_mqtt_interface->tx_qeue.is_empty()) {
        DEBUG_PASS("mqtt_send_next_message() - TX-QEUE is empty !!! ---");
        return MQTT_ERROR_QEUE_EMPTY;
    }

    if (p_mqtt_interface->tx_qeue.deqeue(last_msg_send) == 0) {
        DEBUG_PASS("mqtt_send_next_message() - deqeue next message has FAILED !!! ---");
        return MQTT_ERROR_QEUE_ERROR;
    }

    MQTTClient_message mqtt_message = MQTTClient_message_initializer;
    mqtt_message.payload = last_msg_send;
    mqtt_message.payloadlen = strlen(last_msg_send);
    mqtt_message.qos = MQTT_QOS;
    mqtt_message.retained = 0;

    MQTTClient_deliveryToken message_token;

    DEBUG_PASS("mqtt_send_message() - Sending Message to Broker.");
    MQTTClient_publishMessage(p_mqtt_interface->client, p_mqtt_interface->topic_name, &mqtt_message, &message_token);

    DEBUG_PASS("mqtt_send_message() - Waiting for completion of message.");

    u8 err_code = MQTTClient_waitForCompletion(p_mqtt_interface->client, message_token, p_mqtt_interface->connection_timeout_ms);
    if (err_code != MQTTCLIENT_SUCCESS) {
        DEBUG_TRACE_byte(err_code, "mqtt_send_message() - Sending Message to SmartHomeBroker has FAILED !!! (err_code = %d)");
        return MQTT_ERROR_TIMEOUT;
    }

    DEBUG_PASS("mqtt_send_message() - Delivery of Message succeeds.\n");
    return MQTT_NO_ERROR;
}

/**
 * @see mqtt_interface.h#mqtt_msg_pending
 */
u8 mqtt_msg_pending(MQTT_INTERFACE* p_mqtt_interface) {
    return p_mqtt_interface->rx_qeue.is_empty() == 0;
}

/**
 * @see mqtt_interface.h#mqtt_tx_msg_pending
 */
u8 mqtt_tx_msg_pending(MQTT_INTERFACE* p_mqtt_interface) {
    return p_mqtt_interface->tx_qeue.is_empty() == 0;
}

/**
 * @see mqtt_interface.h#mqtt_connection_lost
 */
u8 mqtt_connection_lost(MQTT_INTERFACE* p_mqtt_interface) {
    return p_mqtt_interface->connection_lost;
}

/**
 * @see mqtt_interface.h#mqtt_get_next_msg
 */
u16 mqtt_get_next_msg(MQTT_INTERFACE* p_mqtt_interface, char* p_msg_to, u16 max_length) {

    if (p_mqtt_interface->rx_qeue.is_empty()) {
        DEBUG_PASS("mqtt_get_next_msg() - RX-QEUE is empty !!! ---");
        return 0;
    }

    char t_msg[MQTT_INTERFACE_MAX_MSG_LENGTH];

    memset(t_msg, '\0', MQTT_INTERFACE_MAX_MSG_LENGTH);
    memset(p_msg_to, '\0', max_length);

    if (p_mqtt_interface->rx_qeue.deqeue(t_msg) == 0) {
        DEBUG_PASS("mqtt_get_next_msg() - deqeue() has FAILED !!! ---");
        return 0;
    }

    if (max_length > MQTT_INTERFACE_MAX_MSG_LENGTH) {

        memcpy(p_msg_to, t_msg, MQTT_INTERFACE_MAX_MSG_LENGTH);
        return MQTT_INTERFACE_MAX_MSG_LENGTH;

    } else {

        memcpy(p_msg_to, t_msg, max_length);
        return max_length;
    }
}

/**
 * @see mqtt_interface.h#mqtt_enqeue_message
 */
u8 mqtt_enqeue_message(MQTT_INTERFACE* p_mqtt_interface, const char* p_msg_from) {

    if (p_mqtt_interface->tx_qeue.max_message_length < common_tools_string_length(p_msg_from)) {
        DEBUG_PASS("mqtt_enqeue_message() - Message to long! OVERFLOW !!! ---");
        return 0;
    }

    DEBUG_TRACE_STR(p_msg_from, "mqtt_enqeue_message() - Message: ");
    return p_mqtt_interface->tx_qeue.enqeue(p_msg_from);
}

// --------------------------------------------------------------------------------

/**
 * @see mqtt_interface.h#mqtt_keep_alive
 */
void mqtt_keep_alive(void) {

    DEBUG_PASS("mqtt_keep_alive()");
    MQTTClient_yield();
}

// --------------------------------------------------------------------------------

/**
 * @brief Callback from mqtt subsystem for trace messages
 * if debug is enabled traces are forwarded to debug-outputs.
 * 
 * @param level is ignored
 * @param message message from mqtt-swubsyste, to output.
 */
void trace_Callback(enum MQTTCLIENT_TRACE_LEVELS level, char* message) {
    (void) level;
    (void) message;
    DEBUG_TRACE_STR(message, "mqtt_interface.trace_Callback() - ");
}

/**
 * @brief Callback if a conenction is lost.
 * 
 * @param context mqtt-context where the connection is lost.
 * @param cause reason of connection lost
 */
void connectionLost_Callback(void *context, char* cause) {

    if (context == NULL) {
        return;
    }

    MQTT_INTERFACE* ctx = (MQTT_INTERFACE*) context;

    if (ctx->connection_lost != 0) {
        return;        
    }

    DEBUG_TRACE_STR(cause, "connectionLost_Callback() - MQTT-Connection has been lost !!! --- ");

    common_tools_string_copy_string(ctx->connection_lost_cause, cause, MQTT_INTERFACE_MAX_MSG_LENGTH);
    
    ctx->connection_lost = 1;

}

/**
 * @brief Callback for new messages that have just received.
 * 
 * @param context mqtt-context where the message was received.
 * @param topicName name of the topic at from which the message was received
 * @param topcLength number of char of topicName
 * @param message message that was received.
 * @return 1 in every case
 */
int messageArrived_Callback(void* context, char* topicName, int topcLength, MQTTClient_message* message) {

    (void) topcLength;

    if (context == NULL) {
    
        DEBUG_PASS("messageArrived_Callback() - Context is zero !!! ---");
        return 1;
    }

    if (message->payloadlen == 0) {
    
        DEBUG_PASS("messageArrived_Callback() - Payload-Length is zero !!! ---");
        return 1;
    }

    char t_msg[MQTT_INTERFACE_MAX_MSG_LENGTH];
    u16 length = message->payloadlen > MQTT_INTERFACE_MAX_MSG_LENGTH ? MQTT_INTERFACE_MAX_MSG_LENGTH : message->payloadlen;

    memset(t_msg, '\0', MQTT_INTERFACE_MAX_MSG_LENGTH);
    memcpy(t_msg, message->payload, length);

    MQTTClient_free(topicName);
    MQTTClient_freeMessage(&message); // message not available anymore from here

    if (strlen(last_msg_send) == strlen(t_msg)) {
        if (memcmp(last_msg_send, t_msg, strlen(t_msg)) == 0) {
            DEBUG_PASS("messageArrived_Callback() - This is my last message");
            return 1;
        }
    }

    MQTT_INTERFACE* ctx = (MQTT_INTERFACE*) context;

    ctx->msg_arrived = 1;
    ctx->rx_qeue.enqeue(t_msg);

    return 1;
}

/**
 * @brief Callback for a complete transfer.
 * 
 * @param context mqtt context where a new messge was sent
 * @param token is ignored.
 */
void deliveryComplete_Callback(void* context, MQTTClient_deliveryToken token) {

    (void) token;

    if (context == NULL) {
        return;
    }

    MQTT_INTERFACE* ctx = (MQTT_INTERFACE*) context;
    
    if (ctx->msg_delivered != 0) {
        return;
    }
    
    ctx->msg_delivered = 1;
    DEBUG_PASS("deliveryComplete_Callback() - Message successful delivered !!! ---");

}
