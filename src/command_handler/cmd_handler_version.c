/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	cmd_handler_version.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "command_handler_interface.h"
#include "command_buffer_interface.h"
#include "answer_buffer_interface.h"

// --------------------------------------------------------------------------------

u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer) {
	
	(void) i_cmd_buffer;
	i_answ_buffer->add_word(VERSION);
	
	return NO_ERR;
}
