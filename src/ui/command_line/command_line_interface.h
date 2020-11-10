/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_line_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef COMMAND_LINE_INTERFACE_H_
#define COMMAND_LINE_INTERFACE_H_

// --------------------------------------------------------------------------------

#define COMMAND_LINE_ARGUMENT_CFG_FILE		"-cfg"
#define COMMAND_LINE_ARGUMENT_LCD		"-lcd"
#define COMMAND_LINE_ARGUMENT_CONTROLLER	"-control"
#define COMMAND_LINE_ARGUMENT_FILE		"-file"
#define COMMAND_LINE_ARGUMENT_MQTT		"-mgtt"
#define COMMAND_LINE_ARGUMENT_PATH		"-path"
#define COMMAND_LINE_ARGUMENT_CONSOLE		"-console"
#define COMMAND_LINE_ARGUMENT_HELP		"-help"
#define COMMAND_LINE_ARGUMENT_DEVICE		"-dev"
#define COMMAND_LINE_ARGUMENT_COMMAND		"-cmd"

#define COMMAND_LINE_ARGUMENT_TOPIC		"-topic"
#define COMMAND_LINE_ARGUMENT_HOST		"-host"
#define COMMAND_LINE_ARGUMENT_CLIENT		"-client"
#define COMMAND_LINE_ARGUMENT_MESSAGE		"-msg"

#define COMMAND_LINE_ARGUMENT_HELP		"-help"
#define COMMAND_LINE_ARGUMENT_HELP_SHORT	"-h"

// --------------------------------------------------------------------------------

/*
 *
 */
void command_line_interface_init(void);

/*
 *
 */
void command_line_interface(int argc, char* argv[]);

// --------------------------------------------------------------------------------

#endif // COMMAND_LINE_INTERFACE_H_