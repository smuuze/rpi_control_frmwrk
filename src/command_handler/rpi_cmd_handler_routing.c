
 /*
  * \@file	command_handler/rpi_cmd_handler_routing.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

#include "copro/copro_interface.h"

#include "common/local_context.h"

#include "command_handler/rpi_command_handler.h"

//-----------------------------------------------------------------------------

/*
 *
 */
typedef void (*COPRO_TABLE_SIGNAL_CALLBACK)		(void* p_arg);

/*
 *
 */
typedef struct {
	COPRO_INTERFACE_OBJECT* copro;
	COPRO_TABLE_SIGNAL_CALLBACK signal_send;
} COPRO_TABLE_TYPE;

//-----------------------------------------------------------------------------

#ifdef COPRO1_IS_AVAILABLE
COPRO_INTERFACE_INCLUDE(COPRO1)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO1_ROUTING_COMMAND_SIGNAL)
#endif

#ifdef COPRO2_IS_AVAILABLE
COPRO_INTERFACE_INCLUDE(COPRO2)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO2_ROUTING_COMMAND_SIGNAL)
#endif

#ifdef COPRO3_IS_AVAILABLE
COPRO_INTERFACE_INCLUDE(COPRO3)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO3_ROUTING_COMMAND_SIGNAL)
#endif

#ifdef COPRO4_IS_AVAILABLE
COPRO_INTERFACE_INCLUDE(COPRO4)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO4_ROUTING_COMMAND_SIGNAL)
#endif



COPRO_TABLE_TYPE copro_table[] = {

	#ifdef COPRO1_IS_AVAILABLE
	{ &COPRO1, &COPRO1_ROUTING_COMMAND_SIGNAL_send },
	#endif

	#ifdef COPRO2_IS_AVAILABLE
	{ &COPRO2, &COPRO2_ROUTING_COMMAND_SIGNAL_send },
	#endif

	#ifdef COPRO3_IS_AVAILABLE
	{ &COPRO3, &COPRO3_ROUTING_COMMAND_SIGNAL_send },
	#endif

	#ifdef COPRO4_IS_AVAILABLE
	{ &COPRO4,  &COPRO4_ROUTING_COMMAND_SIGNAL_send }
	#endif
};

#define COPRO_TABLE_SIZE 		(sizeof(copro_table) / sizeof(COPRO_TABLE_TYPE))

//-----------------------------------------------------------------------------

u8 rpi_cmd_handler_routing(PROTOCOL_INTERFACE* p_protocol) {

	u8 address = p_protocol->cmd_buffer->get_byte();

	DEBUG_TRACE_byte(address, "rpi_cmd_handler_routing() - Address : ");

	u8 i = 0;
	for ( ; i < COPRO_TABLE_SIZE; i++) {
		
		if (address == copro_table[i].copro->get_address()) {
			
			DEBUG_TRACE_byte((i + 1), "rpi_cmd_handler_routing() - COPRO : ");
			copro_table[i].signal_send((void*) p_protocol);

			return CMD_ERR_BUSY;
		}
	}

	p_protocol->set_finished(CMD_ERR_NO_ROUTE_TO_HOST);
	return CMD_ERR_NO_ROUTE_TO_HOST;
}