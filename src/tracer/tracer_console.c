/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

// ------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------------------------------------------------------------------------------------

void hex_dump(const void *src, int length, int line_size, char *prefix) {
	
	int i = 0;
	const unsigned char *address = src;
	//const unsigned char *line = address;

	printf("%s  ", prefix);
	
	while (length-- > 0) {
	
		printf("%02X ", *address++);	

		if (!(++i % line_size) || (length == 0 && (i % line_size))) {

			if (length == 0) {			
				while (i++ % line_size) {
					printf("__ ");
				}
			}
			
			// printf(" | ");  // right close
			// while (line < address) {
				
				// unsigned char c = *line++;
				// printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			// }
			
			printf("\n");			
			if (length > 0) {
				printf("%s  ", prefix);
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------------

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

	printf("%s\n", str);
	
	hex_dump((const void*) p_buffer, length, 16, " ");
}

