
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
#include "command_management/command_handler_interface.h"

#include "protocol_management/json/protocol_json_parser.h"

// --------------------------------------------------------------------------------

#define JSON_PARSER_TEMP_STRING_LENGTH						32

// --------------------------------------------------------------------------------

#define JSON_RESPONSE_PARSER_STRING_VERSION				"VERSION"
#define JSON_RESPONSE_PARSER_STRING_TEMPERATURE				"TEMPERATURE"
#define JSON_RESPONSE_PARSER_STRING_HUMIDITY				"HUMIDITY"
#define JSON_RESPONSE_PARSER_STRING_AMBILIGHT				"AMBILIGHT"

#define JSON_RESPONSE_PARSER_STRING_ACT					"ACT"
#define JSON_RESPONSE_PARSER_STRING_MAX					"MAX"
#define JSON_RESPONSE_PARSER_STRING_MIN					"MIN"
#define JSON_RESPONSE_PARSER_STRING_MEAN				"MEAN"

// --------------------------------------------------------------------------------

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
static void json_parser_response_add_act_max_min_mean(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer->length > 2) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_ACT, (i32) p_com_buffer->data[2]);
	}

	if (p_com_buffer->length > 3) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MAX, (i32) p_com_buffer->data[3]);
	}
	
	if (p_com_buffer->length > 4) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MIN, (i32) p_com_buffer->data[4]);
	}
	
	if (p_com_buffer->length > 5) {
		json_parser_add_integer(p_json_object, JSON_RESPONSE_PARSER_STRING_MEAN, (i32) p_com_buffer->data[5]);
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
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer);
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
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer);
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
	json_parser_response_add_act_max_min_mean(p_json_object, p_com_buffer);
	json_parser_end_group(p_json_object);
}

// --------------------------------------------------------------------------------

void json_parser_append_rpi_cmd_response(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {

	if (p_com_buffer == NULL) {
		DEBUG_PASS("json_parser_append_rpi_cmd_response() - NULL_POINTER_EXCEPTION !!!");
		return;
	}

	DEBUG_PASS("json_parser_append_rpi_cmd_response()");

	switch (CMD_HANDLER_GET_RESPONSE_COMMAND_CODE(p_com_buffer)) {

		default : break;

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

		/*
		case RPI_COMMAND_GET_LIGHT:
		case RPI_COMMAND_GET_INPUT_LIST:
		case RPI_COMMAND_GET_OUTPUT_LIST:
		case RPI_COMMAND_SET_OUTPUT:
		case RPI_COMMAND_GET_OUTPUT_STATE:
		case RPI_COMMAND_GET_INPUT_STATE:
		case RPI_COMMAND_GET_ADC:
		case RPI_COMMAND_IR_REMOTE:
		case RPI_COMMAND_ROUTING:
		*/
	}
}
