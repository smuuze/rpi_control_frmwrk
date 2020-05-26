
 /*
  * \@file	command_handler/rpi_cmd_handler_routing.h
  * \author	sebastian lesse
  */

#ifndef _RPI_CMD_HANDLER_ROUTING_H_
#define _RPI_CMD_HANDLER_ROUTING_H_

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "command_management/protocol_interface.h"

//-----------------------------------------------------------------------------

u8 rpi_cmd_handler_routing(PROTOCOL_INTERFACE* p_protocol);

//-----------------------------------------------------------------------------

#endif // _RPI_CMD_HANDLER_ROUTING_H_