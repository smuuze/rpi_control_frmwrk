/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

// ------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------------------------------------------------------

void tracer_hex_dump(const u8* p_buffer, u16 length) {

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

/*!
 *
 */
static u8 is_initialized = 0;

static u8 is_enabled = 0;

// ------------------------------------------------------------------------------------------------------------

void tracer_init(void) {
	
	is_enabled = 1;
	is_initialized = 1;

	printf("--- TRACER INITIALIZED AND ENABLED ---\n");
}

void tracer_pass(const char* str, const char* file_name, u16 line_id) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s\n", file_name, line_id, str);
}

void tracer_trace_byte(const char* str, const char* file_name, u16 line_id, u8 byte) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s \n - Data: %d (0x%02X)\n", file_name, line_id, str, byte, byte);
}

void tracer_trace_word(const char* str, const char* file_name, u16 line_id, u16 word) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s \n - Data: %d (0x%04X)\n", file_name, line_id, str, word, word);
}

void tracer_trace_long(const char* str, const char* file_name, u16 line_id, u32 integer) {

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s \n - Data: %d (0x%08X)\n", file_name, line_id, str, integer, integer);
}

void tracer_trace_n(const char* str, const char* file_name, u16 line_id, u8 length, const u8* p_buffer) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s\n", file_name, line_id, str);
	
	tracer_hex_dump(p_buffer, length);
}

void tracer_trace_string(const char* str, const char* file_name, u16 line_id, const char* p_string) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (is_enabled == 0) {
		return;
	}

	printf("%s:%d - %s \n - String: \"%s\"\n", file_name, line_id, str, p_string);
}

void tracer_enable(u8 enable) {

	if (is_initialized == 0) {
		tracer_init();
	}

	if (enable) {
		printf("\n--- TRACER ENABLED ---\n");
		is_enabled = 1;
	} else {
		printf("\n--- TRACER DISABLED ---\n");
		is_enabled = 0;
	}

}
