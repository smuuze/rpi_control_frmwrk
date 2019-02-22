/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "system_interface.h"
#include "local_gpio_driver.h"

#define noTRACES
#include <traces.h>

/*
 * How to set Port-Pins on a ATMega 128 4P
 *
 * DDR
 */

#define SET_PIN_AS_INPUT(port, pin)					DDR##port &= (unsigned char)~(pin)
#define SET_PIN_AS_OUTPUT(port, pin)					DDR##port |= (pin)

#define GPIO_DRIVER_IS_OUTPUT(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_OUTPUT) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_IDLE_LOW) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr)				((p_pin_descr->pin_cfg & GPIO_IDLE_HIGH) != 0 ? 1 : 0)

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
#define GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr)			((p_pin_descr->pin_cfg & GPIO_INVERTED) != 0 ? 1 : 0)
#define GPIO_DRIVER_INVERT_LEVEL(level)					level = (level == GPIO_LEVEL_HIGH) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH

#define GPIO_DRIVER_GET_PORT(p_pin_descr)				(p_pin_descr->port_id)
#define GPIO_DRIVER_GET_PIN(p_pin_descr)				(p_pin_descr->pin_id)


void gpio_driver_init(void) {
	PASS(); // gpio_driver_init()

	DDRA = 0x00; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0x00;
	DDRD = 0x00; PORTD = 0x00;

}

void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

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

	SYSTEM_INTERFACE_GPIO_LEVEL level = GPIO_LEVEL_HIGH_Z;

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

void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_DIRECTION direction) {

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

void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_LEVEL level) {

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

	u8 port_id = GPIO_DRIVER_GET_PORT(p_pin_descr);
	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	TRACE_byte(port_id);  // gpio_driver_toggle_level() - PORT
	TRACE_byte(pin_num);  // gpio_driver_toggle_level() - PIN

	switch (port_id) {
		case GPIO_PORT_A: GPIO_DRIVER_TOGGLE_PIN_LEVEL(A, pin_num); break;
		case GPIO_PORT_B: GPIO_DRIVER_TOGGLE_PIN_LEVEL(B, pin_num); break;
		case GPIO_PORT_C: GPIO_DRIVER_TOGGLE_PIN_LEVEL(C, pin_num); break;
		case GPIO_PORT_D: GPIO_DRIVER_TOGGLE_PIN_LEVEL(D, pin_num); break;
		default: break;
	}
}

SYSTEM_INTERFACE_GPIO_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

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
