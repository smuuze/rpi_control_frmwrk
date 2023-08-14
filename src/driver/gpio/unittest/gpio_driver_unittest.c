/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	driver/gpio/arm/gpio_driver_unittest.c
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

#include "system/system_interface.h"
#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

void gpio_driver_init(void) {

	DEBUG_PASS("gpio_driver_init()");
}

void gpio_driver_deinit(void) {
    
}

void gpio_driver_init_pin(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	(void) p_pin_descr;
	DEBUG_PASS("gpio_driver_init_pin()");
}

void gpio_driver_set_direction(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {
	(void) p_pin_descr;
	DEBUG_PASS("gpio_driver_init_pin()");
}

void gpio_driver_set_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {
	(void) p_pin_descr;
	DEBUG_PASS("gpio_driver_init_pin()");
}

void gpio_driver_toggle_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	(void) p_pin_descr;
	DEBUG_PASS("gpio_driver_init_pin()");
}

GPIO_DRIVER_LEVEL gpio_driver_get_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	(void) p_pin_descr;
	DEBUG_PASS("gpio_driver_init_pin()");

	return GPIO_LEVEL_LOW;
}

void gpio_driver_print_pin_state(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
	(void) p_pin_descr;
}
