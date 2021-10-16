/**
 * @file common_tools_string.h
 * @author sebastian lesse (sebastian lesse)
 * @brief 
 * @version 1.0
 * @date 2021-09-24
 * 
 * @copyright Copyright (c) 2021
 * 
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

/**
 * @brief Prints out a HEX dump on the console.
 * There will be 16 bytes printed out per line.
 * 
 * @param p_buffer Buffer where the bytes are read from
 * @param length length of the buffer in number of bytes
 */
void common_tools_hex_dump(const u8* p_buffer, u16 length);

/**
 * @brief Counts the how many times the given character is existing
 * inside of the givens tring.
 * 
 * @param char_to_count character to find inside of the string
 * @param p_string the string where to search for the character
 * @return number of how often the character is existing inside of the givens tring
 */
u16 common_tools_string_get_char_count(char char_to_count, const char* p_string);

/**
 * @brief Splits the given string into two sub-strings. The givens tring is splitted
 * at the first position of the characer given by splitter. The splitter is not included in 
 * any of the sub-strings.
 * 
 * @param splitter character where the string is splitted
 * @param p_string_in string to split
 * @param p_string_out_1 destination for the sub-string of the left side of the splitter
 * @param string_out_1_max_len maximum length of p_string_out_1
 * @param p_string_out_2 destination for the sub-string of the right side of the splitter
 * @param string_out_2_max_len maximum length of p_string_out_2
 */
void common_tools_string_split(char splitter, const char* p_string_in, char* p_string_out_1, u16 string_out_1_max_len, char* p_string_out_2, u16 string_out_2_max_len);

/**
 * @brief Appends a string at the end of the given string
 * 
 * @param p_string_base string where the addtional tring is added at the end
 * @param p_string_to_append string that is added at the end of the base-string
 * @param max_length_string_base maximum lenght of the base string that is checked if the new string can be added
 * @return length of the new string, length of p_string_base + length of p_string_to_append,
 * or the length of p_string_base, if length of p_string_to_append exceeds max_length_string_base
 */
u16 common_tools_string_append(char* p_string_base, const char* p_string_to_append, u16 max_length_string_base);

/**
 * @brief Appends a number to at the end of the given string
 * 
 * @param p_string_base String where the number is appended
 * @param number the number to append to the string 
 * @param max_length_string_base maximum length of p_string_base
 * @return length of the new string, length of p_string_base + length of string of the number to append,
 * or the length of p_string_base, if length of string of the number to append exceeds max_length_string_base
 */
u16 common_tools_string_append_number(char* p_string_base, u32 number, u16 max_length_string_base);

/**
 * @brief Appends a singel character to the given string.
 * This function checks the maximum length of the string if the cahracter can be added.
 * 
 * @param p_string_base string where the character is added to.
 * @param character the character to add at the end of the string.
 * @param max_length_string_base maximum of the given string.
 * @return the length of the string including the new character.
 * Or the length of the string without the character if it cannot be added.
 */
u16 common_tools_string_append_character(char* p_string_base, char character, u16 max_length_string_base);

/**
 * @brief 
 * 
 * @param p_string_base 
 * @param number 
 * @param bit_length 
 * @param max_length_string_base 
 * @return u16 
 */
u16 common_tools_string_number_to_hex_string(char* p_string_base, u32 number, u8 bit_length, u16 max_length_string_base);

/**
 * @brief 
 * 
 * @param p_string_base 
 * @param number 
 * @param bit_length 
 * @param max_length_string_base 
 * @return u16 
 */
u16 common_tools_string_append_hex_number(char* p_string_base, u32 number, u8 bit_length, u16 max_length_string_base);

/**
 * @brief Get the length of p_string
 * 
 * @param p_string valid string with terminating \0
 * @return length of p_string
 */
u16 common_tools_string_length(const char* p_string);

/**
 * @brief Checks if a string is inside of another string
 * 
 * @param p_base_str null-terminated string where to look at
 * @param p_search_str null-terminated string to look for
 * @return 1 : p_search_string was found inside of p_base_string
 * 0 : p_search_string is not inside of p_base_string
 */
u8 common_tools_string_contains(const char* p_base_str, const char* p_search_str);

/**
 * @brief Copies a sub-string. Befor the string is copied, p_string_to is completly cleared (all values are set to '\0').
 * 
 * @param p_string_to destination where the substring is stored into
 * @param p_string_from Source from wher to read the substring
 * @param start_idx start-index of the substring inside of p_string_from
 * @param length number of characters to copy
 * @param max_length maximum length of p_string_to
 * @return u16 number of characters that has been copied to p_string_to
 */
u16 common_tools_string_substring(char* p_string_to, const char* p_string_from, u16 start_idx, u16 length, u16 max_length);

/**
 * @brief 
 * 
 * @param p_str 
 * @return u16 
 */
u16 string_length(char* p_str);

/**
 * @brief 
 * 
 * @param p_string_to 
 * @param p_string_from 
 * @param max_length 
 * @return u16 
 */
u16 string_append(char* p_string_to, char* p_string_from, u16 max_length);

/**
 * @brief 
 * 
 * @param p_string 
 */
void common_tools_string_trim(char* p_string);

/**
 * @brief 
 * 
 * @param hex_string 
 * @param hex_string_len 
 * @param byte_array 
 * @param byte_array_max_len 
 * @return u16 
 */
u16 common_tools_hex_string_to_byte_array(const char* hex_string, u16 hex_string_len, u8* byte_array, u16 byte_array_max_len);

/**
 * @brief 
 * 
 * @param byte_array 
 * @param byte_array_len 
 * @param hex_string 
 * @param hex_string_max_len 
 * @return u16 
 */
u16 common_tools_byte_array_string_to_hex_string(u8* byte_array, u8 byte_array_len, char* hex_string, u16 hex_string_max_len);

/**
 * @brief 
 * 
 * @param p_str 
 * @return u16 
 */
u16 common_tools_string_to_u16(const char* p_str);

/**
 * @brief 
 * 
 * @param p_str 
 * @return u32 
 */
u32 common_tools_string_to_u32(const char* p_str);

/**
 * @brief 
 * 
 * @param p_string 
 * @param character 
 * @return u8 
 */
u8 common_tools_string_ends_with(const char* p_string, char character);

/**
 * @brief 
 * 
 * @param p_string 
 * @param character 
 * @return u8 
 */
u8 common_tools_string_starts_with(const char* p_string, char character);

/**
 * @brief 
 * 
 * @param p_string 
 */
void common_tools_string_remove_last_character(char* p_string);

/**
 * @brief 
 * 
 * @param p_string1 
 * @param p_string2 
 * @return u8 
 */
u8 common_tools_string_compare(const char* p_string1, const char* p_string2);

/**
 * @brief 
 * 
 * @param p_string_to 
 * @param p_string_from 
 * @param max_length 
 * @return u16 
 */
u16 common_tools_string_copy_string(char* p_string_to, const char* p_string_from, u16 max_length);

/**
 * @brief 
 * 
 * @param time_string 
 */
void string_get_time(char* time_string);

/**
 * @brief 
 * 
 * @param string_to 
 * @param max_string_length 
 * @param number 
 */
void common_tools_string_from_u8(char* string_to, u16 max_string_length, u8 number);

/**
 * @brief converts a signed 32 bit integer value into a string
 * 
 * @param string_to string where to store the number
 * @param number signed 32 bit integer value
 */
void common_tools_string_from_i32(char* string_to, u16 max_string_length, i32 number);

#endif // COMMON_TOOLS_STRING_H_
