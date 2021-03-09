/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	protocol_interface.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _PROTOCOL_INTERFACE_H_
#define _PROTOCOL_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "command_buffer_interface.h"
#include "answer_buffer_interface.h"

// --------------------------------------------------------------------------------

typedef void (*PROTOCOL_INTERFACE_SET_FINISHED_CALLBACK)	(u8 err_code);

// --------------------------------------------------------------------------------

typedef struct PROTOCOL_INTERFACE {

	u8 command_length;
	u8 command_code;
	u8 answer_status;
	u16 arrival_time;

	PROTOCOL_INTERFACE_SET_FINISHED_CALLBACK 		set_finished;
	COMMAND_BUFFER_INTERFACE* 				cmd_buffer;
	ANSWER_BUFFER_INTERFACE* 				answ_buffer;

} PROTOCOL_INTERFACE;

// --------------------------------------------------------------------------------

#endif // _PROTOCOL_INTERFACE_H_
