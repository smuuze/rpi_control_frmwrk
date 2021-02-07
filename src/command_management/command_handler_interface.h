/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_management/command_handler_interface.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _COMMAND_HANDLER_INTERFACE_H_
#define _COMMAND_HANDLER_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "command_buffer_interface.h"
#include "answer_buffer_interface.h"
#include "protocol_interface.h"

// --------------------------------------------------------------------------------

#define CMD_NO_ERR				0
#define CMD_ERR_INVARG				1
#define CMD_ERR_INVALID_COMMAND			2
#define CMD_ERR_INVALID_ARGUMENT		3
#define CMD_ERR_TIMEOUT				4
#define CMD_ERR_BUSY				5
#define CMD_ERR_NO_ROUTE_TO_HOST		6

// --------------------------------------------------------------------------------

#define RPI_COMMAND_GET_VERSION			0x01
#define RPI_COMMAND_GET_INPUT_LIST		0x02
#define RPI_COMMAND_GET_OUTPUT_LIST		0x03
#define RPI_COMMAND_SET_OUTPUT			0x04
#define RPI_COMMAND_GET_OUTPUT_STATE		0x05
#define RPI_COMMAND_GET_INPUT_STATE		0x06
#define RPI_COMMAND_GET_TEMPERATURE		0x07
#define RPI_COMMAND_GET_HUMIDTY			0x08
#define RPI_COMMAND_GET_ADC			0x09
#define RPI_COMMAND_GET_LIGHT			0x0A
#define RPI_COMMAND_IR_REMOTE			0x0B
#define RPI_COMMAND_ROUTING			0x0C

// --------------------------------------------------------------------------------

#define CMD_HANDLER_GET_RESPONSE_COMMAND_CODE(p_buffer)		(p_buffer)->data[0]
#define CMD_HANDLER_GET_RESPONSE_ERROR_CODE(p_buffer)		(p_buffer)->data[1]

// --------------------------------------------------------------------------------

typedef u8 (*CMD_HANDLER_INTERFACE_HANDLE_CALLBACK) 		(PROTOCOL_INTERFACE*);


/*!
 *
 */
typedef struct COMMAND_TABLE_INTERFACE {
	u8 command_id;
	CMD_HANDLER_INTERFACE_HANDLE_CALLBACK handle;
} COMMAND_TABLE_INTERFACE;

// --------------------------------------------------------------------------------

typedef void (*CMD_PROTOCOL_INTERFACE_INITIALIZATION_CALLBACK)			(void);
typedef void (*CMD_PROTOCOL_INTERFACE_SET_REQUESTED_CALLBACK)			(PROTOCOL_INTERFACE*);
typedef void (*CMD_PROTOCOL_INTERFACE_UNSET_REQUESTED_CALLBACK)			(void);
typedef PROTOCOL_INTERFACE* (*CMD_PROTOCOL_INTERFACE_GET_PROTOCOL_CALLBACK)	(void);
typedef u8 (*CMD_PROTOCOL_INTERFACE_IS_REQUESTED_CALLBACK)			(void);
typedef u8 (*CMD_PROTOCOL_INTERFACE_GET_COMMAND_CODE_CALLBACK)			(void);
typedef u8 (*CMD_PROTOCOL_INTERFACE_GET_CMD_TABLE_SIZE_CALLBACK)		(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct COMMAND_HANDLER_INTERFACE {
	CMD_PROTOCOL_INTERFACE_INITIALIZATION_CALLBACK		init;
	CMD_PROTOCOL_INTERFACE_SET_REQUESTED_CALLBACK		set_request;
	CMD_PROTOCOL_INTERFACE_UNSET_REQUESTED_CALLBACK		unset_reqeust;
	CMD_PROTOCOL_INTERFACE_GET_PROTOCOL_CALLBACK		get_protocol;
	CMD_PROTOCOL_INTERFACE_IS_REQUESTED_CALLBACK		is_requested;
	CMD_PROTOCOL_INTERFACE_GET_COMMAND_CODE_CALLBACK 	get_cmd_code;
	CMD_PROTOCOL_INTERFACE_GET_CMD_TABLE_SIZE_CALLBACK	get_table_size;
	COMMAND_TABLE_INTERFACE*				command_handler_table;
	CMD_HANDLER_INTERFACE_HANDLE_CALLBACK			default_handler;
	struct COMMAND_HANDLER_INTERFACE*			next;
} COMMAND_HANDLER_INTERFACE;

#endif // _COMMAND_HANDLER_INTERFACE_H_
