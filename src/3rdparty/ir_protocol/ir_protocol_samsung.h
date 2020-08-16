/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_samusung.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_PROTOCOL_SAMSUNG_H_
#define _IR_PROTOCOL_SAMSUNG_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

/*
 *
 */
typedef struct {
	u8 address;
	u8 control;
} SAMSUNG_IR_PROTOCOL_COMMAND_TYPE;

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

/*
 *
 */
void ir_protocol_samsung_transmit(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
u8 ir_protocol_samsung_is_busy(void);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_address_tv(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_cmd_tv_power(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_0(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_1(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_2(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_3(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_4(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_5(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_6(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_7(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_8(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_9(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_enter(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_exit(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_return(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_left(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_right(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_play(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_pause(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_stop(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
