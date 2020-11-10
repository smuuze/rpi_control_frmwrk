#ifndef COMMON_TOOLS_STRING_H_
#define COMMON_TOOLS_STRING_H_

/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_tools_string.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

/*
 *
 */
void common_tools_hex_dump(const u8* p_buffer, u16 length);

/*!
 * returns number of bytes read
 */
//u16 read_line(FILE* file_handle, char* p_buffer_to, u16 num_max_bytes);

/*
 *
 */
//u8 write_line(FILE* file_handle, char* p_buffer_from, u16 num_max_bytes);

/*
 *
 */
u16 common_tools_string_get_char_count(char char_to_count, const char* p_string);

/*
 *
 */
void common_tools_string_split(char splitter, const char* p_string_in, char* p_string_out_1, u16 string_out_1_max_len, char* p_string_out_2, u16 string_out_2_max_len);

/*
 *
 */
u16 string_length(char* p_str);

/*
 *
 */
u16 string_append(char* p_string_to, char* p_string_from, u16 max_length);

/*
 *
 */
void common_tools_string_trim(char* p_string);

/*
 *
 */
u8 common_tools_hex_string_to_byte_array(const char* hex_string, u16 hex_string_len, u8* byte_array, u16 byte_array_max_len);

/*
 *
 */
u8 common_tools_byte_array_string_to_hex_string(u8* byte_array, u8 byte_array_len, char* hex_string, u16 hex_string_max_len);

/*
 *
 */
void string_get_time(char* time_string);

#endif // COMMON_TOOLS_STRING_H_