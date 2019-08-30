/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "local_msg_buffer.h"
#include "command_handler_interface.h"
#include "command_controller.h"

#include "rpi_command_handler.h"

static COMMAND_TABLE_INTERFACE rpi_cmd_handler_table[] = {
	{RPI_COMMAND_GET_VERSION , &rpi_cmd_get_version},
	{RPI_COMMAND_GET_INPUT_LIST, &rpi_cmd_get_input_list},
	{RPI_COMMAND_GET_OUTPUT_LIST, &rpi_cmd_get_output_list},
	{RPI_COMMAND_SET_OUTPUT, &rpi_cmd_set_output},
	{RPI_COMMAND_GET_INPUT_STATE, &rpi_cmd_get_input_state},
	{RPI_COMMAND_GET_OUTPUT_STATE, &rpi_cmd_get_output_state},
	{RPI_COMMAND_GET_TEMPERATURE, &rpi_cmd_get_temperature},
	{RPI_COMMAND_GET_HUMIDTY, &rpi_cmd_get_humidity},
	{RPI_COMMAND_GET_ADC, &rpi_cmd_get_adc},
	{RPI_COMMAND_GET_LIGHT, &rpi_cmd_get_light}
};


static COMMAND_HANDLER_INTERFACE rpi_command_handler = {
	(sizeof(rpi_cmd_handler_table) / sizeof(COMMAND_TABLE_INTERFACE)),//rpi_command_handler_table_size, 	// u8 num_command_handler;
	&rpi_cmd_handler_init, 			// CMD_PROTOCOL_INTERFACE_INITIALIZATION_CALLBACK	init;
	&rpi_cmd_handler_set_request,		// CMD_PROTOCOL_INTERFACE_SET_REQUESTED_CALLBACK	set_request;
	&rpi_cmd_handler_is_requested,		// CMD_PROTOCOL_INTERFACE_IS_REQUESTED_CALLBACK		is_requested;
	&rpi_cmd_handler_get_command_code,	// CMD_PROTOCOL_INTERFACE_GET_COMMAND_CODE_CALLBACK 	get_cmd_code;
	rpi_cmd_handler_table, 			// COMMAND_TABLE_INTERFACE*				command_handler_table;
	&rpi_cmd_default_handler,
	0 					//struct COMMAND_HANDLER_INTERFACE*			next;
};

void command_initialization(void) {

	command_controller_register_handler(&rpi_command_handler);
}

