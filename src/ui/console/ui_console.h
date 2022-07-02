#ifndef COMMON_TOOLS_CONSOLE_H_
#define COMMON_TOOLS_CONSOLE_H_

/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_tools_console.h
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

/**
 * @brief Writes a string on the console.
 * The string is printed out without any modification.
 * No new-line character is added at the end of the string.
 * 
 * @param p_string 
 */
void console_write(const char* p_string);

/**
 * @brief Writes a line on the system console.
 * Automatically adds a new-line ad the end of the string
 * 
 * @param p_string string to print on the console.
 */
void console_write_line(const char* p_string);

/**
 * @brief Writes a message and an additional stirng on the console.
 * the argument-string is added with a space ad the end of the string-message.
 * A new-line caracter will also be added at the end of the output.
 * 
 * @param p_string message to print on the console.
 * @param p_argment_string additional string that is added at the end of the message
 */
void console_write_string(const char* p_string, const char* p_argment_string);

/**
 * @brief Writes a single number only on the console.
 * The output is not modified. There is no new-line
 * character added at the end of the output.
 * 
 * @param number number to write on the console
 */
void console_write_number(i32 number);

/**
 * @brief Writes a new-line character only on the console.
 * 
 */
void console_new_line(void);

/**
 * @brief Prints the given buffer as a hex-dump on the console.
 * The hex dump is printed out with 16 bytes per line. It also contains
 * an address output and the character representation.
 * 
 * @param length number of bytes ot the given buffer
 * @param p_buffer buffer containing the bytes to print on the console.
 */
void console_hex_dump(u16 length, const u8* p_buffer);

#endif // COMMON_TOOLS_CONSOLE_H_
