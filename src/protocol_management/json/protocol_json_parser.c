
/**
 * @file 	protocol_json_parser.c
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

/**
 * @brief Adds a seperator to the json object to add a new value
 * 
 * @param p_json_object json object where to add the seperator
 * @return 1 if speperator was added, 0 if string is full
 */
static u8 json_parser_add_seperator(JSON_OPJECT_TYPE* p_json_object) {

	if (p_json_object->length + 1 >= PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH) {
		DEBUG_PASS("json_parser_add_seperator() - OVERFLOW !!!");
		return 0;
	}

	if (common_tools_string_ends_with((const char*) p_json_object->string_buffer, PROTOCOL_JSON_PARSER_CHARACTER_GROUP_START)) {
		DEBUG_PASS("json_parser_add_seperator() - start of group or string - don't add a seperator");
		return 1;
	}

	if (common_tools_string_ends_with((const char*) p_json_object->string_buffer, PROTOCOL_JSON_PARSER_CHARACTER_SEPERATOR)) {
		DEBUG_PASS("json_parser_add_seperator() - Seperator already existing");
		return 1;
	}

	DEBUG_TRACE_STR(p_json_object->string_buffer, "json_parser_add_seperator() - Appending seperator");

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	return 1;
}

/**
 * @brief Appends a name to the actual string in the form of "name". Name can be the name of a group or a specific value.
 * 
 * @param p_json_object Json object where the name will be added at the end
 * @param name the name to add as string
 * @return u8 
 */
static u8 json_parser_add_name(JSON_OPJECT_TYPE* p_json_object, const char* name) {

	u16 new_length = common_tools_string_length(name) + 2;
	if (new_length >= PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH) {
		DEBUG_PASS("json_parser_add_name() - OVERFLOW !!!");
		return 0;
	}

	DEBUG_TRACE_STR(name, "json_parser_add_name()");

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_NAME_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, name, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_NAME_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	return 1;
}

// --------------------------------------------------------------------------------

// {"Time":"2021-02-05T10:49:50","ENERGY":{"TotalStartTime":"2020-12-30T16:13:50","Total":0.170,"Yesterday":0.003,"Today":0.000,"Period":0,"Power":0,"ApparentPower":0,"ReactivePower":0,"Factor":0.00,"Voltage":0,"Current":0.000}}

void json_parser_initialize(JSON_OPJECT_TYPE* p_json_object) {

	if (p_json_object->status == JSON_OBJECT_STATUS_ACTIVE) {
		DEBUG_PASS("json_parser_initialize() - is already active");
		return;
	}

	DEBUG_PASS("json_parser_initialize()");

	common_tools_string_clear(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_GROUP_START, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	p_json_object->status = JSON_OBJECT_STATUS_ACTIVE;
	p_json_object->open_group_count = 0;
}

void json_parser_start_group(JSON_OPJECT_TYPE* p_json_object, const char* group_name) {

	if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
		DEBUG_PASS("json_parser_start_group() - JSON object is already complete !!!");
		return;
	}

	if (json_parser_add_seperator(p_json_object) == 0) {
		DEBUG_PASS("json_parser_start_group() - Adding seperator has FAILED !!!");
		return;
	}

	if (json_parser_add_name(p_json_object, group_name) == 0) {
		DEBUG_PASS("json_parser_start_group() - Adding Group-Name has FAILED !!!");
		return;
	}

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_KEY_VALUE_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_GROUP_START, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	p_json_object->open_group_count += 1;

	DEBUG_TRACE_byte(p_json_object->open_group_count, "json_parser_start_group() - Open groups: >>>>>>>>>>>>>>>");
}

void json_parser_add_integer(JSON_OPJECT_TYPE* p_json_object, const char* name, i32 value) {

	if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
		DEBUG_PASS("json_parser_add_integer() - JSON object is already complete !!!");
		return;
	}

	DEBUG_PASS("json_parser_add_value()");

	if (json_parser_add_seperator(p_json_object) == 0) {
		DEBUG_PASS("json_parser_add_value() - Adding seperator has FAILED !!!");
		return;
	}

	if (json_parser_add_name(p_json_object, name) == 0) {
		DEBUG_PASS("json_parser_add_value() - Adding Value-Name has FAILED !!!");
		return;
	}

	char temp_string[JSON_PARSER_TEMP_STRING_LENGTH];

	common_tools_string_clear(temp_string, JSON_PARSER_TEMP_STRING_LENGTH);
	common_tools_string_from_i32(temp_string, value);

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_KEY_VALUE_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, temp_string, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	DEBUG_TRACE_STR(p_json_object->string_buffer, "json_parser_add_integer() - New Json string:");
}

void json_parser_add_string(JSON_OPJECT_TYPE* p_json_object, const char* name, const char* p_string) {

	if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
		DEBUG_PASS("json_parser_add_integer() - JSON object is already complete !!!");
		return;
	}

	DEBUG_PASS("json_parser_add_string()");

	if (json_parser_add_seperator(p_json_object) == 0) {
		DEBUG_PASS("json_parser_add_string() - Adding seperator has FAILED !!!");
		return;
	}

	if (json_parser_add_name(p_json_object, name) == 0) {
		DEBUG_PASS("json_parser_add_string() - Adding Value-Name has FAILED !!!");
		return;
	}

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_KEY_VALUE_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

	if (json_parser_add_name(p_json_object, p_string) == 0) {
		DEBUG_PASS("json_parser_add_string() - Adding string has FAILED !!!");
		return;
	}
}

void json_parser_end_group(JSON_OPJECT_TYPE* p_json_object) {

	if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
		DEBUG_PASS("json_parser_end_group() - JSON object is already complete !!!");
		return;
	}

	if (p_json_object->open_group_count == 0) {
		DEBUG_PASS("json_parser_end_group() - No active group !!!");
		return;
	}

	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_GROUP_END, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->open_group_count -= 1;

	DEBUG_TRACE_byte(p_json_object->open_group_count, "json_parser_end_group() - Groups remaining: <<<<<<<<<<<<<");
}

void json_parser_finish(JSON_OPJECT_TYPE* p_json_object) {

	if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
		DEBUG_PASS("json_parser_finish() - JSON object is already complete !!!");
		return;
	}

	while (p_json_object->open_group_count > 0) {
		json_parser_end_group(p_json_object);
	}

	DEBUG_PASS("json_parser_finish()");
	p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_GROUP_END, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);
	p_json_object->status = JSON_OBJECT_STATUS_COMPLETE;
}

u16 json_parser_copy_to(JSON_OPJECT_TYPE* p_json_object, char* p_string, u16 max_length) {

	DEBUG_PASS("json_parser_copy_to()");
	return common_tools_string_copy_string(p_string, (const char*) p_json_object->string_buffer, max_length);
}

u16 json_parser_get_length(JSON_OPJECT_TYPE* p_json_object) {

	DEBUG_PASS("json_parser_copy_to()");
	return p_json_object->length;
}

u8 json_parser_is_active(JSON_OPJECT_TYPE* p_json_object) {

	DEBUG_TRACE_byte(p_json_object->status, "json_parser_is_active()");
	return p_json_object->status == JSON_OBJECT_STATUS_ACTIVE ? 1 : 0;
}

u8 json_parser_is_complete(JSON_OPJECT_TYPE* p_json_object) {

	DEBUG_TRACE_byte(p_json_object->status, "json_parser_is_complete()");
	return p_json_object->status == JSON_OBJECT_STATUS_COMPLETE ? 1 : 0;
}
