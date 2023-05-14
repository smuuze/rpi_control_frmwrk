#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

//-------------------------------------------------------------------------

#include "cpu.h"

//-------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                         			 "platine/board_UNITTEST.h"
#include "platine/board_UNITTEST.h"

//-------------------------------------------------------------------------

#define MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS              100
#define MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS         100
#define MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS                 1000
#define MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS                     99

//-------------------------------------------------------------------------

#include "../src/config_default.h"

#endif /* _config_H_ */
