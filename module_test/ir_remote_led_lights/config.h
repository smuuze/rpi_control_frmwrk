#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

//-------------------------------------------------------------------------

#include "cpu.h"

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_UNITTEST.h"
#include "platine/board_UNITTEST.h"

//-------------------------------------------------------------------------

#define I2C_CLK_LIMIT 225000

//-------------------------------------------------------------------------

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS	0

//-------------------------------------------------------------------------

#include "../src/config_default.h"

#endif /* _config_H_ */
