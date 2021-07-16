#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

//-------------------------------------------------------------------------

#include "cpu.h"

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_UNITTEST.h"
#include "platine/board_UNITTEST.h"

//-------------------------------------------------------------------------

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS		0

//-------------------------------------------------------------------------

#define POWER_UNIT_5V_POWER_UP_TIME_MS		20
#define POWER_UNIT_5V_POWER_DOWN_TIME_MS	2000
#define POWER_UNIT_5V_POWER_ON_CALLBACK		ut_power_mgnmnt_5v_power_on_callback
#define POWER_UNIT_5V_POWER_OFF_CALLBACK	ut_power_mgnmnt_5v_power_off_callback

//-------------------------------------------------------------------------

#include "../src/config_default.h"

//-------------------------------------------------------------------------

#endif /* _config_H_ */
