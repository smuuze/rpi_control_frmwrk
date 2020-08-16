/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_jvc.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_PROTOCOL_JVC_H_
#define _IR_PROTOCOL_JVC_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

/*
 *
 */
typedef struct {
	u8 address;
	u16 control;
} JVC_IR_PROTOCOL_COMMAND_TYPE;

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_jvc_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

/*
 *
 */
void ir_protocol_jvc_transmit(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
u8 ir_protocol_jvc_is_busy(void);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_jvc_address_tv(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_jvc_cmd_radio_power(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_up(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_down(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_mute(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_channel_down(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_play(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_pause(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_stop(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
