/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	answer_buffer_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _ANSWER_BUFFER_INTERFACE_H_
#define _ANSWER_BUFFER_INTERFACE_H_

// --------------------------------------------------------------------------------

typedef u8 (*ANSWER_BUFFER_INTERFACE_START_WRITE_CALLBACK)	(void);
typedef void (*ANSWER_BUFFER_INTERFACE_ADD_BYTE_CALLBACK) 	(u8);
typedef void (*ANSWER_BUFFER_INTERFACE_ADD_WORD_CALLBACK)	(u16);
typedef void (*ANSWER_BUFFER_INTERFACE_ADD_LONG_CALLBACK)	(u32);
typedef u16 (*ANSWER_BUFFER_INTERFACE_ADD_N_BYTES_CALLBACK)	(u16, const u8*);
typedef void (*ANSWER_BUFFER_INTERFACE_STOP_WRITE_CALLBACK)	(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct ANSWER_BUFFER_INTERFACE {
	ANSWER_BUFFER_INTERFACE_START_WRITE_CALLBACK		start_write;
	ANSWER_BUFFER_INTERFACE_ADD_BYTE_CALLBACK		add_byte;
	ANSWER_BUFFER_INTERFACE_ADD_WORD_CALLBACK		add_word;
	ANSWER_BUFFER_INTERFACE_ADD_LONG_CALLBACK		add_long;
	ANSWER_BUFFER_INTERFACE_ADD_N_BYTES_CALLBACK		add_N_bytes;
	ANSWER_BUFFER_INTERFACE_STOP_WRITE_CALLBACK		stop_write;
} ANSWER_BUFFER_INTERFACE;

// --------------------------------------------------------------------------------

#endif // _ANSWER_BUFFER_INTERFACE_H_
