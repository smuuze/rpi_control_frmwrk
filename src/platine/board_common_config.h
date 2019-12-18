#ifndef _PIN_MAP_CFG_H_
#define _PIN_MAP_CFG_H_

#include "cpu.h"

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

typedef enum {
	GPIO_LEVEL_LOW = 0x00,  //!< PIN_LEVEL_LOW
	GPIO_LEVEL_HIGH = 0x01, //!< PIN_LEVEL_HIGH
	GPIO_LEVEL_HIGH_Z = 0x2,//!< PIN_LEVEL_HIGH_Z
} GPIO_DRIVER_LEVEL;

typedef enum {
	GPIO_DIRECTION_INPUT = 0x00,
	GPIO_DIRECTION_OUTPUT = 0x01,
} GPIO_DRIVER_DIRECTION;

void gpio_driver_init(void);
void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction);
void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level);
void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

#define BUILD_GPIO(pin_name, port_id, pin_id, pin_cfg)									\
	const GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {									\
		port_id,												\
		pin_id,													\
		(pin_cfg)												\
	};														\
	const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_##port_id##_##pin_id = &pin_name;					\
															\
	void pin_name##_drive_high(void) {										\
		/* OUTPUT_HIGH_LEVEL  */										\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_OUTPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH);							\
	}														\
															\
	void pin_name##_drive_low(void) {										\
		/* OUTPUT_LOW_LEVEL  */											\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_OUTPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_LOW);							\
															\
	}														\
															\
	void pin_name##_no_drive(void) {										\
		/* INPUT_HIGH_Z  */											\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH_Z);							\
															\
	}														\
															\
	void pin_name##_toggle_level(void) {										\
		gpio_driver_toggle_level(&pin_name);									\
	}														\
															\
	void pin_name##_pull_up(void) {											\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH);							\
															\
	}														\
															\
	void pin_name##_pull_down(void) {										\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_LOW);							\
															\
	}														\
															\
	void pin_name##_no_pull(void) {											\
		/* INPUT_HIGH_Z  */											\
		gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);						\
		gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH_Z);							\
	}														\
															\
	u8 pin_name##_is_high_level(void) {										\
		return gpio_driver_get_level(&pin_name) == GPIO_LEVEL_HIGH ? 1 : 0;					\
	}														\
															\
	u8 pin_name##_is_low_level(void) {										\
		return gpio_driver_get_level(&pin_name) == GPIO_LEVEL_LOW ? 1 : 0;					\
	}														\
															\
	void pin_name##_print_state(void) {										\
		gpio_driver_print_pin_state(&pin_name);									\
	}

#define INCLUDE_GPIO(pin_name)												\
	extern const GPIO_DRIVER_PIN_DESCRIPTOR pin_name;								\
	void pin_name##_drive_high(void);										\
	void pin_name##_drive_low(void);										\
	void pin_name##_no_drive(void);											\
	void pin_name##_pull_up(void);											\
	void pin_name##_pull_down(void);										\
	void pin_name##_no_pull(void);											\
	u8 pin_name##_is_high_level(void);										\
	u8 pin_name##_is_low_level(void);										\
	void pin_name##_print_state(void);
	
#define INCLUDE_GPIO_REFRENCE(port_id, pin_id)			\
	extern const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_##port_id##_##pin_id;
	
#define GET_GPIO_REFERENCE(port_id, pin_id)			\
	p_pin_##port_id##_##pin_id

#define HAS_GPIO(name)			

#endif
