/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	driver/gpio/arm/gpio_driver_wiringpi.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

#include <wiringPi.h>

// --------------------------------------------------------------------------------

#define GPIO_READ_PIN(pin_num)				digitalRead(pin_num)
#define GPIO_WRITE_PIN(pin_num, state)			digitalWrite(pin_num, state)
#define GPIO_INITIALIZE()				wiringPiSetup()
#define GPIO_CONFIGURE_PIN(pin_num, direction)		pinMode(pin_num, (direction == GPIO_DIRECTION_OUTPUT) ? OUTPUT : INPUT )
#define GPIO_PULL_UP_DOWN(pin_num, pull_down)		pullUpDnControl(pin_num, (pull_down == 0) ? PUD_DOWN : (pull_down == 1) ? PUD_UP : PUD_OFF )

#define GPIO_GET_PIN_NUMBER(p_descr)			(10)
#define GPIO_DRIVER_PIN_IS_DEACTIVATED(p_descr)		((p_pin_descr->pin_cfg & GPIO_DEACTIVATE) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_OUTPUT(p_pin_descr)		((p_pin_descr->pin_cfg & GPIO_OUTPUT) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr)		((p_pin_descr->pin_cfg & GPIO_IDLE_LOW) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr)		((p_pin_descr->pin_cfg & GPIO_IDLE_HIGH) != 0 ? 1 : 0)
#define GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr)	((p_pin_descr->pin_cfg & GPIO_INVERTED) != 0 ? 1 : 0)

#define GPIO_DRIVER_SET_PIN_LEVEL(pin, level)    	switch (level) {								\
								case GPIO_LEVEL_HIGH   : GPIO_WRITE_PIN(pin, 1); break;			\
								case GPIO_LEVEL_LOW    : GPIO_WRITE_PIN(pin, 0); break;			\
								default:								\
								case GPIO_LEVEL_HIGH_Z : GPIO_PULL_UP_DOWN(pin, 1); break;	\
							}

#define GPIO_DRIVER_GET_PIN(p_pin_descr)		gpio_driver_get_pin_id(p_pin_descr)

// --------------------------------------------------------------------------------

INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_2) // I2C_SDA
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_4) // I2C_SCL
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_6) // GPIO04
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_7) // UART_TXD

INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_1) // UART_RXD
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_2) // GPIO17
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_3) // GPIO18
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_4) // GPIO27
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_6) // GPIO22
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_7) // GPIO23

INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_1) // GPIO24
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_2) // SPI_MOSI
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_4) // SPI_MISO
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_5) // GPIO25
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_6) // SPI_SCLK
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_7) // SPI_CE0

INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_1) // SPI_CE1
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_4) // GPIO05
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_6) // GPIO06
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_7) // GPIO12

INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_0) // GPIO13
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_2) // GPIO19
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_3) // GPIO16
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_4) // GPIO26
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_5) // GPIO20
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_7) // GPIO21

// --------------------------------------------------------------------------------

static u8 gpio_driver_get_pin_id(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	if (p_pin_descr->port_id == GPIO_PORT_A) {

		if (p_pin_descr->pin_id == GPIO_PIN_6) { return 7; }

	} else if (p_pin_descr->port_id == GPIO_PORT_B) {

		if (p_pin_descr->pin_id == GPIO_PIN_2) { return 0; }
		if (p_pin_descr->pin_id == GPIO_PIN_3) { return 1; }
		if (p_pin_descr->pin_id == GPIO_PIN_4) { return 2; }
		if (p_pin_descr->pin_id == GPIO_PIN_6) { return 3; }
		if (p_pin_descr->pin_id == GPIO_PIN_7) { return 4; }

	} else if (p_pin_descr->port_id == GPIO_PORT_C) {

		if (p_pin_descr->pin_id == GPIO_PIN_1) { return 5; }
		if (p_pin_descr->pin_id == GPIO_PIN_5) { return 6; }
		if (p_pin_descr->pin_id == GPIO_PIN_7) { return 10; }

	} else if (p_pin_descr->port_id == GPIO_PORT_D) {

		if (p_pin_descr->pin_id == GPIO_PIN_4) { return 21; }
		if (p_pin_descr->pin_id == GPIO_PIN_6) { return 22; }
		if (p_pin_descr->pin_id == GPIO_PIN_7) { return 26; }

	} else if (p_pin_descr->port_id == GPIO_PORT_E) {

		if (p_pin_descr->pin_id == GPIO_PIN_0) { return 23; }
		if (p_pin_descr->pin_id == GPIO_PIN_2) { return 24; }
		if (p_pin_descr->pin_id == GPIO_PIN_3) { return 27; }
		if (p_pin_descr->pin_id == GPIO_PIN_4) { return 25; }
		if (p_pin_descr->pin_id == GPIO_PIN_5) { return 28; }
		if (p_pin_descr->pin_id == GPIO_PIN_7) { return 29; }
	}

	return 0xFF;
}

// --------------------------------------------------------------------------------

void gpio_driver_init(void) {

	DEBUG_PASS("gpio_driver_init_pin() - Going to initialize gpio-subset");

	int err_code = GPIO_INITIALIZE();
	if (err_code < 0) {
		DEBUG_TRACE_long(err_code, "- Initializing GPIOD has FAILED !!! --- (err-code)");
		return;
	}

	DEBUG_PASS("gpio_driver_init_pin() - Initializing gpio-subset has succeeded!");
	
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_2)); // I2C_SDA
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_4)); // I2C_SCL
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_6)); // GPIO04
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_7)); // UART_TXD
	
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_1)); // UART_RXD
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_2)); // GPIO17
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_3)); // GPIO18
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_4)); // GPIO27
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_6)); // GPIO22
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_7)); // GPIO23
	
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_1)); // GPIO24
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_2)); // SPI_MOSI
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_4)); // SPI_MISO
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_5)); // GPIO25
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_6)); // SPI_SCLK
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_7)); // SPI_CE0
	
	//gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_1)); // SPI_CE1
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_4)); // GPIO05
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_6)); // GPIO06
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_7)); // GPIO12

	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_0)); // GPIO13
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_2)); // GPIO19
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_3)); // GPIO16
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_4)); // GPIO26
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_5)); // GPIO20
	gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_7)); // GPIO20
}

// --------------------------------------------------------------------------------

void gpio_driver_deinit(void) {
    
}

void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

	#ifdef TRACER_ON
	{
		u8 pin_number = GPIO_GET_PIN_NUMBER(p_pin_descr);
		DEBUG_TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr),  "gpio_driver_init_pin() - Pin-Number:");
	}
	#endif

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {

		DEBUG_PASS("gpio_driver_init_pin() - ignored");
		return;

	} else if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) != 0) {

		DEBUG_PASS("gpio_driver_init_pin() - is output");
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_OUTPUT);

	} else {

		DEBUG_PASS("gpio_driver_init_pin() - is input");
		gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_INPUT);
	}

	GPIO_DRIVER_LEVEL level = GPIO_LEVEL_HIGH_Z;

	if (GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr) != 0) {

		DEBUG_PASS("gpio_driver_init_pin() - is idle low");
		level = GPIO_LEVEL_LOW;

	} else if (GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr) != 0) {

		DEBUG_PASS("gpio_driver_init_pin() - is idle high");
		level = GPIO_LEVEL_HIGH;

	} else {
		DEBUG_PASS("gpio_driver_init_pin() - is idle high-z");
	}

	gpio_driver_set_level(p_pin_descr, level);
}

void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	if (direction == GPIO_DIRECTION_INPUT) {
		DEBUG_TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr), "gpio_driver_set_direction() - new direction: INPUT - PIN:");
	} else {
		DEBUG_TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr), "gpio_driver_set_direction() - new direction: OUTPUT - PIN:");
	}

	GPIO_CONFIGURE_PIN(GPIO_DRIVER_GET_PIN(p_pin_descr), direction);
}

void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {

	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	u8 pin_num = GPIO_DRIVER_GET_PIN(p_pin_descr);

	if (level == GPIO_LEVEL_HIGH) {
		DEBUG_TRACE_byte(pin_num,   "gpio_driver_set_level() - new level: HIGH - PIN:");
	} else if (level == GPIO_LEVEL_LOW) {
		DEBUG_TRACE_byte(pin_num,   "gpio_driver_set_level() - new level: LOW - PIN:");
	} else {
		DEBUG_TRACE_byte(pin_num,   "gpio_driver_set_level() - new level: HIGH-Z - PIN:");
	}

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {

		GPIO_DRIVER_INVERT_LEVEL(level);
		DEBUG_TRACE_byte(level, "gpio_driver_get_level() - LEVEL inverted");
	}

	GPIO_DRIVER_SET_PIN_LEVEL(pin_num, level);
}

void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	
	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return;
	}

	if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) == 0) {
		DEBUG_TRACE_byte(GPIO_DRIVER_GET_PIN(p_pin_descr), "gpio_driver_toggle_level() - This is not an output");
		return;
	}

	GPIO_DRIVER_LEVEL pin_level = gpio_driver_get_level(p_pin_descr);
	GPIO_DRIVER_INVERT_LEVEL(pin_level);
	gpio_driver_set_level(p_pin_descr, pin_level);
}

GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	
	if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
		return GPIO_LEVEL_HIGH_Z;
	}

	u8 pin_num   = GPIO_DRIVER_GET_PIN(p_pin_descr);
	GPIO_DRIVER_LEVEL pin_level = GPIO_READ_PIN(pin_num) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

	if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
		GPIO_DRIVER_INVERT_LEVEL(pin_level);
	}

	DEBUG_TRACE_byte(pin_num,   "gpio_driver_get_level() - PIN");
	DEBUG_TRACE_byte(pin_level, "gpio_driver_get_level() - LEVEL");

	return pin_level;
}

void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	(void) p_pin_descr;
}

void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	p_pin_descr->pin_cfg |= GPIO_DEACTIVATE;
}

void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	p_pin_descr->pin_cfg &= ~GPIO_DEACTIVATE;
	gpio_driver_init_pin(p_pin_descr);
}
