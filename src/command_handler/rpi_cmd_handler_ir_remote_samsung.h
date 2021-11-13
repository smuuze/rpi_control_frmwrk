/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_handler/rpi_cmd_handler_ir_remote_samsung.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _RPI_COMMAND_HANDLER_IR_REMOTE_SAMSUNG_H_
#define _RPI_COMMAND_HANDLER_IR_REMOTE_SAMSUNG_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Generates and sends a ir-command to a samsung device.
 * This functions sends the SAMSUNG_IR_CMD_RECEIVED_SIGNAL signal
 * with the generates ir-command as a parameter.
 * In this context send means, that the generated command is send
 * via the signalslot-interface.
 * This function does not block until the command was transfered over ir-interface.
 * 
 * @param device the type of device for which the command is generated.
 * @param command the comamnd to genearate
 * @return 	CMD_NO_ERR if the command was generated ans send.
 * 		CMD_ERR_INVALID_ARGUMENT if the type of the command is unknown or
 * 		if the device is unknown or unsupported
 */
u8 rpi_cmd_handler_ir_remote_samsung(u8 device, u8 command);

// --------------------------------------------------------------------------------

#endif // _RPI_COMMAND_HANDLER_IR_REMOTE_SAMSUNG_H_
