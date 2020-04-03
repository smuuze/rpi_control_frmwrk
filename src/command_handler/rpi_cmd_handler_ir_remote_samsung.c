/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_hanlder/rpi_cmd_hander_ir_remote_samsung.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "command_handler/rpi_command_handler_ir_remote.h"
#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_samsung_tv(u8 command);

static inline u8 rpi_cmd_ir_sasmsung_tv_power_on(void);
static inline u8 rpi_cmd_ir_samsung_tv_power_off(void);

static inline u8 rpi_cmd_ir_sasmsung_tv_volume_up(void);
static inline u8 rpi_cmd_ir_samsung_tv_volume_down(void);

static inline u8 rpi_cmd_ir_sasmsung_tv_channel_up(void);
static inline u8 rpi_cmd_ir_samsung_tv_channel_down(void);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_samsung(u8 device, u8 command) {

	switch (device) {
		default:
		case IR_DEVICE_TV : 	DEBUG_PASS("rpi_cmd_handler_ir_remote_samsung() - TV");
					return rpi_cmd_ir_samsung_tv(command);
	}
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_samsung_tv(u8 cmd) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_samsung_tv() - Command:");

	switch (device) {
		default:
		case IR_COMMAND_POWER_ON : 	return rpi_cmd_ir_samsung_tv(cmd);
		case IR_COMMAND_POWER_OFF :	return rpi_cmd_ir_samsung_tv(cmd);
		case IR_COMMAND_VOLUME_UP :	return rpi_cmd_ir_sasmsung_tv_volume_up(cmd);
		case IR_COMMAND_VOLUME_DOWN :	return rpi_cmd_ir_samsung_tv_volume_down(cmd);
		case IR_COMMAND_CHANNEL_UP :	return rpi_cmd_ir_sasmsung_tv_channel_up(cmd);
		case IR_COMMAND_CHANNEL_DOWN :	return rpi_cmd_ir_samsung_tv_channel_down(cmd);
	}
}

static inline u8 rpi_cmd_ir_sasmsung_tv_power_on(void) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_sasmsung_tv_power_on()");

	SAMSUNG_IR_PROTOCOL_COMMAND_TYPE ir_command;
	ir_protocol_samsung_cmd_tv_power(&ir_command);
	ir_protocol_samsung_transmit(ir_command);
}

static inline u8 rpi_cmd_ir_samsung_tv_power_off(void) {

}

static inline u8 rpi_cmd_ir_sasmsung_tv_volume_up(void) {

}

static inline u8 rpi_cmd_ir_samsung_tv_volume_down(void) {

}

static inline u8 rpi_cmd_ir_sasmsung_tv_channel_up(void) {

}

static inline u8 rpi_cmd_ir_samsung_tv_channel_down(void) {

}
