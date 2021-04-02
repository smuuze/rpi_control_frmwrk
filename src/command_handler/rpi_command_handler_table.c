/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_command_handler_table.c
 * \brief
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

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#ifdef RPI_CMD_HANDLER_IO_AVAILABLE
#include "io_management/io_input_controller.h"
#include "io_management/io_output_controller.h"
#endif

#ifdef RPI_CMD_HANDLER_SENSOR_AVAILABLE
#include "command_handler/rpi_cmd_handler_sensor.h"
#endif

#ifdef RPI_CMD_HANDLER_IR_REMOTE_AVAILABLE
#include "command_handler/rpi_command_handler_ir_remote.h"
#endif

#ifdef RPI_CMD_HANDLER_ROUTING_AVAILABLE
#include "command_handler/rpi_cmd_handler_routing.h"
#endif

#include "command_handler/rpi_command_handler.h"

// --------------------------------------------------------------------------------

COMMAND_TABLE_INTERFACE rpi_cmd_handler_table[] = {
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
	{RPI_COMMAND_GET_LIGHT, &rpi_cmd_get_light},
	#endif

	#ifdef RPI_CMD_HANDLER_IR_REMOTE_AVAILABLE
	{RPI_COMMAND_IR_REMOTE, &rpi_cmd_handler_ir_remote},
	#endif

	#ifdef RPI_CMD_HANDLER_ROUTING_AVAILABLE
	{RPI_COMMAND_ROUTING, &rpi_cmd_handler_routing},
	#endif
};

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_table_get_size(void) {
	return (sizeof(rpi_cmd_handler_table) / sizeof(COMMAND_TABLE_INTERFACE));
}
