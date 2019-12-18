#ifndef _LOCAL_GPIO_DRIVER_H_
#define _LOCAL_GPIO_DRIVER_H_

#include "system/system_interface.h"

/*!
 *
 */
void gpio_driver_init(void);

/*!
 *
 * @param p_pin_descr
 */
void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/*!
 *
 * @param pin_num
 * @param direction
 */
void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction);

/*!
 *
 * @param pin_num
 * @param level
 */
void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level);

/*!
 *
 * @param p_pin_descr
 */
void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/*!
 *
 * @param pin_num
 * @return
 */
GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/*!
 *
 * @param p_pin_descr
 */
void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

#endif
