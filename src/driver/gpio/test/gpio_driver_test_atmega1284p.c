/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "driver/gpio/gpio_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------

#define GPIO_DRIVER_GET_PORT(p_pin_descr)				(p_pin_descr->port_id)
#define GPIO_DRIVER_GET_PIN(p_pin_descr)				(p_pin_descr->pin_id == GPIO_PIN_0) ? 0 :			\
									(p_pin_descr->pin_id == GPIO_PIN_1) ? 1 :			\
									(p_pin_descr->pin_id == GPIO_PIN_2) ? 2 :			\
									(p_pin_descr->pin_id == GPIO_PIN_3) ? 3 :			\
									(p_pin_descr->pin_id == GPIO_PIN_4) ? 4 :			\
									(p_pin_descr->pin_id == GPIO_PIN_5) ? 5 :			\
									(p_pin_descr->pin_id == GPIO_PIN_6) ? 6 : 7

#define SET_PIN_AS_INPUT(port, pin)					pin_table[port][pin].direction = GPIO_DIRECTION_INPUT
#define SET_PIN_AS_OUTPUT(port, pin)					pin_table[port][pin].direction = GPIO_DIRECTION_OUTPUT

#define GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr)			((p_pin_descr->pin_cfg & GPIO_INVERTED) != 0 ? 1 : 0)
#define GPIO_DRIVER_INVERT_LEVEL(level)					level = (level == GPIO_LEVEL_HIGH) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH

#define GPIO_DRIVER_SET_PIN_DIRECTION(port, pin, direction)		if (direction == GPIO_DIRECTION_INPUT) {			\
										SET_PIN_AS_INPUT(port, pin);				\
                                                                    	} else {							\
                                                                        	SET_PIN_AS_OUTPUT(port, pin);				\
                                                                    	}
#define GPIO_DRIVER_GET_PIN_DIRECTION(port, pin)			pin_table[port][pin].direction

#define SET_PIN_HIGH_LEVEL(port, pin)                           	pin_table[port][pin].level = GPIO_LEVEL_HIGH
#define SET_PIN_LOW_LEVEL(port, pin)                            	pin_table[port][pin].level = GPIO_LEVEL_LOW
#define SET_PIN_HIGH_Z(port, pin)                      			pin_table[port][pin].level = GPIO_LEVEL_HIGH_Z

#define GPIO_DRIVER_SET_PIN_LEVEL(port, pin, level)    			if (level == GPIO_LEVEL_HIGH) {					\
                                                                        	SET_PIN_HIGH_LEVEL(port, pin);				\
                                                                    	} else if (level == GPIO_LEVEL_LOW) {				\
                                                                        	SET_PIN_LOW_LEVEL(port, pin);				\
                                                                    	} else {							\
                                                                        	SET_PIN_HIGH_Z(port, pin);				\
                                                                    	}

#define GPIO_DRIVER_GET_PIN_LEVEL(port, pin)				pin_table[port][pin].level
									// != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW

#define GPIO_DRIVER_TOGGLE_PIN_LEVEL(port, pin)				if (GPIO_DRIVER_GET_PIN_LEVEL(port, pin) == GPIO_LEVEL_HIGH) {	\
										SET_PIN_HIGH_LEVEL(port, pin);				\
									} else {							\
										SET_PIN_LOW_LEVEL(port, pin);				\
									}

#define GPIO_DRIVER_IS_OUTPUT(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_OUTPUT) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_IDLE_LOW) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_IDLE_HIGH) != 0 ? 1 : 0)

//-----------------------------------------------------------------------------

INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_7)

//-----------------------------------------------------------------------------

typedef struct GPIO_TEST_PIN {
	u8 direction;
	u8 level;
} GPIO_TEST_PIN_TYPE;

static GPIO_TEST_PIN_TYPE pin_table[4][8];

//-----------------------------------------------------------------------------

void gpio_driver_init(void) {

	DEBUG_PASS("gpio_driver_init()");
	
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_0));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_1));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_2));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_3));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_4));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_5));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_6));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_7));
	
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_0));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_1));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_2));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_3));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_4));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_5));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_6));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_7));
	
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_0));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_1));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_2));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_3));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_4));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_5));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_6));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_7));
	
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_0));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_1));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_2));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_3));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_4));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_5));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_6));
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_7));

}

void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	DEBUG_PASS("gpio_driver_init_pin()");
	DEBUG_TRACE_byte(GPIO_DRIVER_GET_PORT(p_pin_descr), " - PORT");
	DEBUG_TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr), " - PIN");

	//is input or output
	if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) != 0) {
		DEBUG_PASS(" - IS_OUTPUT");
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_OUTPUT);
	} else {
		DEBUG_PASS(" - IS_INPUT");
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_INPUT);
	}

	SYSTEM_INTERFACE_GPIO_LEVEL level = GPIO_LEVEL_HIGH_Z;

	if (GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr) != 0) {
		DEBUG_PASS(" - IS ILDE LOW");
		level = GPIO_LEVEL_LOW;

	} else if (GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr) != 0) {
		DEBUG_PASS(" - IS IDLE HIGH");
		level = GPIO_LEVEL_HIGH;
	} else {
		DEBUG_PASS(" - IS IDLE HIGH-Z");
	}

	gpio_driver_set_level(p_pin_descr, level);
}

void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_DIRECTION direction) {

	DEBUG_PASS("gpio_driver_set_direction()");

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	DEBUG_TRACE_byte(port_id, " - PORT");
	DEBUG_TRACE_byte(pin_num, " - PIN");
	DEBUG_TRACE_byte(direction, " - DIRECTION");

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_SET_PIN_DIRECTION(0, pin_num, direction) break;
		case GPIO_PORT_B: GPIO_DRIVER_SET_PIN_DIRECTION(1, pin_num, direction) break;
		case GPIO_PORT_C: GPIO_DRIVER_SET_PIN_DIRECTION(2, pin_num, direction) break;
		case GPIO_PORT_D: GPIO_DRIVER_SET_PIN_DIRECTION(3, pin_num, direction) break;
		default: break;
	}
}

void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_LEVEL level) {

	DEBUG_PASS("gpio_driver_set_level()");

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	DEBUG_TRACE_byte(port_id, " - PORT");
	DEBUG_TRACE_byte(pin_num, " - PIN"); 
	DEBUG_TRACE_byte(level, " - LEVEL"); 

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {

		GPIO_DRIVER_INVERT_LEVEL(level);
		DEBUG_TRACE_byte(level, " - LEVEL inverted");
	}

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_SET_PIN_LEVEL(0, pin_num, level) break;
		case GPIO_PORT_B: GPIO_DRIVER_SET_PIN_LEVEL(1, pin_num, level) break;
		case GPIO_PORT_C: GPIO_DRIVER_SET_PIN_LEVEL(2, pin_num, level) break;
		case GPIO_PORT_D: GPIO_DRIVER_SET_PIN_LEVEL(3, pin_num, level) break;
		default: break;
	}
}

void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	DEBUG_PASS("gpio_driver_toggle_level()");

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	DEBUG_TRACE_byte(port_id, " - PORT");
	DEBUG_TRACE_byte(pin_num, " - PIN");

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_TOGGLE_PIN_LEVEL(0, pin_num); break;
		case GPIO_PORT_B: GPIO_DRIVER_TOGGLE_PIN_LEVEL(1, pin_num); break;
		case GPIO_PORT_C: GPIO_DRIVER_TOGGLE_PIN_LEVEL(2, pin_num); break;
		case GPIO_PORT_D: GPIO_DRIVER_TOGGLE_PIN_LEVEL(3, pin_num); break;
		default: break;
	}
}

SYSTEM_INTERFACE_GPIO_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	DEBUG_PASS("gpio_driver_get_level()");

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	u8 pin_level = GPIO_LEVEL_LOW;

	switch (port_id) {
		case GPIO_PORT_A: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(0, pin_num); break;
		case GPIO_PORT_B: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(1, pin_num); break;
		case GPIO_PORT_C: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(2, pin_num); break;
		case GPIO_PORT_D: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(3, pin_num); break;
		default: break;
	}

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
		GPIO_DRIVER_INVERT_LEVEL(pin_level);
		DEBUG_PASS(" - LEVEL inverted");
	}

	DEBUG_TRACE_byte(port_id, " - PORT");
	DEBUG_TRACE_byte(pin_num, " - PIN");
	DEBUG_TRACE_byte(pin_level, " - LEVEL");

	return pin_level;
}

void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	#ifdef TRACER_ENABLED
	{
		ALWAYS_PASS("gpio_driver_print_pin_state()");

		u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
		u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

		u8 pin_level = GPIO_LEVEL_LOW;
		u8 pin_direction = GPIO_LEVEL_LOW;

		switch (port_id) {
			case GPIO_PORT_A: 	pin_level = GPIO_DRIVER_GET_PIN_LEVEL(0, pin_num);
						pin_direction = GPIO_DRIVER_GET_PIN_DIRECTION(0, pin_num);
						break;

			case GPIO_PORT_B: 	pin_level = GPIO_DRIVER_GET_PIN_LEVEL(1, pin_num);
						pin_direction = GPIO_DRIVER_GET_PIN_DIRECTION(1, pin_num);
						break;

			case GPIO_PORT_C: 	pin_level = GPIO_DRIVER_GET_PIN_LEVEL(2, pin_num);
						pin_direction = GPIO_DRIVER_GET_PIN_DIRECTION(2, pin_num);
						break;

			case GPIO_PORT_D: 	pin_level = GPIO_DRIVER_GET_PIN_LEVEL(3, pin_num);
						pin_direction = GPIO_DRIVER_GET_PIN_DIRECTION(3, pin_num);
						break;
			default: break;
		}

		ALWAYS_TRACE_byte(port_id, " - PORT");
		ALWAYS_TRACE_byte(pin_num, " - PIN");

		if (pin_direction == GPIO_DIRECTION_INPUT) {
			ALWAYS_TRACE_byte(pin_direction, " - IS INPUT");
		} else {
			ALWAYS_TRACE_byte(pin_direction, " - IS OUTPUT");
		}

		if (pin_level == GPIO_LEVEL_HIGH) {
			ALWAYS_TRACE_byte(pin_level, " - IS HIGH-LEVEL");

		} else if (pin_level == GPIO_LEVEL_LOW) {
			ALWAYS_TRACE_byte(pin_level, " - IS LOW-LEVEL");

		} else if (pin_level == GPIO_LEVEL_HIGH_Z) {
			ALWAYS_TRACE_byte(pin_level, " - IS HIGH-Z");

		} else {
			ALWAYS_TRACE_byte(pin_level, " - unknown level");
		}
	}
	#endif
}
