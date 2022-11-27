/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_handler/rpi_cmd_hander_ir_remote_jvc.c
 * \author	sebastian lesse
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
#include "3rdparty/ir_protocol/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(JVC_IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_jvc_radio(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_jvc(u8 device, u8 command) {

	DEBUG_PASS("rpi_cmd_handler_ir_remote_jvc()");

	switch (device) {
		default:                DEBUG_TRACE_byte(device, "rpi_cmd_handler_ir_remote_jvc() - Unknown device");
                                return CMD_ERR_INVALID_ARGUMENT;
		case IR_DEVICE_RADIO : 	return rpi_cmd_ir_jvc_radio(command);
	}
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_jvc_radio(u8 cmd) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_jvc_radio() - Command:");
	JVC_IR_PROTOCOL_COMMAND_TYPE ir_command;

	switch (cmd) {

		default:			    DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_jvc_radio() - Unknown Command:");
                                return CMD_ERR_INVALID_ARGUMENT;

		case IR_COMMAND_POWER_ON : 	ir_protocol_jvc_cmd_radio_power(&ir_command); break;
		case IR_COMMAND_POWER_OFF :	ir_protocol_jvc_cmd_radio_power(&ir_command); break;

		case IR_COMMAND_VOLUME_UP :	ir_protocol_jvc_cmd_radio_volume_up(&ir_command); break;
		case IR_COMMAND_VOLUME_DOWN :	ir_protocol_jvc_cmd_radio_volume_down(&ir_command); break;

		case IR_COMMAND_PLAY :		ir_protocol_jvc_cmd_radio_play(&ir_command); break;
		case IR_COMMAND_PAUSE :		ir_protocol_jvc_cmd_radio_pause(&ir_command); break;
		case IR_COMMAND_STOP :		ir_protocol_jvc_cmd_radio_stop(&ir_command); break;

		case IR_COMMAND_BASS_UP :	ir_protocol_jvc_cmd_radio_bass_up(&ir_command); break;
		case IR_COMMAND_BASS_DOWN :	ir_protocol_jvc_cmd_radio_bass_down(&ir_command); break;
		case IR_COMMAND_SOUND_MODE :	ir_protocol_jvc_cmd_radio_sound_mode(&ir_command); break;
	}

	ir_protocol_jvc_address_radio(&ir_command);

	JVC_IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);

	return CMD_NO_ERR;
}