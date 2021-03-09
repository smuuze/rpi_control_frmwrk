/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_tools_string.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef COMMON_TOOLS_STRING_H_
#define COMMON_TOOLS_STRING_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

/**
 * @brief Clears the actual content of p_string by setting all characters to '\0'
 * 
 * @param p_string string to clear
 * @param length length of string to clear
 */
void common_tools_string_clear(char* p_string, u16 length);

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

/**
 * @brief 	Appends the p_string_to_append to the end of p_string_base.
 * 
 * @param p_string_base 
 * @param p_string_to_append 
 * @param max_length_string_base 
 * @return 	length of the new string, length of p_string_base + length of p_string_to_append,
 * 		or the length of p_string_base, if length of p_string_to_append exceeds max_length_string_base
 */
u16 common_tools_string_append(char* p_string_base, const char* p_string_to_append, u16 max_length_string_base);

/**
 * @brief Get the length of p_string
 * 
 * @param p_string valid string with terminating \0
 * @return length of p_string
 */
u16 common_tools_string_length(const char* p_string);

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
u16 common_tools_hex_string_to_byte_array(const char* hex_string, u16 hex_string_len, u8* byte_array, u16 byte_array_max_len);

/*
 *
 */
u16 common_tools_byte_array_string_to_hex_string(u8* byte_array, u8 byte_array_len, char* hex_string, u16 hex_string_max_len);

/*
 *
 */
u16 common_tools_string_to_u16(const char* p_str);

/*
 *
 */
u32 common_tools_string_to_u32(const char* p_str);

/*
 *
 */
u8 common_tools_string_ends_with(const char* p_string, char character);

/*
 *
 */
void common_tools_string_remove_last_character(char* p_string);

/*
 *
 */
u8 common_tools_string_compare(const char* p_string1, const char* p_string2);

/*
 *
 */
u16 common_tools_string_copy_string(char* p_string_to, const char* p_string_from, u16 max_length);

/*
 *
 */
void string_get_time(char* time_string);

/*
 *
 */
void common_tools_string_from_u8(char* string_to, u8 number);

/**
 * @brief converts a signed 32 bit integer value into a string
 * 
 * @param string_to string where to store the number
 * @param number signed 32 bit integer value
 */
void common_tools_string_from_i32(char* string_to, i32 number);

#endif // COMMON_TOOLS_STRING_H_
