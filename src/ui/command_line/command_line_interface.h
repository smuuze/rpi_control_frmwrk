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

/**
 * @brief Module to realize an command line argument parser.
 * This module uses a table where all available command-line handler
 * are added. on parsing given cli-arguments the table is iterated 
 * by matching every argument with the entries in that table.
 * If a entry matches the argument, the callback of this entry is
 * executed.
 * 
 * If a cli-argument comes with an paremter the handler callback will parse this parameter
 * Then the handler sends this parameter via an individual signal to all
 * connected slots.
 * 
 */

// --------------------------------------------------------------------------------

/**
 * @brief All CLI-arguments must start with this character
 * If a argument does not have this cahracter as a prefix
 * the argument will be detected as invalid and the 
 * CLI_INVALID_ARGUMENT_SIGNAL is send
 * 
 */
#define COMMAND_LINE_ARGUMENT_PREFIX_CHAR	'-'

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_UNKNWON		"end"

/**
 * @brief CLI-argument to give a configuration file
 * at program start
 * 
 */
#define COMMAND_LINE_ARGUMENT_CFG_FILE		"-cfg"

/**
 * @brief CLI-argument to activate the LCD output
 * 
 */
#define COMMAND_LINE_ARGUMENT_LCD		"-lcd"

/**
 * @brief
 * 
 */
#define COMMAND_LINE_ARGUMENT_CONTROLLER	"-control"

/**
 * @brief CLI-argument to set a file-name on program start
 * 
 */
#define COMMAND_LINE_ARGUMENT_FILE		"-file"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_MQTT		"-mgtt"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_PATH		"-path"

/**
 * @brief CLI-Argument to activate the console-output.
 * This argument does not need any parameter.
 * 
 */
#define COMMAND_LINE_ARGUMENT_CONSOLE		"-console"

/**
 * @brief CLI-argument to shw the help menu of your program
 * 
 */
#define COMMAND_LINE_ARGUMENT_HELP		"-help"

/**
 * @brief CLI-Argument to control a gpio pin.
 * 
 */
#define COMMAND_LINE_ARGUMENT_GPIO		"-gpio"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_DEVICE		"-dev"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_COMMAND		"-cmd"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_REMOTE		"-remote"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_TOPIC		"-topic"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_HOST		"-host"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_CLIENT		"-client"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_MESSAGE		"-msg"

/**
 * @brief Command line argument to give a count of something
 * 
 */
#define COMMAND_LINE_ARGUMENT_N             "-n"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_HELP		"-help"

/**
 * @brief 
 * 
 */
#define COMMAND_LINE_ARGUMENT_HELP_SHORT	"-h"

// --------------------------------------------------------------------------------

/**
 * @brief The following signals are used by this module:
 * 
 * - 	CLI_INVALID_ARGUMENT_SIGNAL
 * 
 * 	This signal is send if a unknown argument was given.
 *	The parameter of this signal is the name of the unknwon argument.
 * 
 *
 * -	CLI_INVALID_PARAMETER_SIGNAL
 * 
 * 	This signal is send if the parameter of an argument is invalid.
 * 
 * 
 * - 	CLI_HELP_REQUESTED_SIGNAL
 * 
 * 	This signal is send if the help was requested.
 * 	The parameter of this signal is a NULL-POINTER.
 * 	You need to implement a help menu that meets the
 * 	needs of your program
 * 
 * 
 * -	CLI_LCD_ACTIVATED_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_LCD
 * 	was given. The parameter of this signal is a NULL-POINTER
 * 	@see COMMAND_LINE_ARGUMENT_LCD
 * 
 * 
 * -	CLI_CONSOLE_ACTIVATED_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_CONSOLE
 * 	was given, The parameter of this signal is a NULL-POINTER
 * 	@see COMMAND_LINE_ARGUMENT_CONSOLE
 * 
 * 
 * -	CLI_ARGUMENT_FILE_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_FILE
 * 	was given. The parameter of this signal is the file-name that was given
 *	with this cli-argument.
 *	@see COMMAND_LINE_ARGUMENT_FILE

 * 
 * -	CLI_ARGUMENT_PATH_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_PATH
 * 	was given. The parameter of this signal is the path that was given
 * 	with the cli-argument
 * 	@see COMMAND_LINE_ARGUMENT_PATH
 * 
 * 
 * -	CLI_ARGUMENT_DEVICE_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_DEVICE
 * 	was given. The parameter of this signal is the device-name that
 * 	was given with the cli-argument
 * 	@see COMMAND_LINE_ARGUMENT_DEVICE
 * 
 * 
 * -	CLI_CONFIGURATION_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_CFG_FILE
 * 	was given. The parameter of this signal is the path to the cfg-file
 * 	that was given with this cli-argument
 *	@see COMMAND_LINE_ARGUMENT_CFG_FILE
 *
 * 
 * -	CLI_MESSAGE_SIGNAL
 * 
 * 	This signal is send if the argument COMMAND_LINE_ARGUMENT_MESSAGE
 * 	was given. The parameter of this signal is the message that was given 
 * 	with this cli-argument.
 * 	@see COMMAND_LINE_ARGUMENT_MESSAGE
 * 
 * -	CLI_NO_ARGUMENT_GIVEN_SIGNAL
 * 
 * -	CLI_UNKNOWN_ARGUMENT_SIGNAL
 * 
 */

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct {
	const char* argument;
	const char* option;
} COMMAND_LINE_ARGUMENT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Type of the callback function pointer
 * that is used to execute a the callback of a cli-argument
 * 
 */
typedef void (*COMMAND_LINE_ARGUMENT_PARSER_CALLBACK)	(const char* parameter);

// --------------------------------------------------------------------------------

/**
 * @brief Type to creatte a cli handler callback.
 * @param argument The string that must be given as cli-argument
 * @param callback pointer to the function that is executed if the argument is recognized
 * 
 */
typedef struct {
	const char* argument;
	COMMAND_LINE_ARGUMENT_PARSER_CALLBACK callback;
} COMMAND_LINE_ARGUMENT_TABLE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the command-line parser.
 * This function initializes the signals of this module
 * and must be called before the command-line parser can be used.
 * 
 */
void command_line_interface_init(void);

/**
 * @brief This function parses the command-line arguments and executes the 
 * handler for each argument. If an argument cannot be parsed the CLI_INVALID_ARGUMENT_SIGNAL
 * is send out. In this case al following arguments are not processed and the program shall
 * inform the user and exit itself.
 * 
 * @param argc 
 * @param argv 
 */
void command_line_interface(int argc, char* argv[]);

// --------------------------------------------------------------------------------

#endif // COMMAND_LINE_INTERFACE_H_
