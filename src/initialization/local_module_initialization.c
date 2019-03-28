/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "local_context.h"

#include "io_controller.h"
#include "io_interface.h"


void local_io_controller_initialization(LOCAL_CONTEXT* context) {
	io_controller_register_output(context->extern_output_01);
	io_controller_register_output(context->extern_output_02);
	io_controller_register_output(context->extern_output_03);
	io_controller_register_output(context->extern_output_04);
}


// ----------------------------------------------------------------------------------------------------------------------------

#include "command_protocol_interface.h"
#include "command_buffer_interface.h"
#include "answer_buffer_interface.h"
#include "command_protocol_control.h"
#include "local_msg_buffer.h"
#include "command_controller.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , control_cmd_buffer, 64)

static COMMAND_BUFFER_INTERFACE control_cmd_buffer_interface = {

	&control_cmd_buffer_bytes_available,			// CMD_BUFFER_INTERFACE_BYTES_AVAILABLE_CALLBACK	bytes_available;
	&control_cmd_buffer_get_byte,				// CMD_BUFFER_INTERFACE_GET_BYTE_CALLBACK		get_byte;
	&control_cmd_buffer_get_word,				// CMD_BUFFER_INTERFACE_GET_WORD_CALLBACK		get_word;
	&control_cmd_buffer_get_long,				// CMD_BUFFER_INTERFACE_GET_LONG_CALLBACK		get_long;
	&control_cmd_buffer_get_N_bytes				// CMD_BUFFER_INTERFACE_GET_N_BYTES_CALLBACK		get_N_bytes;
};


BUILD_LOCAL_MSG_BUFFER( , control_answ_buffer, 64)

static ANSWER_BUFFER_INTERFACE control_answ_buffer_interface = {

	&control_answ_buffer_add_byte,				// ANSWER_BUFFER_INTERFACE_ADD_BYTE_CALLBACK		add_byte;
	&control_answ_buffer_add_word,				// ANSWER_BUFFER_INTERFACE_ADD_WORD_CALLBACK		add_word;
	&control_answ_buffer_add_long,				// ANSWER_BUFFER_INTERFACE_ADD_LONG_CALLBACK		add_long;
	&control_answ_buffer_add_N_bytes			// ANSWER_BUFFER_INTERFACE_ADD_N_BYTES_CALLBACK		add_N_bytes;
};

static COMMAND_PROTOCOL_INTERFACE control_protocol = {
	ROLE_TYPE_SLAVE,
	HW_INTERFACE_SPI_0,
	&cmd_protocol_control_init,
	&cmd_protocol_control_request_pending, 			//CMD_PROTOCOL_INTERFACE_REQUESTED_CALLBACK		request_pending;
	&cmd_protocol_control_get_cmd_code, 			//CMD_PROTOCOL_INTERFACE_GET_COMMAND_CODE_CALLBACK 	get_cmd_code;
	&cmd_protocol_control_set_err_code,			//CMD_PROTOCOL_INTERFACE_SET_ERROR_CODE_CALLBACK 	set_err_code;
	&control_cmd_buffer_interface,
	&control_answ_buffer_interface,
	0 							//struct COMMAND_PROTOCOL_INTERFACE*			next;

};


void local_cmd_protocol_initialization(void) {

	PASS(); // local_cmd_protocol_initialization()
	command_controller_register_protocol(&control_protocol);
}


// ----------------------------------------------------------------------------------------------------------------------------





// ----------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------

