
#ifndef _BOARD_UNITTEST_H_
#define _BOARD_UNITTEST_H_

// --------------------------------------------------------------------------------

#include "config.h"
#include "board_common_config.h"

// --------------------------------------------------------------------------------

INCLUDE_GPIO ( READY_INOUT )
#define HAS_GPIO_READY_INOUT

INCLUDE_GPIO ( REQUEST_CLIENT )
#define HAS_GPIO_READY_INOUT

INCLUDE_GPIO ( IR_CARRIER_IN )
#define HAS_GPIO_IR_CARRIER_IN

INCLUDE_GPIO ( IR_CARRIER_OUT )
#define HAS_GPIO_IR_CARRIER_OUT

INCLUDE_GPIO ( IR_MOD_OUT )
#define HAS_GPIO_IR_MOD_OUT


// --------------------------------------------------------------------------------

#endif // _BOARD_UNITTEST_H_
