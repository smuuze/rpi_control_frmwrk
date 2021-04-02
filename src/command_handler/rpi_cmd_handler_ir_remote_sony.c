/**
 * --------------------------------------------------------------------------------
 *
 * @file 	rpi_cmd_handler_ir_remote_sony.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-03-20
 * 
 * @copyright 	Copyright (c) 2021
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "command_management/command_handler_interface.h"
#include "command_handler/rpi_command_handler_ir_remote.h"
#include "3rdparty/ir_protocol/ir_protocol_sony.h"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SONY_IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief 	Evaluates the Given command-type, builds the command and sends the 
 * 		SONY_IR_CMD_RECEIVED_SIGNAL
 * 
 * @param command Type of command 
 * @return 	CMD_NO_ERR if the command is known and the SONY_IR_CMD_RECEIVED_SIGNAL was send
 * 		CMD_ERR_INVALID_ARGUMENT if the command is unknown
 */
static inline u8 rpi_cmd_ir_sony_bdplayer(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_sony(u8 device, u8 command) {

	DEBUG_PASS("rpi_cmd_handler_ir_remote_sony()");

	switch (device) {
		default:				return CMD_ERR_INVALID_ARGUMENT;
		case IR_DEVICE_BLUE_RAY_PLAYER : 	return rpi_cmd_ir_sony_bdplayer(command);
	}
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_sony_bdplayer(u8 cmd) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_sony_bdplayer() - Command:");
	SONY_IR_PROTOCOL_COMMAND_TYPE ir_command;

	switch (cmd) {

		default:				return CMD_ERR_INVALID_ARGUMENT;

		case IR_COMMAND_POWER_ON : 		ir_protocol_sony_cmd_bdplayer_power(&ir_command); break;
		case IR_COMMAND_POWER_OFF :		ir_protocol_sony_cmd_bdplayer_power(&ir_command); break;

		case IR_COMMAND_TOP_MENU :		ir_protocol_sony_cmd_bdplayer_top_menu(&ir_command); break;
		case IR_COMMAND_POP_UP_MENU :		ir_protocol_sony_cmd_bdplayer_pop_up_menu(&ir_command); break;
		case IR_COMMAND_RETURN :		ir_protocol_sony_cmd_bdplayer_return(&ir_command); break;

		case IR_COMMAND_PLAY :			ir_protocol_sony_cmd_bdplayer_play(&ir_command); break;
		case IR_COMMAND_PAUSE :			ir_protocol_sony_cmd_bdplayer_pause(&ir_command); break;
		case IR_COMMAND_STOP :			ir_protocol_sony_cmd_bdplayer_stop(&ir_command); break;

		case IR_COMMAND_AUDIO_LANGUAGE :	ir_protocol_sony_cmd_bdplayer_audio_language(&ir_command); break;
		case IR_COMMAND_SUBTITLE_LANGUAGE :	ir_protocol_sony_cmd_bdplayer_subtitle_language(&ir_command); break;

		case IR_COMMAND_NEXT :			ir_protocol_sony_cmd_bdplayer_next(&ir_command); break;
		case IR_COMMAND_PREVIOUS :		ir_protocol_sony_cmd_bdplayer_previous(&ir_command); break;
	
		case IR_COMMAND_EJECT :			ir_protocol_sony_cmd_bdplayer_eject(&ir_command); break;
		
	}

	ir_protocol_sony_address_bdplayer(&ir_command);

	SONY_IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);

	return CMD_NO_ERR;
}
