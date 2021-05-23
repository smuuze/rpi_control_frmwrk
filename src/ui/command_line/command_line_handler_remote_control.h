/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_line_handler_remote_control.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_POWER_ON		"jvc_radio_power_on"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_DOWN		"jvc_radio_volume_down"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_UP		"jvc_radio_volume_up"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_MUTE		"jvc_radio_volume_mute"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PLAY			"jvc_radio_play"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PAUSE			"jvc_radio_pause"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_STOP			"jvc_radio_stop"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_UP		"jvc_radio_bass_up"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_DOWN		"jvc_radio_bass_down"
#define CLI_REMOTE_CONTROL_NAME_JVC_RADIO_SOUND_MODE		"jvc_radio_sound_mode"

#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_POWER_ON_HEX_STR	"070C11040B020201"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_DOWN_HEX_STR	"070C11040B020204"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_UP_HEX_STR	"070C11040B020203"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_MUTE_HEX_STR	"070C11040B02021E"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PLAY_HEX_STR		"070C11040B020218"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PAUSE_HEX_STR		"070C11040B020219"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_STOP_HEX_STR		"070C11040B02021A"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_UP_HEX_STR	"070C11040B02021B"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_DOWN_HEX_STR	"070C11040B02021C"
#define CLI_REMOTE_CONTROL_CMD_JVC_RADIO_SOUND_MODE_HEX_STR	"070C11040B02021D"

// --------------------------------------------------------------------------------

void command_line_handler_remote_control(const char* parameter);


