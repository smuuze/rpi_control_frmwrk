#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

//-------------------------------------------------------------------------

#include "cpu.h"

///-----------------------------------------------------------------------------
//#ifndef __ASSEMBLER__
//#include "platine/22B2.h"  // port definition + crystal frequency
//#endif

#define BOARD_DESCRIPTION_FILE "platine/board_180920.h"

/// SIO_FOR_HOST : Festlegung ueber welchen UART die deBus-Kommandos empfangen werden
// (SIO_FOR_HOST =  0 ) // Host-Interface kommuniziert ueber UART0
// (SIO_FOR_HOST =  1 ) // Host-Interface kommuniziert ueber UART1
// (SIO_FOR_HOST = 'U') // Host-Interface kommuniziert ueber USB
// (SIO_FOR_HOST = 'S') // Host-Interface kommuniziert ueber SPI
// (SIO_FOR_HOST = 'I') // Host-Interface kommuniziert ueber I2C
// (SIO_FOR_HOST = 'A') // Host-Interface kommuniziert ueber AIRLINK
// (SIO_FOR_HOST = 'V') // Host-Interface kommuniziert ueber virt. SIOV
// (SIO_FOR_HOST = 'W') // Host-Interface kommuniziert ueber virt. SIOW
// (SIO_FOR_HOST = '-') // Bootloader kommuniziert ger nicht
#define SIO_FOR_HOST    		'-'
#define DEFAULT_BAUD_SELECTOR		BD_115200

#define HOST_DRIVER			'S'

#define I2C_CLK_LIMIT 225000


//-------------------------------------------------------------------------
#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Mus zuletzt eingebunden werden. Alles was oben nicht definiert wurde, wird
// hier auf default Werte gesetzt.
#include "config_default.h"

#endif /* _config_H_ */
