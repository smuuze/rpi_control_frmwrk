
 /*
  * \@file	command_handler/rpi_cmd_handler_sensor.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

#include "common/local_context.h"

//-----------------------------------------------------------------------------

u8 rpi_cmd_get_temperature(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_temperature()

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).temperature.actual);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).temperature.maximal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).temperature.minimal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).temperature.mean);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}

u8 rpi_cmd_get_humidity(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_humidity()

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).humidity.actual);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).humidity.maximal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).humidity.minimal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).humidity.mean);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}

u8 rpi_cmd_get_light(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_light()

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).light.actual);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).light.maximal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).light.minimal);
	p_protocol->answ_buffer->add_byte(GET_SYSTEM(data).light.mean);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}

u8 rpi_cmd_get_adc(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_adc()

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.channel_0);
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.channel_1);
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.channel_2);
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.channel_3);
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.temperature);
	p_protocol->answ_buffer->add_word(GET_SYSTEM(data).adc.humidity);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}
