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

/*
 *
 */
void console_write_line(const char* p_string);

/*
 *
 */
void console_write_string(const char* p_string, const char* p_argment_string);

/*
 *
 */
void console_new_line(void);

/*
 *
 */
void console_hex_dump(u16 length, const u8* p_buffer);

#endif // COMMON_TOOLS_CONSOLE_H_