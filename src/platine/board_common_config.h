#ifndef _PIN_MAP_CFG_H_
#define _PIN_MAP_CFG_H_
//-----------------------------------------------------------------------------

#include "cpu.h"

//-----------------------------------------------------------------------------

#define GPIO_INVERTED					0x80
#define GPIO_IDLE_HIGH					0x40
#define GPIO_IDLE_LOW					0x20
#define GPIO_IDLE_HIGH_Z				0x10

/**
 * @brief Tell the GPIO driver to use this pin as output
 * 
 */
#define GPIO_OUTPUT					0x08

/**
 * @brief Tell the GPIO driver to use this pin as input
 * 
 */
#define GPIO_INPUT					0x00

/**
 * @brief Tell the GPIO driver to do not use this pin
 * This causes that the pin does not have any functionality at all
 * 
 */
#define GPIO_DEACTIVATE					0x01

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
#define GPIO_PORT_E					5

//-----------------------------------------------------------------------------

typedef struct  {

	const u8 port_id;
	const u8 pin_id;
	u8 pin_cfg;

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

//-----------------------------------------------------------------------------

/**
 * @brief Initializes the gpio-driver.
 * This fucntion must be called before any
 * gpio-pin operation is performed
 * 
 */
void gpio_driver_init(void);

/**
 * @brief Deinitializes the gpio-driver.
 * The driver then cannot be used anymore until 
 * gpio_driver_init() is called once again.
 * 
 */
void gpio_driver_deinit(void);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 */
void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 * @param direction 
 */
void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 * @param level 
 */
void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 */
void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 * @return GPIO_DRIVER_LEVEL 
 */
GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 */
void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Deactivates the GPIO pin given by p_pin_descr. The configuration of this pin
 * is set to deactivated. The pin then cannot be used at all.
 * 
 * @param p_pin_descr descriptor of the pin to deactivate.
 */
void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Activates the GPIO pin given by p_pin_descr. The configuration of this pin
 * is set to activated. The pin then can be used.
 * 
 * @param p_pin_descr descriptor of the pin to be activate
 */
void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

//-----------------------------------------------------------------------------

#define BUILD_GPIO(pin_name, port_id, pin_id, pin_cfg)									\
															\
	GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {										\
		port_id,												\
		pin_id,													\
		(pin_cfg)												\
	};														\
															\
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
	}														\
															\
	void pin_name##_activate(void) {										\
		gpio_driver_activate(&pin_name);									\
	}														\
															\
	void pin_name##_deactivate(void) {										\
		gpio_driver_deactivate(&pin_name);									\
	}

//-----------------------------------------------------------------------------

#define INCLUDE_GPIO(pin_name)												\
	extern GPIO_DRIVER_PIN_DESCRIPTOR pin_name;									\
	void pin_name##_drive_high(void);										\
	void pin_name##_drive_low(void);										\
	void pin_name##_no_drive(void);											\
	void pin_name##_toggle_level(void);										\
	void pin_name##_pull_up(void);											\
	void pin_name##_pull_down(void);										\
	void pin_name##_no_pull(void);											\
	u8 pin_name##_is_high_level(void);										\
	u8 pin_name##_is_low_level(void);										\
	void pin_name##_print_state(void);										\
	void pin_name##_activate(void);											\
	void pin_name##_deactivate(void);

//-----------------------------------------------------------------------------

#define INCLUDE_GPIO_ALIAS(pin_name)											\
	void pin_name##_drive_high(void);										\
	void pin_name##_drive_low(void);										\
	void pin_name##_no_drive(void);											\
	void pin_name##_toggle_level(void);										\
	void pin_name##_pull_up(void);											\
	void pin_name##_pull_down(void);										\
	void pin_name##_no_pull(void);											\
	u8 pin_name##_is_high_level(void);										\
	u8 pin_name##_is_low_level(void);										\
	void pin_name##_print_state(void);										\
	void pin_name##_activate(void);											\
	void pin_name##_deactivate(void);

//-----------------------------------------------------------------------------
	
#define INCLUDE_GPIO_REFRENCE(port_id, pin_id)										\
	extern const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_##port_id##_##pin_id;
	
#define GET_GPIO_REFERENCE(port_id, pin_id)										\
	p_pin_##port_id##_##pin_id

//-----------------------------------------------------------------------------

#define HAS_GPIO(name)

//-----------------------------------------------------------------------------

#define GPIO_ALIAS(new_name, exisitng_name)										\
															\
	void exisitng_name##_drive_high(void);										\
	void exisitng_name##_drive_low(void);										\
	void exisitng_name##_no_drive(void);										\
	void exisitng_name##_toggle_level(void);									\
	void exisitng_name##_pull_up(void);										\
	void exisitng_name##_pull_down(void);										\
	void exisitng_name##_no_pull(void);										\
	u8 exisitng_name##_is_high_level(void);										\
	u8 exisitng_name##_is_low_level(void);										\
	void exisitng_name##_print_state(void);										\
	void exisitng_name##_activate(void);										\
	void exisitng_name##_deactivate(void);										\
															\
	void new_name##_drive_high(void) {										\
		exisitng_name##_drive_high();										\
	}														\
															\
	void new_name##_drive_low(void) {										\
		exisitng_name##_drive_low();										\
	}														\
															\
	void new_name##_no_drive(void) {										\
		exisitng_name##_no_drive();										\
	}														\
															\
	void new_name##_toggle_level(void) {										\
		exisitng_name##_toggle_level();										\
	}														\
															\
	void new_name##_pull_up(void) {											\
		exisitng_name##_pull_up();										\
	}														\
															\
	void new_name##_pull_down(void) {										\
		exisitng_name##_pull_down();										\
	}														\
															\
	void new_name##_no_pull(void) {											\
		exisitng_name##_no_pull();										\
	}														\
															\
	u8 new_name##_is_high_level(void) {										\
		return exisitng_name##_is_high_level();									\
	}														\
															\
	u8 new_name##_is_low_level(void) {										\
		return exisitng_name##_is_low_level();									\
	}														\
															\
	void new_name##_print_state(void) {										\
		exisitng_name##_print_state();										\
	}														\
															\
	void new_name##_activate(void) {										\
		exisitng_name##_activate();										\
	}														\
															\
	void new_name##_deactivate(void) {										\
		exisitng_name##_deactivate();										\
	}

#endif

//-----------------------------------------------------------------------------

#define FAKE_GPIO(pin_name, port_id, pin_id, pin_cfg)									\
															\
	static GPIO_DRIVER_DIRECTION _##pin_name##_direction = GPIO_DIRECTION_INPUT;					\
	static GPIO_DRIVER_LEVEL _##pin_name##_level = GPIO_LEVEL_HIGH_Z;						\
	const GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {									\
		port_id,												\
		pin_id,													\
		(pin_cfg)												\
	};														\
	const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_##port_id##_##pin_id = &pin_name;					\
															\
	void pin_name##_drive_high(void) {										\
		_##pin_name##_direction = GPIO_DIRECTION_OUTPUT;							\
		_##pin_name##_level = GPIO_LEVEL_HIGH;									\
	}														\
															\
	void pin_name##_drive_low(void) {										\
		_##pin_name##_direction = GPIO_DIRECTION_OUTPUT;							\
		_##pin_name##_level = GPIO_LEVEL_LOW;									\
	}														\
															\
	void pin_name##_no_drive(void) {										\
		_##pin_name##_direction = GPIO_DIRECTION_INPUT;								\
		_##pin_name##_level = GPIO_LEVEL_HIGH_Z;								\
	}														\
															\
	void pin_name##_toggle_level(void) {										\
	}														\
															\
	void pin_name##_pull_up(void) {											\
		_##pin_name##_direction = GPIO_DIRECTION_INPUT;								\
		_##pin_name##_level = GPIO_LEVEL_HIGH;									\
	}														\
															\
	void pin_name##_pull_down(void) {										\
		_##pin_name##_direction = GPIO_DIRECTION_INPUT;								\
		_##pin_name##_level = GPIO_LEVEL_LOW;									\
															\
	}														\
															\
	void pin_name##_no_pull(void) {											\
		_##pin_name##_direction = GPIO_DIRECTION_INPUT;								\
		_##pin_name##_level = GPIO_LEVEL_HIGH_Z;								\
	}														\
															\
	u8 pin_name##_is_high_level(void) {										\
		return _##pin_name##_level == GPIO_LEVEL_HIGH ? 1 : 0;							\
	}														\
															\
	u8 pin_name##_is_low_level(void) {										\
		return _##pin_name##_level == GPIO_LEVEL_LOW ? 1 : 0;							\
	}														\
															\
	void pin_name##_print_state(void) {										\
	}														\
															\
	void pin_name##_activate(void) {										\
	}														\
															\
	void pin_name##_deactivate(void) {										\
	}
