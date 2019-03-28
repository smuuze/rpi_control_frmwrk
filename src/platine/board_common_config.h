#ifndef _PIN_MAP_CFG_H_
#define _PIN_MAP_CFG_H_

#include "../src/common/cpu.h"

#define GPIO_INVERTED					0x80
#define GPIO_IDLE_HIGH					0x40
#define GPIO_IDLE_LOW					0x20
#define GPIO_IDLE_HIGH_Z				0x10

#define GPIO_OUTPUT					0x08
#define GPIO_INPUT					0x00

#define GPIO_PIN_0					0x01
#define GPIO_PIN_1					0x02
#define GPIO_PIN_2					0x04
#define GPIO_PIN_3					0x08
#define GPIO_PIN_4					0x10
#define GPIO_PIN_5					0x20
#define GPIO_PIN_6					0x40
#define GPIO_PIN_7					0x80

#define GPIO_PORT_A					1
#define GPIO_PORT_B					2
#define GPIO_PORT_C					3
#define GPIO_PORT_D					4

typedef struct  {

	const u8 port_id;
	const u8 pin_id;
	const u8 pin_cfg;

} GPIO_DRIVER_PIN_DESCRIPTOR;

#define BUILD_GPIO(pin_name, port_id, pin_id, pin_cfg)		\
	const GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {		\
		port_id,					\
		pin_id,						\
		(pin_cfg)					\
	};

#define INCLUDE_GPIO(pin_name)					\
	extern const GPIO_DRIVER_PIN_DESCRIPTOR pin_name;

#endif
