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
 * @file    common_tools_string.c
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 21
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// --------------------------------------------------------------------------------

#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

void common_tools_hex_dump(const u8* p_buffer, u16 length) {

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

// --------------------------------------------------------------------------------

u16 common_tools_string_get_char_count(char char_to_count, const char* p_string) {

	u16 counter = 0;

	for (u16 i = 0; i < strlen(p_string); i++) {
		if (p_string[i] == char_to_count) {
			counter += 1;
		}
	}

	return counter;
}

void common_tools_string_clear(char* p_string, u16 length) {
	memset(p_string, '\0', length);
}

void common_tools_string_trim(char* p_string) {

	u16 length = strlen(p_string);

	if (length == 0) {
		DEBUG_PASS("common_tools_string_trim() - Length of string is zero");
		return;
	}

	u16 index_of_start = 0;
	u16 index_of_end = 0;

	DEBUG_TRACE_STR(p_string, "common_tools_string_trim() - String-Length");
	DEBUG_TRACE_word(length, "common_tools_string_trim() - String-Length:");
	
	u16 i = 0;
	for ( ; i < length; i++) {
		
		if (isspace(p_string[i])) { 
			continue;
		}
			
		index_of_start = i;
		break;
	}

	DEBUG_TRACE_word(index_of_start, "common_tools_string_trim() - White-Spaces at start of string:");
	
	i = length - 1;
	index_of_end = length;
	for ( ; i != 0 ; ) {

		if (isspace(p_string[i]) && p_string[i] != '\0') {

			i--;
			index_of_end--;
			continue;
		}

		break;
	}

	DEBUG_TRACE_word((length - index_of_end), "common_tools_string_trim() - White-Spaces at end of string:");
	
	u16 new_length = index_of_end - index_of_start;
	memmove(p_string, p_string + index_of_start, new_length);
	memset(p_string + new_length, '\0', length - new_length);
}

// --------------------------------------------------------------------------------

u16 common_tools_string_split(char splitter, const char* p_string_in, char* p_string_out_1, u16 string_out_1_max_len, char* p_string_out_2, u16 string_out_2_max_len) {

	u16 length = strlen(p_string_in);
	u16 num_bytes_string1 = 0;
	u16 num_bytes_string2 = 0;
	
	u8 splitter_detected = 0;
	
	u16 i = 0;
	
	//DEBUG_PASS("split_string() - String1: ");
	
	for ( ; i < length; i++) {	
	
		if (p_string_in[i] == '\0') {
			break;
		}
	
		if (splitter_detected == 0 && p_string_in[i] == splitter) {
			//p_string_out_1[num_bytes_string1] = '\0';
			splitter_detected = 1;
			//DEBUG_PASS("split_string() - String2: ");
			continue;
		}
	
		if (splitter_detected == 0) {
		
			// processing string 1
			if (num_bytes_string1 < string_out_1_max_len) {
				if (p_string_out_1 != NULL) {
					p_string_out_1[num_bytes_string1++] = p_string_in[i];
				}
				//DEBUG_PASS("%c", p_string_in[i]);
			}
			
		} else {	
		
			// processing string 2
			if (num_bytes_string2 < string_out_2_max_len) {
				if (p_string_out_2 != NULL) {
					p_string_out_2[num_bytes_string2++] = p_string_in[i];
				}
			}
		}
	}
	
	if (num_bytes_string1 < string_out_1_max_len) {
		if (p_string_out_1 != NULL) {
			memset(p_string_out_1 + num_bytes_string1, '\0', string_out_1_max_len - num_bytes_string1);
		}
	}	
		
	if (num_bytes_string2 < string_out_2_max_len) {
		if (p_string_out_2 != NULL) {
			memset(p_string_out_2 + num_bytes_string2, '\0', string_out_2_max_len - num_bytes_string2);
		}
	}

    return num_bytes_string1;
}

// --------------------------------------------------------------------------------

u16 common_tools_string_append(char* p_string_base, const char* p_string_to_append, u16 max_length_string_base) {

	u16 length_of_string_base = strlen(p_string_base);
	u16 length_of_string_append = strlen(p_string_to_append);
	u16 length_of_new_string = length_of_string_base + length_of_string_append;

	if (length_of_new_string > max_length_string_base) {
		DEBUG_PASS("common_tools_string_append() - Cant append string - OVERFLOW !!! --- ");
		return length_of_string_base;
	}

	DEBUG_TRACE_STR(p_string_to_append, "common_tools_string_append() - String to append");
	DEBUG_TRACE_word(length_of_string_append, "common_tools_string_append() - Length of string to append");

	memcpy((u8*)p_string_base + length_of_string_base, p_string_to_append, length_of_string_append);
	memset(p_string_base + length_of_new_string, '\0', (max_length_string_base - length_of_new_string));

	DEBUG_TRACE_STR(p_string_base, "common_tools_string_append() - New String: ");

	return length_of_new_string;
}

// --------------------------------------------------------------------------------

u16 common_tools_string_append_number(char* p_string_base, u32 number, u16 max_length_string_base) {

	char number_string[32];
	common_tools_string_clear(number_string, sizeof(number_string));
	common_tools_string_from_i32(number_string, sizeof(number_string), (i32) number);
	common_tools_string_append(p_string_base, number_string, max_length_string_base);
	
	return common_tools_string_length(p_string_base);
}

// --------------------------------------------------------------------------------

u16 common_tools_string_append_character(char* p_string_base, char character, u16 max_length_string_base) {

	char character_string[] = {character, '\0'};
	common_tools_string_append(p_string_base, character_string, max_length_string_base);
	
	return common_tools_string_length(p_string_base);
}

// --------------------------------------------------------------------------------

u16 common_tools_string_append_hex_number(char* p_string_base, u32 number, u8 bit_length, u16 max_length_string_base) {

	char hex_number_string[10];
	common_tools_string_clear(hex_number_string, sizeof(hex_number_string));
	common_tools_string_number_to_hex_string(hex_number_string, number, bit_length, max_length_string_base);
	common_tools_string_append(p_string_base, hex_number_string, max_length_string_base);
	
	return common_tools_string_length(p_string_base);
}

// --------------------------------------------------------------------------------

u16 common_tools_string_number_to_hex_string(char* p_string_base, u32 number, u8 bit_length, u16 max_length_string_base) {

	if (bit_length > 32) {
		// nothing to do here ...

	} else if (bit_length > 16) {
		snprintf(p_string_base, max_length_string_base, "%08X", number);

	} else if (bit_length > 8) {
		snprintf(p_string_base, max_length_string_base, "%04X", number);
	} else {
		snprintf(p_string_base, max_length_string_base, "%02X", number);
	}

	return common_tools_string_length(p_string_base);
}

// --------------------------------------------------------------------------------

u8 common_tools_string_ends_with(const char* p_string, char character) {

	u16 length = strlen(p_string);

	if (length == 0) {
		return 0;
	}

	DEBUG_TRACE_CHAR(p_string[length - 1], "common_tools_string_ends_with()");
	DEBUG_TRACE_word(length, "common_tools_string_ends_with()");

	if (p_string[length - 1] == character) {
		return 1;
	} else {
		return 0;
	}
}

// --------------------------------------------------------------------------------

u8 common_tools_string_starts_with(const char* p_string, char character) {

	u16 length = strlen(p_string);

	if (length == 0) {
		return 0;
	}

	DEBUG_TRACE_CHAR(p_string[length - 1], "common_tools_string_starts_with()");
	DEBUG_TRACE_word(length, "common_tools_string_starts_with()");

	if (p_string[0] == character) {
		return 1;
	} else {
		return 0;
	}
}

// --------------------------------------------------------------------------------

u16 common_tools_string_length(const char* p_string) {
	return strlen(p_string);
}

// --------------------------------------------------------------------------------

void common_tools_string_remove_last_character(char* p_string) {

	u16 length = strlen(p_string);

	if (length == 0) {
		return ;
	}

	p_string[length - 1] = '\0';
}

// --------------------------------------------------------------------------------

u8 common_tools_string_compare(const char* p_string1, const char* p_string2) {

	if (strlen(p_string1) != strlen(p_string2)) {
		return 0;
	}
	
	if (memcmp(p_string1, p_string2, strlen(p_string1)) != 0) {
		return 0;
	}

	return 1;
}

// --------------------------------------------------------------------------------

u16 common_tools_string_copy_string(char* p_string_to, const char* p_string_from, u16 max_length) {

	memset(p_string_to, '\0', max_length);

	if (p_string_from == NULL) {
		return 0;
	}

	u16 length = strlen(p_string_from);

	if (length > max_length - 1) {
		length = max_length - 1;
	}

	memcpy(p_string_to, p_string_from, length);

	return length;
}

// --------------------------------------------------------------------------------

u8 common_tools_string_contains(const char* p_base_str, const char* p_search_str) {

	u16 base_length = strlen(p_base_str);
	u16 search_length = strlen(p_search_str);

	if (base_length == 0 || search_length == 0) {
		DEBUG_PASS("common_tools_string_contains() - String-Length must not be 0");
		DEBUG_TRACE_word(base_length, "- base_length:");
		DEBUG_TRACE_word(search_length, "- search_length:");
		return 0;
	}

	if (base_length < search_length) {
		DEBUG_PASS("common_tools_string_contains() - String does not fit");
		DEBUG_TRACE_word(base_length, "- base_length:");
		DEBUG_TRACE_word(search_length, "- search_length:");
		return 0;
	}

	u16 i = 0;
	u16 limit = base_length - search_length + 1;

	for ( ; i < limit ; i++ ) {

		DEBUG_TRACE_STR(p_base_str + i, "STRING");

		if (memcmp(p_base_str + i, p_search_str, search_length) == 0) {
			return 1;
		}
	}

	return 0;
}

// --------------------------------------------------------------------------------

u16 common_tools_string_substring(char* p_string_to, const char* p_string_from, u16 start_idx, u16 length, u16 max_length) {

	memset(p_string_to, '\0', max_length);

	if (p_string_from == NULL) {
		return 0;
	}

	if (length > max_length - 1) {
		length = max_length - 1;
	}

	memcpy(p_string_to, p_string_from + start_idx, length);
	return length;
}

// --------------------------------------------------------------------------------

void common_tools_string_from_u8(char* string_to, u16 max_string_length, u8 number) {
	snprintf(string_to, max_string_length, "%d", number);
}

// --------------------------------------------------------------------------------

void common_tools_string_from_i32(char* string_to, u16 max_string_length, i32 number) {
	snprintf(string_to, max_string_length, "%d", number);
}

// --------------------------------------------------------------------------------

u16 common_tools_hex_string_to_byte_array(const char* hex_string, u16 hex_string_len, u8* byte_array, u16 byte_array_max_len) {
	
	if (hex_string_len < 2) {
		DEBUG_TRACE_byte(hex_string_len, "hex_string_to_byte_array() - hex_string_len < 2");
		return 0;
	}
		
	DEBUG_TRACE_byte(hex_string_len, "hex_string_to_byte_array() - hex_string_len: ");
	DEBUG_TRACE_byte(byte_array_max_len, "hex_string_to_byte_array() - byte_array_max_len: ");
	
	u16 i = 0;
	u16 j = 0;
	u8 is_upper_nibble = 1;
	
	memset(byte_array, 0x00, byte_array_max_len);
	
	while (i < hex_string_len && j < byte_array_max_len) {
		
		char nibble = hex_string[i++];
		
		if (nibble == '\0') {
			break;
		}
		
		u8 nibble_value = 0;
		u8 nibble_factor = is_upper_nibble != 0 ? 16 : 1;
		
		if (nibble >= 48 && nibble <= 57) {
			// is between 0 and 9
			nibble_value = nibble - '0';
			
		} else if (nibble > 64 && nibble < 91) {		
			// is between A and Z
			nibble_value = nibble - 'A' + 10;
			
		} else if (nibble > 97 && nibble < 122) {
			// is between a and z
			nibble_value = nibble - 'a' + 10;
			
		} else {
			continue;
		}
		
		byte_array[j] += (nibble_value * nibble_factor);
		
		if (is_upper_nibble == 0) {
			is_upper_nibble = 1;
			j++;
		} else {
			is_upper_nibble = 0;
		}
	}
	
	return j;
}

// --------------------------------------------------------------------------------

u16 common_tools_byte_array_string_to_hex_string(u8* byte_array, u8 byte_array_len, char* hex_string, u16 hex_string_max_len) {
	
	if (hex_string_max_len < 2 || byte_array_len == 0) {
		return 0;
	}
	
	u16 i = 0;
	u16 j = 0;
	
	memset(hex_string, '\0', hex_string_max_len);
	
	while (i < byte_array_len && j < hex_string_max_len - 2) {
		
		u8 nibble = byte_array[i] >> 4;
		char character = '0';
		
		if (nibble > 9) {
			// is between A and F
			character = nibble + 'A' - 10;
			
		} else {
			// is between 0 and 9
			character = nibble + '0';
		}
		
		hex_string[j++] = character;
		
		nibble = (byte_array[i] & 0x0F);
		
		if (nibble > 9) {
			// is between A and F
			character = nibble + 'A' - 10;
			
		} else {
			// is between 0 and 9
			character = nibble + '0';
		}
		
		hex_string[j++] = character;
		
		i++;
	}
	
	return j;
}

// --------------------------------------------------------------------------------

u8 common_tools_sting_is_number(const char* p_str) {
    
    u16 length = common_tools_string_length(p_str);
    u16 i = 0;

    for ( ; i < length; i++) {
        if (p_str[i] > '9' || p_str[i] < '0') {
            return 0;
        }
    }

    return 1;
}

// --------------------------------------------------------------------------------

u16 common_tools_string_to_u16(const char* p_str) {
	return (u16)atoi(p_str);
}

// --------------------------------------------------------------------------------

u32 common_tools_string_to_u32(const char* p_str) {
	return (u32)atoi(p_str);
}
// --------------------------------------------------------------------------------

u8 common_tools_to_u8(const char* p_str) {
	return (u8)atoi(p_str);
}

// --------------------------------------------------------------------------------

/*
void string_get_time(char* time_string) {

	FILE* pipe = popen("date +\"%H:%M\"", "r");
	read_line(pipe, time_string, 128);
	fclose(pipe);
}
*/

// --------------------------------------------------------------------------------

u8 common_tools_string_is_in_front(const char* p_string_base, const char* p_string_find) {

	if (strlen(p_string_base) < strlen(p_string_find)) {
		return 0;
	}
	
	if (memcmp(p_string_base, p_string_find, strlen(p_string_find)) != 0) {
		return 0;
	}

	return 1;

}

// --------------------------------------------------------------------------------
