/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"
#include <fakertos.h>
#include RTOS_H

#include "command_handler_interface.h"
#include "command_buffer_interface.h"
#include "answer_buffer_interface.h"

u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer) {
	
	(void) i_cmd_buffer;
	i_answ_buffer->add_word(VERSION);
	
	return NO_ERR;
}
