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
 * @file    protocol_json_parser.c
 * @author  Sebastian Lesse
 * @date    2021 / 02 / 05
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
#include "command_management/command_handler_interface.h"

#include "protocol_management/json/protocol_json_parser.h"

// --------------------------------------------------------------------------------

#define JSON_PARSER_TEMP_STRING_LENGTH                              32

#define PROTOCOL_JSON_PARSER_CHARACTER_SEPERATOR                    ','
#define PROTOCOL_JSON_PARSER_STRING_SEPERATOR                       ","

#define PROTOCOL_JSON_PARSER_CHARACTER_GROUP_START                  '{'
#define PROTOCOL_JSON_PARSER_STRING_GROUP_START                     "{"

#define PROTOCOL_JSON_PARSER_CHARACTER_GROUP_END                    '}'
#define PROTOCOL_JSON_PARSER_STRING_GROUP_END                       "}"

#define PROTOCOL_JSON_PARSER_CHARACTER_KEY_VALUE_SEPERATOR          ':'
#define PROTOCOL_JSON_PARSER_STRING_KEY_VALUE_SEPERATOR             ":"

#define PROTOCOL_JSON_PARSER_CHARACTER_NAME_SEPERATOR               '\"'
#define PROTOCOL_JSON_PARSER_STRING_NAME_SEPERATOR                  "\""

// --------------------------------------------------------------------------------

#define JSON_OBJECT_STATUS_COMPLETE                                 (1 << 0)
#define JSON_OBJECT_STATUS_ACTIVE                                   (1 << 1)
#define JSON_OBJECT_STATUS_IDLE                                     (1 << 2)
#define JSON_OBJECT_STATUS_ERROR                                    (1 << 3)

// --------------------------------------------------------------------------------

/**
 * @brief Adds a seperator to the json object to add a new value
 * 
 * @param p_json_object json object where to add the seperator
 * @return 1 if speperator was added, 0 if string is full
 */
static u8 json_parser_add_seperator(JSON_OPJECT_TYPE* p_json_object) {

    if (shared_memory_available(&p_json_object->memory_ctx) == 0) {
        DEBUG_PASS("json_parser_add_seperator() - OVERFLOW !!!");
        return 0;
    }

    // if (p_json_object->length + 1 >= PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH) {
    //     DEBUG_PASS("json_parser_add_seperator() - OVERFLOW !!!");
    //     return 0;
    // }

    if (common_tools_string_ends_with((const char*) shared_memory_get(&p_json_object->memory_ctx), PROTOCOL_JSON_PARSER_CHARACTER_GROUP_START)) {
        DEBUG_PASS("json_parser_add_seperator() - start of group or string - don't add a seperator");
        return 1;
    }

    if (common_tools_string_ends_with((const char*) shared_memory_get(&p_json_object->memory_ctx), PROTOCOL_JSON_PARSER_CHARACTER_SEPERATOR)) {
        DEBUG_PASS("json_parser_add_seperator() - Seperator already existing");
        return 1;
    }

    // DEBUG_TRACE_STR(p_json_object->string_buffer, "json_parser_add_seperator() - Appending seperator");
    DEBUG_TRACE_STR(
        shared_memory_get(&p_json_object->memory_ctx),
        "json_parser_add_seperator() - Appending seperator"
    );

    /**
     * @brief Adds the following:
     * 
     * ','
     * 
     */

    // p_json_object->length = common_tools_string_append(p_json_object->string_buffer, PROTOCOL_JSON_PARSER_STRING_SEPERATOR, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);


    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_SEPERATOR) == 0) {
        DEBUG_PASS("json_parser_add_seperator() - adding seperator FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return 0;
    }

    return 1;
}

/**
 * @brief Appends a name to the actual string in the form of "name".
 * Name can be the name of a group or a specific value.
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

    /**
     * @brief Adds the following:
     * 
     * '"<name>"'
     * 
     */

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_NAME_SEPERATOR) == 0) {
        DEBUG_PASS("json_parser_add_name() - adding start seperator FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return 0;
    }

    u16 name_len = common_tools_string_length(name);
    if (shared_memory_add_data(&p_json_object->memory_ctx, (const u8*)name, name_len) == 0) {
        DEBUG_PASS("json_parser_add_name() - adding name FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return 0;
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_NAME_SEPERATOR) == 0) {
        DEBUG_PASS("json_parser_add_name() - adding end seperator FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return 0;
    }

    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);
    return 1;
}

// --------------------------------------------------------------------------------

// {"Time":"2021-02-05T10:49:50","ENERGY":{"TotalStartTime":"2020-12-30T16:13:50","Total":0.170,"Yesterday":0.003,"Today":0.000,"Period":0,"Power":0,"ApparentPower":0,"ReactivePower":0,"Factor":0.00,"Voltage":0,"Current":0.000}}

/**
 * @see protocol_json_parser.h
 */
u8 json_parser_initialize(JSON_OPJECT_TYPE* p_json_object) {

    DEBUG_PASS("json_parser_initialize()");

    if ((p_json_object->status == JSON_OBJECT_STATUS_IDLE) || (p_json_object->status == 0)) {

        DEBUG_PASS("json_parser_initialize() - allocating memory");

        p_json_object->memory_ctx.handle = SHARED_MEMORY_INVALID_HANDLE;
        u8 ret_val = shared_memory_allocate(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH);

        if (ret_val == SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE) {
            DEBUG_PASS("json_parser_initialize() - allocating memory FAIELD!");
            return 0;
        }
    }

    p_json_object->status = JSON_OBJECT_STATUS_ACTIVE;
    p_json_object->open_group_count = 0;

    json_parser_clear(p_json_object);

    return 1;
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_deinitialize(JSON_OPJECT_TYPE* p_json_object) {

    if (p_json_object->status == JSON_OBJECT_STATUS_IDLE) {
        DEBUG_PASS("json_parser_deinitialize() - object is not active");
        return;
    }

    p_json_object->status = JSON_OBJECT_STATUS_IDLE;
    shared_memory_free(&p_json_object->memory_ctx);

}

/**
 * @see protocol_json_parser.h
 */
void json_parser_clear(JSON_OPJECT_TYPE* p_json_object) {

    if (p_json_object->status == JSON_OBJECT_STATUS_IDLE) {
        DEBUG_PASS("json_parser_clear() - object is not active");
        return;
    }

    shared_memory_reset(&p_json_object->memory_ctx);
    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_GROUP_START)) {

        p_json_object->length = 1;
        p_json_object->open_group_count = 0;
        p_json_object->status = JSON_OBJECT_STATUS_ACTIVE;

        DEBUG_PASS("json_parser_clear() - successful");

    } else {

        DEBUG_PASS("json_parser_clear() - FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
    }
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_start_group(JSON_OPJECT_TYPE* p_json_object, const char* group_name) {

    if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
        DEBUG_PASS("json_parser_start_group() - JSON object is already complete !!!");
        return;
    }

    if (json_parser_add_seperator(p_json_object) == 0) {
        DEBUG_PASS("json_parser_start_group() - Adding seperator has FAILED !!!");
        return;
    }

    /**
     * @brief Adds the following:
     * 
     * '<group_name>:{'
     */

    if (json_parser_add_name(p_json_object, group_name) == 0) {
        DEBUG_PASS("json_parser_start_group() - Adding Group-Name has FAILED !!!");
        return;
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_KEY_VALUE_SEPERATOR) == 0) {
        DEBUG_PASS("json_parser_start_group() - adding seperator FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_GROUP_START) == 0) {
        DEBUG_PASS("json_parser_start_group() - adding start of group FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);
    p_json_object->open_group_count += 1;

    // DEBUG_TRACE_N(PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH, shared_memory_get(&p_json_object->memory_ctx), "json_parser_start_group() - Content:");
    DEBUG_TRACE_byte(p_json_object->open_group_count, "json_parser_start_group() - Open groups: >>>>>>>>>>>>>>>");
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_add_integer(JSON_OPJECT_TYPE* p_json_object, const char* name, i32 value) {

    if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
        DEBUG_PASS("json_parser_add_integer() - JSON object is already complete !!!");
        return;
    }

    DEBUG_PASS("json_parser_add_value()");

    /**
     * @brief Adds the following:
     * 
     * ,"<name>":<value>
     * 
     */

    if (json_parser_add_seperator(p_json_object) == 0) {
        DEBUG_PASS("json_parser_add_value() - adding seperator FAILED !!!");
        return;
    }

    if (json_parser_add_name(p_json_object, name) == 0) {
        DEBUG_PASS("json_parser_add_value() - adding name FAILED !!!");
        return;
    }

    char temp_string[JSON_PARSER_TEMP_STRING_LENGTH];

    common_tools_string_clear(temp_string, JSON_PARSER_TEMP_STRING_LENGTH);
    u16 temp_string_len = (u16) common_tools_string_from_i32(temp_string, JSON_PARSER_TEMP_STRING_LENGTH, value);

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_KEY_VALUE_SEPERATOR) == 0) {
        DEBUG_PASS("json_parser_add_integer() - adding seperator FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    if (shared_memory_add_data(&p_json_object->memory_ctx, (const u8*)temp_string, temp_string_len) == 0) {
        DEBUG_PASS("json_parser_add_integer() - adding value FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);

    DEBUG_TRACE_STR(
        shared_memory_get(&p_json_object->memory_ctx),
        "json_parser_add_integer() - New Json string:"
    );
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_add_string(JSON_OPJECT_TYPE* p_json_object, const char* name, const char* p_string) {

    if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
        DEBUG_PASS("json_parser_add_integer() - JSON object is already complete !!!");
        return;
    }

    DEBUG_PASS("json_parser_add_string()");

    /**
     * @brief Adds the following:
     * 
     * ,"<name>":"<p_string>"
     * 
     */

    if (json_parser_add_seperator(p_json_object) == 0) {
        DEBUG_PASS("json_parser_add_string() - Adding seperator has FAILED !!!");
        return;
    }

    if (json_parser_add_name(p_json_object, name) == 0) {
        DEBUG_PASS("json_parser_add_string() - Adding Value-Name has FAILED !!!");
        return;
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_KEY_VALUE_SEPERATOR)) {
        p_json_object->length += 1;
    }

    if (json_parser_add_name(p_json_object, p_string) == 0) {
        DEBUG_PASS("json_parser_add_string() - Adding string has FAILED !!!");
        return;
    }

    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_end_group(JSON_OPJECT_TYPE* p_json_object) {

    if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
        DEBUG_PASS("json_parser_end_group() - JSON object is already complete !!!");
        return;
    }

    if (p_json_object->open_group_count == 0) {
        DEBUG_PASS("json_parser_end_group() - No active group !!!");
        return;
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_GROUP_END) == 0) {
        DEBUG_PASS("json_parser_end_group() - Adding group end FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    p_json_object->open_group_count -= 1;
    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);

    DEBUG_TRACE_byte(p_json_object->open_group_count, "json_parser_end_group() - Groups remaining: <<<<<<<<<<<<<");
}

/**
 * @see protocol_json_parser.h
 */
void json_parser_finish(JSON_OPJECT_TYPE* p_json_object) {

    if (p_json_object->status == JSON_OBJECT_STATUS_COMPLETE) {
        DEBUG_PASS("json_parser_finish() - JSON object is already complete !!!");
        return;
    }

    while (p_json_object->open_group_count > 0) {
        json_parser_end_group(p_json_object);
    }

    if (shared_memory_add_u8(&p_json_object->memory_ctx, PROTOCOL_JSON_PARSER_CHARACTER_GROUP_END) == 0) {
        DEBUG_PASS("json_parser_finish() - Adding group end FAILED!");
        p_json_object->status = JSON_OBJECT_STATUS_ERROR;
        return;
    }

    DEBUG_PASS("json_parser_finish()");
    p_json_object->status = JSON_OBJECT_STATUS_COMPLETE;
    p_json_object->length = shared_memory_occupied(&p_json_object->memory_ctx);
}

/**
 * @see protocol_json_parser.h
 */
u16 json_parser_copy_to(JSON_OPJECT_TYPE* p_json_object, char* p_string, u16 max_length) {

    if (p_json_object->length > max_length) {
        DEBUG_PASS("json_parser_copy_to() - Destination too short");
        return 0;
    }

    DEBUG_PASS("json_parser_copy_to()");

    p_string[p_json_object->length] = 0;
    return shared_memory_copy_to(&p_json_object->memory_ctx, 0, (u8*)p_string, p_json_object->length);
}

/**
 * @see protocol_json_parser.h
 */
u16 json_parser_get_length(JSON_OPJECT_TYPE* p_json_object) {

    DEBUG_PASS("json_parser_get_length()");
    return p_json_object->length;
}

/**
 * @see protocol_json_parser.h
 */
u8 json_parser_is_active(JSON_OPJECT_TYPE* p_json_object) {

    DEBUG_TRACE_byte(p_json_object->status, "json_parser_is_active()");
    return p_json_object->status == JSON_OBJECT_STATUS_ACTIVE ? 1 : 0;
}

/**
 * @see protocol_json_parser.h
 */
u8 json_parser_is_complete(JSON_OPJECT_TYPE* p_json_object) {

    DEBUG_TRACE_byte(p_json_object->status, "json_parser_is_complete()");
    return p_json_object->status == JSON_OBJECT_STATUS_COMPLETE ? 1 : 0;
}

/**
 * @see protocol_json_parser.h
 */
const char* json_parser_to_string(JSON_OPJECT_TYPE* p_json_object) {
    return (const char*)shared_memory_get(&p_json_object->memory_ctx);
}

// --------------------------------------------------------------------------------
