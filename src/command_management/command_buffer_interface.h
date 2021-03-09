/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_handler_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _COMMAND_BUFFER_INTERFACE_H_
#define _COMMAND_BUFFER_INTERFACE_H_

// --------------------------------------------------------------------------------

typedef u8 (*CMD_BUFFER_INTERFACE_START_READ_CALLBACK)		(void);
typedef u8 (*CMD_BUFFER_INTERFACE_BYTES_AVAILABLE_CALLBACK)	(void);
typedef u8 (*CMD_BUFFER_INTERFACE_GET_BYTE_CALLBACK) 		(void);
typedef u16 (*CMD_BUFFER_INTERFACE_GET_WORD_CALLBACK)		(void);
typedef u32 (*CMD_BUFFER_INTERFACE_GET_LONG_CALLBACK)		(void);
typedef u16 (*CMD_BUFFER_INTERFACE_GET_N_BYTES_CALLBACK)	(u16, u8*);
typedef void (*CMD_BUFFER_INTERFACE_STOP_READ_CALLBACK)		(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct COMMAND_BUFFER_INTERFACE {
	CMD_BUFFER_INTERFACE_START_READ_CALLBACK		start_read;
	CMD_BUFFER_INTERFACE_BYTES_AVAILABLE_CALLBACK		bytes_available;
	CMD_BUFFER_INTERFACE_GET_BYTE_CALLBACK			get_byte;
	CMD_BUFFER_INTERFACE_GET_WORD_CALLBACK			get_word;
	CMD_BUFFER_INTERFACE_GET_LONG_CALLBACK			get_long;
	CMD_BUFFER_INTERFACE_GET_N_BYTES_CALLBACK		get_N_bytes;
	CMD_BUFFER_INTERFACE_STOP_READ_CALLBACK			stop_read;
} COMMAND_BUFFER_INTERFACE;

// --------------------------------------------------------------------------------

#endif // _COMMAND_BUFFER_INTERFACE_H_
