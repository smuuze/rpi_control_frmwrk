/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    rpi_command_handler_table.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

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
#include "modules/ir/ir_protocol_rpi_cmd_handler.h"
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
