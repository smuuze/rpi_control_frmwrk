/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_cmd_handler_routing.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _RPI_CMD_HANDLER_ROUTING_H_
#define _RPI_CMD_HANDLER_ROUTING_H_

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "command_management/protocol_interface.h"

//-----------------------------------------------------------------------------

/*
 *
 */
void rpi_cmd_handler_routing_init(void);

/*
 *
 */
u8 rpi_cmd_handler_routing(PROTOCOL_INTERFACE* p_protocol);

//-----------------------------------------------------------------------------

#endif // _RPI_CMD_HANDLER_ROUTING_H_