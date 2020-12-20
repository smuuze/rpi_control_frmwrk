
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
#include "command_management/command_controller.h"

#include "command_handler/rpi_command_handler.h"

//-----------------------------------------------------------------------------

void command_initialization(void) {

	#ifdef COMMAND_CONTROLLER_AVAILABLE
	{
		DEBUG_PASS("command_initialization()");
		command_controller_init();
	}
	#endif

	#ifdef RPI_CMD_HANDLER_AVAILABLE
	{
		DEBUG_PASS("command_initialization() - RPI-Command-Handler");
		rpi_cmd_handler_init();
	}
	#endif
}
