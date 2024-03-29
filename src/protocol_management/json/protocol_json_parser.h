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
 * @file   protocol_json_parser.h
 * @author Sebastian Lesse
 * @date   2021 / 02 / 05
 * @brief  Interface to create an static isntance of a JSON string parser
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _PROTOCOL_JSON_PARSER_H_
#define _PROTOCOL_JSON_PARSER_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "common/common_types.h"

// --------------------------------------------------------------------------------

/**
 * @brief Maximum number of characters that can be added to a json string
 * 
 */
#ifndef PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH
#define PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH                1024
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct JSON_OBJECT_STRUCT {
    char  string_buffer[PROTOCOL_JSON_PARSER_STRING_BUFFER_MAX_LENGTH];
    u16 length;
    u8 open_group_count;
    u8 status;
} JSON_OPJECT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Initialized the json-object for usage. Data of an already initialized object gets lost.
 * 
 * @param p_json_object The json-object to initialize.
 */
void json_parser_initialize(JSON_OPJECT_TYPE* p_json_object);

/**
 * @brief Parses the response of a RPi-Protocol command into its json representation.
 * 
 * @param p_json_object The Json object where the response is added. The object must be initialized
 * @param p_com_buffer byte-array that holds the command-response
 */
void json_parser_append_rpi_cmd_response(JSON_OPJECT_TYPE* p_json_object, COMMON_GENERIC_BUFFER_TYPE* p_com_buffer);

/**
 * @brief Parses the response of a Command-Line-Executer command into its json representation
 * 
 * @param p_json_object  The Json object where the response is added. The object must be initialized
 * @param p_command name of the command that was executed as string
 * @param p_response response of the command that was executed as string
 */
void json_parser_append_cli_cmd_response(JSON_OPJECT_TYPE* p_json_object, const char* p_command, const char* p_response);

/**
 * @brief Starts a new group with the given name.
 * 
 * @param p_json_object The Json object to use for the new group entry
 * @param group_name name of the new group as string
 */
void json_parser_start_group(JSON_OPJECT_TYPE* p_json_object, const char* group_name);

/**
 * @brief Adds a new integer value to the Json object using the given name
 * 
 * @param p_json_object Json object where the new value is added to
 * @param name Name of the new value added
 * @param value integer value to be added
 */
void json_parser_add_integer(JSON_OPJECT_TYPE* p_json_object, const char* name, i32 value);

/**
 * @brief Adds a string to the json object with the given name
 * 
 * @param p_json_object Json object where the string is added to
 * @param name The name of the string to add
 * @param p_string The string to add
 */
void json_parser_add_string(JSON_OPJECT_TYPE* p_json_object, const char* name, const char*  p_string);

/**
 * @brief Stops a group that was started before. Be sure not to call this function twice
 * 
 * @param p_json_object The Json object where the group will be ended
 */
void json_parser_end_group(JSON_OPJECT_TYPE* p_json_object);

/**
 * @brief Finishes the json-object. A json-object has to be finished to be valid.
 * This function also closes all remaining groups that are still open.
 * After the json-object has finished no more data can be added to it
 * 
 * @param p_json_object json-object to finish
 */
void json_parser_finish(JSON_OPJECT_TYPE* p_json_object);

/**
 * @brief Gets the actual string length of the json object
 * 
 * @param p_json_object Json object to get the length from
 */
u16 json_parser_get_length(JSON_OPJECT_TYPE* p_json_object);

/**
 * @brief Copies the actual content of the json-object into the given string.
 * If the lenght of p_String is below the conetnt of the json object no
 * data is copied and 0 is returned.
 * 
 * @param p_json_object The finished json-object
 * @param p_string Content of the json-object will copied into here
 * @param max_length maximum length of the given string
 * @return number of characters that has been copied into p_string
 */
u16 json_parser_copy_to(JSON_OPJECT_TYPE* p_json_object, char* p_string, u16 max_length);

/**
 * @brief Returns the active status of the json-object
 * 
 * @param p_json_object 
 * @return 1 if json-object is active, otherwise 0
 */
u8 json_parser_is_active(JSON_OPJECT_TYPE* p_json_object);

/**
 * @brief Returns the complete status of the json-object
 * 
 * @param p_json_object 
 * @return 1 if json-object is complete, otherwise 0
 */
u8 json_parser_is_complete(JSON_OPJECT_TYPE* p_json_object);

// --------------------------------------------------------------------------------

#define JSON_PARSER_CREATE_OBJECT(name)                                                     \
                                                                                            \
    static JSON_OPJECT_TYPE __##name##_json_object;                                         \
                                                                                            \
    void name##_initialize(void) {                                                          \
        json_parser_initialize(&__##name##_json_object);                                    \
    }                                                                                       \
                                                                                            \
    void name##_append_pri_cmd_response(COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {         \
        json_parser_append_rpi_cmd_response(&__##name##_json_object, p_com_buffer);         \
    }                                                                                       \
                                                                                            \
    void name##_start_group(const char* group_name) {                                       \
        json_parser_start_group(&__##name##_json_object, group_name);                       \
    }                                                                                       \
                                                                                            \
    void name##_add_integer(const char* name, i32 value) {                                  \
        json_parser_add_integer(&__##name##_json_object, name, value);                      \
    }                                                                                       \
                                                                                            \
    void name##_add_rpi_response(COMMON_GENERIC_BUFFER_TYPE* p_com_buffer) {                \
        json_parser_append_rpi_cmd_response(&__##name##_json_object, p_com_buffer);         \
    }                                                                                       \
                                                                                            \
    void name##_add_cli_response(const char* p_command, const char* p_response) {           \
        json_parser_append_cli_cmd_response(&__##name##_json_object, p_command, p_response);\
    }                                                                                       \
                                                                                            \
    void name##_add_string(const char* name, const char*  p_string) {                       \
        json_parser_add_string(&__##name##_json_object, name, p_string);                    \
    }                                                                                       \
                                                                                            \
    void name##_end_group(void) {                                                           \
        json_parser_end_group(&__##name##_json_object);                                     \
    }                                                                                       \
                                                                                            \
    void name##_finish(void) {                                                              \
        json_parser_finish(&__##name##_json_object);                                        \
    }                                                                                       \
                                                                                            \
    u16 name##_copy_to(char* p_string, u16 max_length) {                                    \
        return json_parser_copy_to(&__##name##_json_object, p_string, max_length);          \
    }                                                                                       \
                                                                                            \
    u16 name##_get_length(void) {                                                           \
        return json_parser_get_length(&__##name##_json_object);                             \
    }                                                                                       \
                                                                                            \
    const char* name##_to_string(void) {                                                    \
        return (const char*) __##name##_json_object.string_buffer;                          \
    }                                                                                       \
                                                                                            \
    u8 name##_is_active(void) {                                                             \
        return json_parser_is_active(&__##name##_json_object);                              \
    }                                                                                       \
                                                                                            \
    u8 name##_is_complete(void) {                                                           \
        return json_parser_is_complete(&__##name##_json_object);                            \
    }

// --------------------------------------------------------------------------------

#endif // _PROTOCOL_JSON_PARSER_H_

// --------------------------------------------------------------------------------
