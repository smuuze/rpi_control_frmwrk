/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void tracer_init(void) {

}

void tracer_pass(const char* str, const char* file_name, u16 line_id) {
	(void) file_name;
	(void) line_id;

	printf("%s\n", str);
}

void tracer_trace_byte(const char* str, const char* file_name, u16 line_id, u8 byte) {
	(void) file_name;
	(void) line_id;

	printf("%s - %02X\n", str, byte);
}

void tracer_trace_word(const char* str, const char* file_name, u16 line_id, u16 word) {
	(void) file_name;
	(void) line_id;

	printf("%s - %02X\n", str, word);
}

void tracer_trace_long(const char* str, const char* file_name, u16 line_id, u32 integer) {
	(void) file_name;
	(void) line_id;

	printf("%s - %02X\n", str, integer);
}

void tracer_trace_n(const char* str, const char* file_name, u16 line_id, u8 length, u8* p_buffer) {
	(void) file_name;
	(void) line_id;

	printf("%s - %d\n", str, length);
}

