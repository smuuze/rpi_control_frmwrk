/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	trace_object.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _TRACE_OBJECT_H_
#define _TRACE_OBJECT_H_

// ------------------------------------------------------------------------------

#include "config.h"

// ------------------------------------------------------------------------------

#ifndef TRACE_OBJECT_FILE_NAME_LENGTH
#define TRACE_OBJECT_FILE_NAME_LENGTH		255
#endif
#ifndef TRACE_OBJECT_SOURCE_LINE_LENGTH
#define TRACE_OBJECT_SOURCE_LINE_LENGTH		512
#endif

#ifndef TRACE_OBJECT_ARRAY_LENGTH		
#define TRACE_OBJECT_ARRAY_LENGTH		255
#endif

#ifndef TRACE_OBJECT_RAW_DATA_LENGTH
#define TRACE_OBJECT_RAW_DATA_LENGTH		1024
#endif

// ------------------------------------------------------------------------------

#define TRACE_PARSER_HEADER_BYTE_ARRAY		0xFA,0xFA,0xFA

#define TRACE_PARSER_NUM_BYTES_HEADER		3
#define TRACE_PARSER_NUM_BYTES_BYTE_COUNT	2
#define TRACE_PARSER_NUM_BYTES_TRACE_TYPE	1
#define TRACE_PARSER_NUM_BYTES_LINE_NUMBER	2
#define TRACER_PARSER_NUM_BYTES_FOOTER		1

#define TRACE_PARSER_INDEX_BYTE_COUNT		(TRACE_PARSER_NUM_BYTES_HEADER)
#define TRACE_PARSER_INDEX_TRACE_TYPE		(TRACE_PARSER_NUM_BYTES_HEADER + TRACE_PARSER_NUM_BYTES_BYTE_COUNT)
#define TRACE_PARSER_INDEX_OF_TYPE
#define TRACE_PARSER_INDEX_OF_LINE_NUMBER

#define TRACER_TRACE_TYPE_RAW_PASS		0x01
#define TRACER_TRACE_TYPE_RAW_BYTE		0x02
#define TRACER_TRACE_TYPE_RAW_WORD		0x03
#define TRACER_TRACE_TYPE_RAW_LONG		0x04
#define TRACER_TRACE_TYPE_RAW_ARRAY		0x05

// ------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	TRACE_OBJECT_TYPE_INVALID = 0x00,
	TRACE_OBJECT_TYPE_PASS = 0x01,
	TRACE_OBJECT_TYPE_BYTE,
	TRACE_OBJECT_TYPE_WORD,
	TRACE_OBJECT_TYPE_LONG,
	TRACE_OBJECT_TYPE_ARRAY
} TRACE_OBJECT_TYPE;

/*!
 *
 */
typedef struct {

	TRACE_OBJECT_TYPE type;
	u16 length;
	u16 line_number;
	u8 data_length;
	char file_name[TRACE_OBJECT_FILE_NAME_LENGTH];
	char source_line[TRACE_OBJECT_SOURCE_LINE_LENGTH];

	union {
		u8 byte;
		u16 word;
		u32 integer;
		u8 array[TRACE_OBJECT_ARRAY_LENGTH];
	} data;

} TRACE_OBJECT;

/*!
 *
 */
typedef struct {
	u8 data[TRACE_OBJECT_RAW_DATA_LENGTH];
	u16 length;
} TRACE_OBJECT_RAW;

#endif // _SHC_TRACE_OBJECT_H_
