/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_tools_console.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

void console_write_line(const char* p_string) {
	printf("%s\n", p_string);
}

void console_write(const char* p_string) {
	printf("%s", p_string);
}

void console_write_string(const char* p_string, const char* p_argment_string) {
	printf("%s %s\n", p_string, p_argment_string);
}

void console_write_number(u32 number) {
	printf("%d", number);
}

void console_new_line() {
	printf("\n");
}

void console_hex_dump(u16 length, const u8* p_buffer) {
	common_tools_hex_dump(p_buffer, length);
}