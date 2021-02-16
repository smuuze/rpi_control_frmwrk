
/**
 * @file 	protocol_json_response_parser.c
 * @author 	sebastian lesse
 * @brief 	Creates a JSon string from RPi-Command Response
 * @version 	1.0
 * @date 	2021-02-05
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_string.h"
#include "common/common_tools_number.h"
#include "command_management/command_handler_interface.h"
#include "protocol_management/json/protocol_json_parser.h"

// --------------------------------------------------------------------------------

#define JSON_PARSER_TEMP_STRING_LENGTH						16
#define JSON_PARSER_TEMP_STRING_LONG_LENGTH					128

// --------------------------------------------------------------------------------

#define JSON_RESPONSE_PARSER_STRING_VERSION				"VERSION"
#define JSON_RESPONSE_PARSER_STRING_TEMPERATURE				"TEMPERATURE"
#define JSON_RESPONSE_PARSER_STRING_HUMIDITY				"HUMIDITY"
#define JSON_RESPONSE_PARSER_STRING_AMBILIGHT				"AMBILIGHT"
#define JSON_RESPONSE_PARSER_STRING_OUTPUT_STATE			"OUTPUT_STATE_"
#define JSON_RESPONSE_PARSER_STRING_SET_OUTPUT_STATE			"SET_OUTPUT_STATE"
#define JSON_RESPONSE_PARSER_STRING_PIN_STATE				"PIN_STATE"
#define JSON_RESPONSE_PARSER_STRING_ON_TIME				"ON_TIME_MS"
#define JSON_RESPONSE_PARSER_STRING_TOGGLE_DURATION			"DURATION_MS"
#define JSON_RESPONSE_PARSER_STRING_TOGGLE_PERIOD			"PERIOD_MS"

#define JSON_RESPONSE_PARSER_STRING_ERROR				"ERR"

#define JSON_RESPONSE_PARSER_STRING_ACT					"ACT"
#define JSON_RESPONSE_PARSER_STRING_MAX					"MAX"
#define JSON_RESPONSE_PARSER_STRING_MIN					"MIN"
#define JSON_RESPONSE_PARSER_STRING_MEAN				"MEAN"
#define JSON_RESPONSE_PARSER_STRING_ON					"ON"
#define JSON_RESPONSE_PARSER_STRING_OFF					"OFF"

#define JSON_PARSER_CLI_RESPONSE_SPLITTER_CHAR				'='

// --------------------------------------------------------------------------------

static void json_parser_resposne_error_code(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	DEBUG_TRACE_byte(CMD_HANDLER_GET_RESPONSE_ERROR_CODE(p_com_buffer), "json_parser_resposne_error_code()");

	switch (CMD_HANDLER_GET_RESPONSE_ERROR_CODE(p_com_buffer)) {
		default:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "UNKNOWN");
			break;

		case CMD_NO_ERR :
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "OK");
			break;

		case CMD_ERR_INVALID_ARGUMENT:
		case CMD_ERR_INVARG:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "INVALID ARGUMENT");
			break;
			
		case CMD_ERR_INVALID_COMMAND:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "INVALID COMMAND");
			break;
			
		case CMD_ERR_TIMEOUT:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "TIMEOUT");
			break;
			
		case CMD_ERR_BUSY:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "BUSY");
			break;
			
		case CMD_ERR_NO_ROUTE_TO_HOST:
			json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_ERROR, "NO_ROUTE_TO_HOST");
			break;
			
	}
}

/**
 * @brief	Adds the response of the version command to the actual json object
 * 		Version is added as string value in the form "VERSION":"5.0".
 * @param p_json_object json object where the version response is added to
 * @param p_com_buffer command buffer holding the response of the version-command
 */
static void json_parser_response_version(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer->length < 4) {
		return;
	}

	char version_major_string[4];
	char version_minor_string[4];
	char version_string[JSON_PARSER_TEMP_STRING_LENGTH];

	common_tools_string_clear(version_major_string, 4);
	common_tools_string_clear(version_minor_string, 4);
	common_tools_string_clear(version_string, JSON_PARSER_TEMP_STRING_LENGTH);

	common_tools_string_from_u8(version_major_string, p_com_buffer->data[2]);
	common_tools_string_from_u8(version_minor_string, p_com_buffer->data[3]);

	common_tools_string_append(version_string, version_major_string, JSON_PARSER_TEMP_STRING_LENGTH);
	common_tools_string_append(version_string, ".", JSON_PARSER_TEMP_STRING_LENGTH);
	common_tools_string_append(version_string, version_minor_string, JSON_PARSER_TEMP_STRING_LENGTH);

	json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_VERSION, version_string);
}

/**
 * @brief 	Adds the ACT / MAX / MIN / MEAN values of a Temperature / Humidity / Ambilight command response
 * 
 * @param p_json_object json object where to add the values
 * @param p_com_buffer ommand buffer holding the response of the command
 */
static void json_parser_response_add_act_max_min_mean(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer, u8 is_signed) {

	if (p_com_buffer->length > 2) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_ACT, common_tools_number_from_u8(p_com_buffer->data[2], is_signed));
	}

	if (p_com_buffer->length > 3) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MAX, common_tools_number_from_u8(p_com_buffer->data[3], is_signed));
	}
	
	if (p_com_buffer->length > 4) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MIN, common_tools_number_from_u8(p_com_buffer->data[4], is_signed));
	}
	
	if (p_com_buffer->length > 5) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MEAN, common_tools_number_from_u8(p_com_buffer->data[5], is_signed));
	}
}

/**
 * @brief 	Adds the reponse of the get-temperature command to the actual json object
 * 		Temperature is added as group in the form "TEMPERATURE":{"ACT":20,"MAX":27,"MIN":-1,"MEAN":15}
 * @param p_json_object json object where the temperature response is added to
 * @param p_com_buffer command buffer holding the response of the get-temperature command
 */
static void json_parser_response_temperature(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	json_parser_start_group(p_json_object, JSON_RESPONSE_PARSER_STRING_TEMPERATURE);
	json_parser_resposne_error_code(p_json_object, p_com_buffer);
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer, 1 /* signed */);
	json_parser_end_group(p_json_object);
}

/**
 * @brief 	Adds the reponse of the get-humidity command to the actual json object
 * 		Humidity is added as group in the form "HUMIDITY":{"ACT":50,"MAX":70,"MIN":39,"MEAN":45}
 * @param p_json_object json object where the humidity response is added to
 * @param p_com_buffer command buffer holding the response of the get-humidity command
 */
static void json_parser_response_humidity(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer->length < 3) {
		return;
	}

	json_parser_start_group(p_json_object, JSON_RESPONSE_PARSER_STRING_HUMIDITY);
	json_parser_resposne_error_code(p_json_object, p_com_buffer);
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer, 0 /* unsigned */);
	json_parser_end_group(p_json_object);
}

/**
 * @brief 	Adds the reponse of the get-ambilight command to the actual json object
 * 		Ambilight is added as group in the form "AMBILIGHT":{"ACT":50,"MAX":70,"MIN":39,"MEAN":45}
 * @param p_json_object json object where the ambilight response is added to
 * @param p_com_buffer command buffer holding the response of the get-ambilight command
 */
static void json_parser_response_ambilight(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer->length < 3) {
		return;
	}

	json_parser_start_group(p_json_object, JSON_RESPONSE_PARSER_STRING_AMBILIGHT);
	json_parser_resposne_error_code(p_json_object, p_com_buffer);
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer, 0 /* unsigned */);
	json_parser_end_group(p_json_object);
}

/**
 * @brief 	Adds the reponse of the get-output-state command to the actual json object
 * 		Output-state is added as group in the form "OUTPUT_STATE_1":{"PIN_STATE":"ON/OFF","ON_TIME":"HH:MM:SS","TOGGLE_PERIOD":1234L,"TOGGLE_DURATION":5678L}
 * 
 * @param p_json_object json object where the output-state response is added to
 * @param p_com_buffer command buffer holding the response of the get-output-state command
 */
static void json_parser_response_output_state(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer->length < 3) {
		return;
	}

	// Response to parse:
	// POS:		|   00     |    01    |      02      |    03     |  04 05 06 07 |  08 09 10 11  |   12 13 14 15   |
	// DATA:	|   05     |    00    |      01      |    01     |  BB 32 2B 00 |  00 00 00 00  |   00 00 00 00   |
	// NAMES:	| cmd-code | err-code | output-index | pin-state |    on-time   | toggle-period | toggle-duration |

	char group_name[JSON_PARSER_TEMP_STRING_LENGTH];
	char index_str[JSON_PARSER_TEMP_STRING_LENGTH];

	common_tools_string_clear(group_name, JSON_PARSER_TEMP_STRING_LENGTH);
	common_tools_string_clear(index_str, JSON_PARSER_TEMP_STRING_LENGTH);

	common_tools_string_from_u8(index_str, p_com_buffer->data[2]);

	common_tools_string_append(group_name, JSON_RESPONSE_PARSER_STRING_OUTPUT_STATE, JSON_PARSER_TEMP_STRING_LENGTH);
	common_tools_string_append(group_name, index_str, JSON_PARSER_TEMP_STRING_LENGTH);

	json_parser_start_group(p_json_object, group_name);

	json_parser_resposne_error_code(p_json_object, p_com_buffer);

	if (p_com_buffer->data[3] != 0) {
		json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_PIN_STATE, JSON_RESPONSE_PARSER_STRING_ON);
	} else {
		json_parser_add_string(p_json_object, JSON_RESPONSE_PARSER_STRING_PIN_STATE, JSON_RESPONSE_PARSER_STRING_OFF);
	}

	json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_ON_TIME, common_tools_number_u32_from_array(p_com_buffer->data + 4));
	json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_TOGGLE_DURATION, common_tools_number_u32_from_array(p_com_buffer->data + 8));
	json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_TOGGLE_PERIOD, common_tools_number_u32_from_array(p_com_buffer->data + 12));

	json_parser_end_group(p_json_object);
}

/**
 * @brief 
 * 
 * @param p_json_object 
 * @param p_com_buffer 
 */
static void json_parser_response_set_output_state(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	json_parser_start_group(p_json_object, JSON_RESPONSE_PARSER_STRING_SET_OUTPUT_STATE);
	json_parser_resposne_error_code(p_json_object, p_com_buffer);
	json_parser_end_group(p_json_object);
}

/**
 * @brief 
 * 
 * @param p_json_object 
 * @param p_com_buffer 
 */
static void json_parser_response_bad_command(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	DEBUG_PASS("json_parser_append_rpi_cmd_response() - BAD_COMMAND");

	char t_string[4];
	common_tools_string_clear(t_string, 4);
	common_tools_string_from_u8(t_string, CMD_HANDLER_GET_RESPONSE_COMMAND_CODE(p_com_buffer));

	json_parser_start_group(p_json_object, t_string);
	json_parser_add_string(p_json_object, "ERR", "BAD_COMMAND");
	json_parser_end_group(p_json_object);
}

// --------------------------------------------------------------------------------

void json_parser_append_cli_cmd_response(JSON_OPJECT_TYPE* p_json_object, const char*p_cli_response) {

	char cmd_name[JSON_PARSER_TEMP_STRING_LONG_LENGTH];
	char cmd_response[JSON_PARSER_TEMP_STRING_LONG_LENGTH];

	common_tools_string_split(JSON_PARSER_CLI_RESPONSE_SPLITTER_CHAR, p_cli_response, cmd_name, JSON_PARSER_TEMP_STRING_LONG_LENGTH, cmd_response, JSON_PARSER_TEMP_STRING_LONG_LENGTH);
	json_parser_add_string(p_json_object, cmd_name, cmd_response);
}

void json_parser_append_rpi_cmd_response(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer == NULL) {
		DEBUG_PASS("json_parser_append_rpi_cmd_response() - NULL_POINTER_EXCEPTION !!!");
		return;
	}

	DEBUG_TRACE_byte(CMD_HANDLER_GET_RESPONSE_COMMAND_CODE(p_com_buffer), "json_parser_append_rpi_cmd_response()");

	switch (CMD_HANDLER_GET_RESPONSE_COMMAND_CODE(p_com_buffer)) {

		default : 
			json_parser_response_bad_command(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_GET_VERSION:
			json_parser_response_version(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_GET_TEMPERATURE:
			json_parser_response_temperature(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_GET_HUMIDTY:
			json_parser_response_humidity(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_GET_LIGHT:
			json_parser_response_ambilight(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_GET_OUTPUT_STATE:
			json_parser_response_output_state(p_json_object, p_com_buffer);
			break;

		case RPI_COMMAND_SET_OUTPUT:
			json_parser_response_set_output_state(p_json_object, p_com_buffer);
			break;


		/*
		case RPI_COMMAND_GET_LIGHT:
		case RPI_COMMAND_GET_INPUT_LIST:
		case RPI_COMMAND_GET_OUTPUT_LIST:
		case RPI_COMMAND_GET_OUTPUT_STATE:
		case RPI_COMMAND_GET_INPUT_STATE:
		case RPI_COMMAND_GET_ADC:
		case RPI_COMMAND_IR_REMOTE:
		case RPI_COMMAND_ROUTING:
		*/
	}
}
