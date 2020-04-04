
 /*
  * @file	initialization/command_initialization.c
  * @author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "local_msg_buffer.h"
#include "command_management/command_handler_interface.h"
#include "command_management/command_controller.h"

#include "command_handler/rpi_command_handler.h"

//-----------------------------------------------------------------------------

#ifdef RPI_CMD_HANDLER_AVAILABLE

static COMMAND_TABLE_INTERFACE rpi_cmd_handler_table[] = {
	{RPI_COMMAND_GET_VERSION , &rpi_cmd_get_version},

	#ifdef RPI_CMD_HANDLER_IO_AVAILABLE
	{RPI_COMMAND_GET_INPUT_LIST, &rpi_cmd_get_input_list},
	{RPI_COMMAND_GET_OUTPUT_LIST, &rpi_cmd_get_output_list},
	{RPI_COMMAND_SET_OUTPUT, &rpi_cmd_set_output},
	{RPI_COMMAND_GET_INPUT_STATE, &rpi_cmd_get_input_state},
	{RPI_COMMAND_GET_OUTPUT_STATE, &rpi_cmd_get_output_state},
	#endif

	#ifdef RPI_CMD_HANDLER_SENSOR_AVAILABLE
	{RPI_COMMAND_GET_TEMPERATURE, &rpi_cmd_get_temperature},
	{RPI_COMMAND_GET_HUMIDTY, &rpi_cmd_get_humidity},
	{RPI_COMMAND_GET_ADC, &rpi_cmd_get_adc},
	{RPI_COMMAND_GET_LIGHT, &rpi_cmd_get_light}
	#endif

	#ifdef RPI_CMD_HANDLER_IR_REMOTE_AVAILABLE
	{RPI_COMMAND_IR_REMOTE, &rpi_cmd_handler_ir_remote},
	#endif
};

static COMMAND_HANDLER_INTERFACE rpi_command_handler = {
	(sizeof(rpi_cmd_handler_table) / sizeof(COMMAND_TABLE_INTERFACE)),
	&rpi_cmd_handler_init, 			
	&rpi_cmd_handler_set_request,		
	&rpi_cmd_handler_set_unrequested,
	&rpi_cmd_handler_get_protocol,
	&rpi_cmd_handler_is_requested,		
	&rpi_cmd_handler_get_command_code,	
	rpi_cmd_handler_table, 			
	&rpi_cmd_default_handler,
	0 					
};

#endif

//-----------------------------------------------------------------------------

void command_initialization(void) {

	#ifdef COMMAND_CONTROLLER_AVAILABLE
	{
		DEBUG_PASS("command_initialization()");

		command_controller_init();

		#ifdef RPI_CMD_HANDLER_AVAILABLE
		{
			DEBUG_PASS("command_initialization() - RPI-Command-Handler");
			command_controller_register_handler(&rpi_command_handler);
		}
		#endif
	}
	#endif
}

