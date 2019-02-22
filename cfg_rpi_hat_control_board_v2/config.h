#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

#define noTRACEAKTIV         // Schaltet generell meine TRACES EIN/AUS
#define TRACES_ENABLED
#define noTRACE_FAKE_RTOS_TIME_STAMP 2	// Trace-Ausgaben mit Timestamp (Anzahl-Bytes 1..4)

///-----------------------------------------------------------------------------
#define EMBEDDED

///-----------------------------------------------------------------------------
#define NO_CARD_READ_DEVICE

#define DEBUS_F1_HANDLER      // Testfunktionen fuer delpro
///----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
///  VERSION
///-----------------------------------------------------------------------------
#define MIN_BIOS_VERSION 0x163

///-----------------------------------------------------------------------------
/// Konfigurationsspeicher
// Konfigurationsspeicher ist groesser als 110 Byte und passt nicht in den deBus
// Buffer deswegen CFG_ALL_SUPPORT = 0
#define CFG_ALL_SUPPORT   0 // CFG-Register 02 wg. Laengenbeschraenkung auf 110 nicht unterstuetzt

// Wenn die Konfiguration groesser als 255 Byte wird, soll dieser Schalter aktiviert werden.
#define  BIG_CFG_MEM 1

#define CFG_DEFAULT_VALUE_SELECTOR 2
///-----------------------------------------------------------------------------
#ifndef __ASSEMBLER__
#include "platine/22B2.h"  // port definition + crystal frequency
#endif

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


/// z.Zt nur TRUE/FALSE koennte aber mal 32 BIT CCM-MAC bedeuten:  //32
#define noCOPRO1_ENCRYPTION           32

#define ASSERT_LOOP_INDICATION_ON() \
        BEEPER_ON();                \
        LED_RED_ON();               \
        LED_GRN_ON();               \
        LED_YELLOW_OFF()

#define ASSERT_LOOP_INDICATION_AFTER_3_SECONDS() \
        BEEPER_OFF()

// ================================================================================
#define HOSTFIFO_BUFFER_SIZE 	512	//Eventuell auch kleiner möglich
#define AIRLINK_SERVICE_TYPE 	2	//Wenn 0: (AIRLINK_NOT_USED)  kein Airlink
					//Wenn 1: (AIRLINK_IN_USE)    Airlink Service in diesem CPU
					//Wenn 2: (AIRLINK_IN_COPRO1) Airlink Service im Copro

//-------------------------------------------------------------------------
#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_PROTO		\
	u8 cmd_handler_version(const COMMAND_BUFFER_INTERFACE* i_cmd_buffer, const ANSWER_BUFFER_INTERFACE* i_answ_buffer);

#define config_LOCAL_COMMAND_HANDLER_TABLE_FUNC_CALLBACK	\
	{CMD_VERSION, &cmd_handler_version},

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// Mus zuletzt eingebunden werden. Alles was oben nicht definiert wurde, wird
// hier auf default Werte gesetzt.
#include "../src/config_default.h"
#endif /* _config_H_ */
