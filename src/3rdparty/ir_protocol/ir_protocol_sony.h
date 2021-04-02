/**
 * 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_sony.h
 * \brief	This file includes all IR-commands of the Sony BD9400 Blueray Disc-Player.
 * 		Usage:
 * 		- set the pointer to carrier and modulation timer at initialization
 * 		- set the device address into the actual command-container
 * 		- set the data of the requested ir-command into the command.-container
 * 		- start transmitting the command
 * 		- wait until transmitting is done before sending a new command
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_PROTOCOL_SONY_H_
#define _IR_PROTOCOL_SONY_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Structure that holds a single Sony-Infrared command
 * 
 */
typedef struct {
	u8 command;
	u8 device;
	u8 extended;
} SONY_IR_PROTOCOL_COMMAND_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Sets the Access-Pointer to the timer control-units.
 * 
 * @param p_timer_carrier Pointer to the Carrier-Timer
 * @param p_timer_modulator Pointer to the Modulator-Timer
 */
void ir_protocol_sony_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

/**
 * @brief Transmits the actual content of command. This is a non-blocking function.
 * @param p_command valid command that is transmitted.
 * @see u8 ir_protocol_sony_is_busy(void) to check acutal transmit status
 */
void ir_protocol_sony_transmit(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Cheks if the actual transmission is done, or not
 * 
 * @return 1 if transimission is still going on, otherwise 0.
 */
u8 ir_protocol_sony_is_busy(void);

// --------------------------------------------------------------------------------

/**
 * @brief Sets the Address of the SOny BD900 Blueray Disc-Player.
 * 
 * @param p_command COmmand where the Device-Address ist set
 */
void ir_protocol_sony_address_bdplayer(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/**
 * @brief Command to Power On/Off the Sony Blueray Dis-Player
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_power(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to start playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_play(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to stop playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_stop(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to pause playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_pause(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to change the actual audio language
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_audio_language(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to change the actual subtitle language
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_subtitle_language(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to eject tha actual blueray disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_eject(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to return to the previous menu page
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_return(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to enter the top menu of the actual media disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_top_menu(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to activate the pop-up menu of the actual media disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_pop_up_menu(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to go to the next chapter
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_next(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

/**
 * @brief Command to go to the previous chapter
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_previous(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
