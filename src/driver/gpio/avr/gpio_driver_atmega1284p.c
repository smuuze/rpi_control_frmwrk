/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system/system_interface.h"
#include "driver/gpio/gpio_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//---------- ------------------------------------------------------------------

#define SET_PIN_AS_INPUT(port, pin)					DDR##port &= (unsigned char)~(pin)
#define SET_PIN_AS_OUTPUT(port, pin)					DDR##port |= (pin)

#define GPIO_DRIVER_IS_OUTPUT(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_OUTPUT) != 0 ? 1 : 0)
#define GPIO_DRIVER_PIN_IS_DEACTIVATED(p_descr)				((p_pin_descr->pin_cfg & GPIO_DEACTIVATE) != 0 ? 1 : 0)

#define GPIO_DRIVER_SET_PIN_DIRECTION(port, pin, direction)		if (direction == GPIO_DIRECTION_INPUT) {		\
										SET_PIN_AS_INPUT(port, pin);			\
                                                                    	} else {						\
                                                                        	SET_PIN_AS_OUTPUT(port, pin);			\
                                                                    	}

#define SET_PIN_HIGH_LEVEL(port, pin)                           	PORT##port |= (pin)
#define SET_PIN_LOW_LEVEL(port, pin)                            	PORT##port &= (unsigned char)~(pin)
#define SET_PIN_HIGH_Z(port, pin)                      			PORT##port &= (unsigned char)~(pin); DDR##port &= (unsigned char)~(pin)

#define GPIO_DRIVER_SET_PIN_LEVEL(port, pin, level)    			if (level == GPIO_LEVEL_HIGH) {				\
                                                                        	SET_PIN_HIGH_LEVEL(port, pin);			\
                                                                    	} else if (level == GPIO_LEVEL_LOW) {			\
                                                                        	SET_PIN_LOW_LEVEL(port, pin);			\
                                                                    	} else {						\
                                                                        	SET_PIN_HIGH_Z(port, pin);			\
                                                                    	}

#define GPIO_DRIVER_TOGGLE_PIN_LEVEL(port, pin)				PORT##port ^= (pin)

#define GPIO_DRIVER_GET_PIN_LEVEL(port, pin)				(PIN##port & pin) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW

#define GPIO_DRIVER_GET_PORT(p_pin_descr)				(p_pin_descr->port_id)
#define GPIO_DRIVER_GET_PIN(p_pin_descr)				(p_pin_descr->pin_id)


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

void gpio_driver_init(void) {

	PASS(); // gpio_driver_init()
	
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

// --------------------------------------------------------------------------------

void gpio_driver_deinit(void) {
    
}

void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	PASS(); // gpio_driver_init_pin()
	TRACE_byte(GPIO_DRIVER_GET_PORT(p_pin_descr));  // gpio_driver_get_level() - PORT
	TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr));  // gpio_driver_get_level() - PIN

	//is input or output
	if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) != 0) {
		PASS(); // IS_OUTPUT
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_OUTPUT);
	} else {
		PASS(); // IS_INPUT
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_INPUT);
	}

	GPIO_DRIVER_LEVEL level = GPIO_LEVEL_HIGH_Z;

	if (GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr) != 0) {
		PASS(); // IS ILDE LOW
		level = GPIO_LEVEL_LOW;

	} else if (GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr) != 0) {
		PASS(); // IS IDLE HIGH
		level = GPIO_LEVEL_HIGH;
	} else {
		PASS(); // IS IDLE HIGH-Z
	}

	gpio_driver_set_level(p_pin_descr, level);
}

void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	TRACE_byte(port_id);  // gpio_driver_get_level() - PORT
	TRACE_byte(pin_num);  // gpio_driver_get_level() - PIN
	TRACE_byte(direction);  // gpio_driver_get_level() - DIRECTION

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_SET_PIN_DIRECTION(A, pin_num, direction) break;
		case GPIO_PORT_B: GPIO_DRIVER_SET_PIN_DIRECTION(B, pin_num, direction) break;
		case GPIO_PORT_C: GPIO_DRIVER_SET_PIN_DIRECTION(C, pin_num, direction) break;
		case GPIO_PORT_D: GPIO_DRIVER_SET_PIN_DIRECTION(D, pin_num, direction) break;
		default: break;
	}
}

void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	TRACE_byte(port_id);  // gpio_driver_set_level() - PORT
	TRACE_byte(pin_num);  // gpio_driver_set_level() - PIN
	TRACE_byte(level);  // gpio_driver_set_level() - LEVEL

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {

		GPIO_DRIVER_INVERT_LEVEL(level);
		TRACE_byte(level);  // gpio_driver_get_level() - LEVEL inverted
	}

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_SET_PIN_LEVEL(A, pin_num, level) break;
		case GPIO_PORT_B: GPIO_DRIVER_SET_PIN_LEVEL(B, pin_num, level) break;
		case GPIO_PORT_C: GPIO_DRIVER_SET_PIN_LEVEL(C, pin_num, level) break;
		case GPIO_PORT_D: GPIO_DRIVER_SET_PIN_LEVEL(D, pin_num, level) break;
		default: break;
	}
}

void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	DEBUG_TRACE_byte(port_id, "gpio_driver_toggle_level() - PORT");
	DEBUG_TRACE_byte(pin_num, "gpio_driver_toggle_level() - PIN");
	//DEBUG_TRACE_byte(gpio_driver_get_level(p_pin_descr), "gpio_driver_toggle_level() - LEVEL before");

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_TOGGLE_PIN_LEVEL(A, pin_num); break;
		case GPIO_PORT_B: GPIO_DRIVER_TOGGLE_PIN_LEVEL(B, pin_num); break;
		case GPIO_PORT_C: GPIO_DRIVER_TOGGLE_PIN_LEVEL(C, pin_num); break;
		case GPIO_PORT_D: GPIO_DRIVER_TOGGLE_PIN_LEVEL(D, pin_num); break;
		default: break;
	}

	//DEBUG_TRACE_byte(gpio_driver_get_level(p_pin_descr), "gpio_driver_toggle_level() - LEVEL after");
}

GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	
	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return GPIO_LEVEL_HIGH_Z;
	}

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	u8 pin_level = GPIO_LEVEL_LOW;

	switch (port_id) {
		case GPIO_PORT_A: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(A, pin_num); break;
		case GPIO_PORT_B: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(B, pin_num); break;
		case GPIO_PORT_C: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(C, pin_num); break;
		case GPIO_PORT_D: pin_level = GPIO_DRIVER_GET_PIN_LEVEL(D, pin_num); break;
		default: break;
	}

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
		GPIO_DRIVER_INVERT_LEVEL(pin_level);
	}

	TRACE_byte(port_id);  // gpio_driver_get_level() - PORT
	TRACE_byte(pin_num);  // gpio_driver_get_level() - PIN
	TRACE_byte(pin_level);  // gpio_driver_get_level() - LEVEL

	return pin_level;
}

void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	p_pin_descr->pin_cfg |= GPIO_DEACTIVATE;
}

void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	p_pin_descr->pin_cfg &= ~GPIO_DEACTIVATE;
	gpio_driver_init_pin(p_pin_descr);

}
