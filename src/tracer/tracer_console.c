/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

// ------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------------------------------------------------------

void tracer_hex_dump(u8* p_buffer, u16 length) {

	u16 i = 0;
	u8 buffer_index = 0;
	u8 character_index = 0;
	u16 address = 0;
	u8 line_size = 16;
	u8 end_of_line = 0;

	printf("\t%04X  |  ", address);
	
	do {
	
		if (length != 0) {
			printf("%02x ", p_buffer[buffer_index++]);
			length--;
		} else {
			printf("__ ");
		}

		if ( (i != 0) && ((i + 1) % 8) == 0) {
			printf(" ");
		}

		if ( !(++i % line_size) ) {
			
			printf(" |  ");	

			while (character_index < buffer_index) {
				
				unsigned char c = (unsigned char) p_buffer[character_index++];
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			
			printf("\n");	

			if (length != 0) {
				address += line_size;
				printf("\t%04X  |  ", address);
			} else {
				end_of_line = 1;
			}
		}

	} while (end_of_line == 0);
}

// ------------------------------------------------------------------------------------------------------------

void tracer_init(void) {

}

void tracer_pass(const char* str, const char* file_name, u16 line_id) {
	(void) file_name;
	(void) line_id;

	printf("%s:%d - %s\n", file_name, line_id, str);
}

void tracer_trace_byte(const char* str, const char* file_name, u16 line_id, u8 byte) {
	(void) file_name;
	(void) line_id;

	printf("%s:%d - %s - %02X\n", file_name, line_id, str, byte);
}

void tracer_trace_word(const char* str, const char* file_name, u16 line_id, u16 word) {
	(void) file_name;
	(void) line_id;

	printf("%s:%d - %s - %02X\n", file_name, line_id, str, word);
}

void tracer_trace_long(const char* str, const char* file_name, u16 line_id, u32 integer) {
	(void) file_name;
	(void) line_id;

	printf("%s:%d - %s - %02X\n", file_name, line_id, str, integer);
}

void tracer_trace_n(const char* str, const char* file_name, u16 line_id, u8 length, u8* p_buffer) {
	(void) file_name;
	(void) line_id;

	printf("%s:%d - %s\n", file_name, line_id, str);
	
	tracer_hex_dump(p_buffer, length);
}

