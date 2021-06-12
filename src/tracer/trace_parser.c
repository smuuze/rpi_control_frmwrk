/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	tracer/trace_parser.c
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

#include "common/common_tools_number.h"
#include "common/common_tools_string.h"
#include "tracer/trace_object.h"

// --------------------------------------------------------------------------------

static u8 tracer_get_trace_type(TRACE_OBJECT_RAW* p_raw_object, TRACE_OBJECT* p_trace_obj){

	u8 raw_type = p_raw_object->data[TRACE_PARSER_INDEX_TRACE_TYPE];

	switch (raw_type) {
		default: 
			DEBUG_TRACE_byte(raw_type, "tracer_get_trace_type() - UNKNOWN: ");
			return 0;
		case TRACER_TRACE_TYPE_RAW_PASS  : 

			p_trace_obj->type = TRACE_OBJECT_TYPE_PASS;
			p_trace_obj->data_length = 0;

			DEBUG_PASS("tracer_get_trace_type() - TRACE_OBJECT_TYPE_PASS");
			break;

		case TRACER_TRACE_TYPE_RAW_BYTE  : 

			p_trace_obj->type = TRACE_OBJECT_TYPE_BYTE;  
			p_trace_obj->data_length = 1;

			DEBUG_PASS("tracer_get_trace_type() - TRACE_OBJECT_TYPE_BYTE");
			break;

		case TRACER_TRACE_TYPE_RAW_WORD  : 

			p_trace_obj->type = TRACE_OBJECT_TYPE_WORD;  
			p_trace_obj->data_length = 2;

			DEBUG_PASS("tracer_get_trace_type() - TRACE_OBJECT_TYPE_WORD");
			break;

		case TRACER_TRACE_TYPE_RAW_LONG  : 

			p_trace_obj->type = TRACE_OBJECT_TYPE_LONG;  
			p_trace_obj->data_length = 4;

			DEBUG_PASS("tracer_get_trace_type() - TRACE_OBJECT_TYPE_LONG");
			break;

		case TRACER_TRACE_TYPE_RAW_ARRAY : 

			p_trace_obj->type = TRACE_OBJECT_TYPE_ARRAY; 
			p_trace_obj->data_length = p_raw_object->data[TRACE_PARSER_INDEX_TRACE_TYPE + 1];

			DEBUG_TRACE_word(p_trace_obj->data_length, "tracer_get_trace_type() - TRACE_OBJECT_TYPE_ARRAY - Length:");
			break;
	}

	return 1;
}

static void tracer_get_line_number(TRACE_OBJECT_RAW* p_raw_object, TRACE_OBJECT* p_trace_obj) {

	u16 index = 0;

	switch (p_trace_obj->type) {
		default:
			break;

		case TRACE_OBJECT_TYPE_PASS  :
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE;
			break;

		case TRACE_OBJECT_TYPE_BYTE  : 
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 1;
			break;

		case TRACE_OBJECT_TYPE_WORD  : 
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 2;
			break;

		case TRACE_OBJECT_TYPE_LONG  : 
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 4;
			break;

		case TRACE_OBJECT_TYPE_ARRAY : 
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 1 + p_trace_obj->data_length;
			break;
	}

	DEBUG_TRACE_word(index, "tracer_get_line_number() - Index:");
	p_trace_obj->line_number = common_tools_number_readU16_MSB(p_raw_object->data + index);
	DEBUG_TRACE_word(p_trace_obj->line_number, "tracer_get_line_number() - Line-Number: ");
}

static void tracer_get_file_name(TRACE_OBJECT_RAW* p_raw_object, TRACE_OBJECT* p_trace_obj) {

	u16 index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_LINE_NUMBER;
	u16 length = p_raw_object->length - TRACE_PARSER_NUM_BYTES_HEADER - TRACE_PARSER_NUM_BYTES_BYTE_COUNT - TRACE_PARSER_NUM_BYTES_TRACE_TYPE - TRACE_PARSER_NUM_BYTES_LINE_NUMBER - TRACER_PARSER_NUM_BYTES_FOOTER;
	u16 offset = 0;

	switch (p_trace_obj->type) {
		default:
			DEBUG_PASS("tracer_get_file_name() - UNKNOWN");
			break;

		case TRACE_OBJECT_TYPE_PASS  :

			DEBUG_PASS("tracer_get_file_name() - TRACE_OBJECT_TYPE_PASS");
			break;

		case TRACE_OBJECT_TYPE_BYTE  : 

			DEBUG_PASS("tracer_get_file_name() - TRACE_OBJECT_TYPE_BYTE");
			index += 1;
			length -= 1;

			break;

		case TRACE_OBJECT_TYPE_WORD  : 

			DEBUG_PASS("tracer_get_file_name() - TRACE_OBJECT_TYPE_WORD");
			index += 2;
			length -= 2;

			break;

		case TRACE_OBJECT_TYPE_LONG  : 

			DEBUG_PASS("tracer_get_file_name() - TRACE_OBJECT_TYPE_LONG");
			index += 4;
			length -= 4;

			break;

		case TRACE_OBJECT_TYPE_ARRAY : 

			DEBUG_PASS("tracer_get_file_name() - TRACE_OBJECT_TYPE_ARRAY");
			index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 2 + 1 + p_trace_obj->data_length;
			length -= (1 + p_trace_obj->data_length);

			break;
	}

	if (length > TRACE_OBJECT_FILE_NAME_LENGTH - 1) {
		DEBUG_TRACE_word(length, "tracer_get_file_name() - OVERFLOW !! length > TRACE_OBJECT_FILE_NAME_LENGTH !!!");
		offset = length - (TRACE_OBJECT_FILE_NAME_LENGTH - 1);
		length = TRACE_OBJECT_FILE_NAME_LENGTH - 1;
	}

	if (index + length > p_raw_object->length) {
		DEBUG_PASS("tracer_get_file_name() - OVERFLOW !! length + index > p_raw_object->length !!!");
		DEBUG_TRACE_word(length, "- Length:");
		DEBUG_TRACE_word(index, "- Index:");
		DEBUG_TRACE_word(p_raw_object->length, "- p_raw_object->length:");
		return;
	}
		
	DEBUG_TRACE_word(index, "tracer_get_file_name() - Index:");
	DEBUG_TRACE_word(length, "tracer_get_file_name() - Length:");

	common_tools_string_substring(p_trace_obj->file_name, (const char*)(p_raw_object->data), index + offset, length, TRACE_OBJECT_RAW_DATA_LENGTH);
	DEBUG_TRACE_STR(p_trace_obj->file_name, "tracer_get_file_name() - File-Name:");
}

static void tracer_get_trace_data(TRACE_OBJECT_RAW* p_raw_object, TRACE_OBJECT* p_trace_obj) {

	u16 index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE;
	//u16 length = p_raw_object->length - TRACE_PARSER_NUM_BYTES_HEADER - TRACE_PARSER_NUM_BYTES_BYTE_COUNT - TRACE_PARSER_NUM_BYTES_TRACE_TYPE - TRACE_PARSER_NUM_BYTES_LINE_NUMBER - TRACER_PARSER_NUM_BYTES_FOOTER;
	//u16 offset = 0;

	switch (p_trace_obj->type) {
		default:
			DEBUG_PASS("tracer_get_trace_data() - UNKNOWN !!! ---");
			break;

		case TRACE_OBJECT_TYPE_PASS  :

			DEBUG_PASS("tracer_get_trace_data() - TRACE_OBJECT_TYPE_PASS");
			p_trace_obj->data_length = 0;
			break;

		case TRACE_OBJECT_TYPE_BYTE  :

			p_trace_obj->data.byte = p_raw_object->data[index];
			p_trace_obj->data_length = 1;
			DEBUG_TRACE_byte(p_trace_obj->data.byte, "tracer_get_trace_data() - TRACE_OBJECT_TYPE_BYTE");

			//index += 1;
			//length -= 1;
			break;

		case TRACE_OBJECT_TYPE_WORD  : 
		
			p_trace_obj->data.word = ((u16)p_raw_object->data[index + 1] << 8);
			p_trace_obj->data.word += (u16)p_raw_object->data[index + 0];
			p_trace_obj->data_length = 2;
			DEBUG_TRACE_word(p_trace_obj->data.word, "tracer_get_trace_data() - TRACE_OBJECT_TYPE_WORD");

			//index += 2;
			//length -= 2;
			break;

		case TRACE_OBJECT_TYPE_LONG  : 
		
			p_trace_obj->data.integer  = ((u16)p_raw_object->data[index + 3] << 24);
			p_trace_obj->data.integer += ((u16)p_raw_object->data[index + 2] << 16);		
			p_trace_obj->data.integer += ((u16)p_raw_object->data[index + 1] << 8);
			p_trace_obj->data.integer += ((u16)p_raw_object->data[index + 0]);
			p_trace_obj->data_length = 4;
			DEBUG_TRACE_long(p_trace_obj->data.integer, "tracer_get_trace_data() - TRACE_OBJECT_TYPE_LONG");

			//index += 4;
			//length -= 4;
			break;

		case TRACE_OBJECT_TYPE_ARRAY : 

			p_trace_obj->data_length = p_raw_object->data[index] > TRACE_OBJECT_ARRAY_LENGTH ? TRACE_OBJECT_ARRAY_LENGTH : p_raw_object->data[index];
			memcpy(p_trace_obj->data.array, p_raw_object->data + index + 1, p_trace_obj->data_length);
			DEBUG_TRACE_N(p_trace_obj->data_length, p_trace_obj->data.array, "tracer_get_trace_data() - TRACE_OBJECT_TYPE_ARRAY");

			//index = TRACE_PARSER_INDEX_TRACE_TYPE + TRACE_PARSER_NUM_BYTES_TRACE_TYPE + 2 + 1 + p_trace_obj->data_length;
			//length -= (1 + p_trace_obj->data_length);
			break;
	}
}

u8 tracer_parse_object(TRACE_OBJECT_RAW* p_raw_object, TRACE_OBJECT* p_object) {

	//hex_dump((const void*)p_raw_object->data, p_raw_object->length, 16, "RX");

	p_object->type = TRACE_OBJECT_TYPE_INVALID;

	u8 header[] = {TRACE_PARSER_HEADER_BYTE_ARRAY};

	if (memcmp(p_raw_object->data, header, TRACE_PARSER_NUM_BYTES_HEADER) != 0) {
		DEBUG_PASS("tracer_parse_object() - Incorrect header");
		return 0;
	}

	DEBUG_TRACE_N(p_raw_object->length, p_raw_object->data, "tracer_parse_object() - RAW trace-object");

	p_object->length = common_tools_number_readU16_MSB(p_raw_object->data + TRACE_PARSER_INDEX_BYTE_COUNT);
	if (p_object->length == 0) {
		DEBUG_PASS("tracer_parse_object() - Length is zero");
		return 0;
	}

	if (tracer_get_trace_type(p_raw_object, p_object) == 0) {
		DEBUG_PASS("tracer_parse_object() - Invalid Tracetype");
		return 0;
	}

	tracer_get_line_number(p_raw_object, p_object);
	tracer_get_file_name(p_raw_object, p_object);
	tracer_get_trace_data(p_raw_object, p_object);

	return 1;

}
