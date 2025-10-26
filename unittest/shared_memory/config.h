#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

//-------------------------------------------------------------------------

#include "cpu.h"

///-----------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE  "platine/board_RASPBERRYPI.h"
#include "platine/board_RASPBERRYPI.h"

//-------------------------------------------------------------------------

#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS	                0
#define CFG_FILE_PARSER_AVAILABLE                                       1

//-------------------------------------------------------------------------

#define SHARED_MEMORY_COUNT_64_BYTES          2
#define SHARED_MEMORY_COUNT_128_BYTES         4
#define SHARED_MEMORY_COUNT_256_BYTES         0
#define SHARED_MEMORY_COUNT_512_BYTES         0
#define SHARED_MEMORY_COUNT_1024_BYTES        0
#define SHARED_MEMORY_COUNT_2048_BYTES        0
#define SHARED_MEMORY_COUNT_4096_BYTES        1

//-------------------------------------------------------------------------

#endif /* _config_H_ */
