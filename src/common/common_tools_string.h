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
 * @file   common_tools_string.h
 * @author Sebastian Lesse
 * @date   2021 / 09 / 24
 * @brief  Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

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
 * @return length of p_string_out_1 in number of characters (without 0-terminator)
 */
u16 common_tools_string_split(char splitter, const char* p_string_in, char* p_string_out_1, u16 string_out_1_max_len, char* p_string_out_2, u16 string_out_2_max_len);

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
 * @brief Checks if p_search_str is somewhere inside of p_base_str.
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
 * @brief Converts the number of p_str
 * into an unsigned 8 bit numeric value
 * 
 * @param p_str string representation of the number
 * @return unsigned 8-bit number
 * 
 */
u8 common_tools_to_u8(const char* p_str);

/**
 * @brief Checks if the given string is a number.
 * This means every character musst be in the range '0' to '9'
 * 
 * @param p_str the string to check if it is a number
 * @return 1: p_str represents a number, otherwise 0
 */
u8 common_tools_sting_is_number(const char* p_str);

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
 * @brief Checks if p_string1 is equal to p_string2.
 * 
 * @param p_string1 first string for compare
 * @param p_string2 second string for compare
 * @return 1 if p_string1 and p_string2 are equal, 0 otherwise
 */
u8 common_tools_string_compare(const char* p_string1, const char* p_string2);

/**
 * @brief Copies a string into another string.
 * The source-string is copied at the beginning of the destination string.
 * Existing data is overwritten. The destination-string is set to all '\0'
 * before the source is copied. The number of characters to copy is determined by the length of
 * the source-string (null-termination). If the source-string is longer than the destination,
 * only the number of characters that fits into the destination are copied (max_length-1). The resulting string
 * always has a null-termination sign at the end.
 * 
 * @param p_string_to destination where the source is copied into, is deleted befor copying
 * @param p_string_from source-string that is copied to the destination, must end with null-termination
 * @param max_length maximum length of the destination string in number of characters.
 * @return number of characters that have been copied
 */
u16 common_tools_string_copy_string(char* p_string_to, const char* p_string_from, u16 max_length);

/**
 * @brief Checks if p_string_base starts with p_string_find.
 * This function is case sensitive.
 * 
 * @param p_string_base 0-terminated string where to search
 * @param p_string_find  0-terminated string to look for
 * @return 1 if p_string_base starts with p_string_find
 */
u8 common_tools_string_is_in_front(const char* p_string_base, const char* p_string_find);

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
