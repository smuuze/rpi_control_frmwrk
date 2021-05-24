
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

INCLUDE_GPIO ( 	LCD_RS )
#define HAS_GPIO_LCD_RS_OUT

INCLUDE_GPIO ( 	LCD_EN )
#define HAS_GPIO_LCD_EN_OUT

INCLUDE_GPIO ( 	LCD_D4 )
#define HAS_GPIO_LCD_D4_OUT

INCLUDE_GPIO ( 	LCD_D5 )
#define HAS_GPIO_LCD_D5_OUT

INCLUDE_GPIO ( 	LCD_D6 )
#define HAS_GPIO_LCD_D6_OUT

INCLUDE_GPIO ( 	LCD_D7 )
#define HAS_GPIO_LCD_D7_OUT

// --------------------------------------------------------------------------------

#endif // _BOARD_UNITTEST_H_
