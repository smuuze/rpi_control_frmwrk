
 /*
  * \@file	command_handler/rpi_cmd_handler_sensor.h
  * \author	sebastian lesse
  */

#ifndef _RPI_COMMAND_HANDLER_SNESOR_H_
#define _RPI_COMMAND_HANDLER_SNESOR_H_

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "command_management/protocol_interface.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
u8 rpi_cmd_get_temperature(PROTOCOL_INTERFACE* p_protocol);

/*!
 *
 */
u8 rpi_cmd_get_humidity(PROTOCOL_INTERFACE* p_protocol);

/*!
 *
 */
u8 rpi_cmd_get_light(PROTOCOL_INTERFACE* p_protocol);

/*!
 *
 */
u8 rpi_cmd_get_adc(PROTOCOL_INTERFACE* p_protocol);

#endif // _RPI_COMMAND_HANDLER_SNESOR_H_