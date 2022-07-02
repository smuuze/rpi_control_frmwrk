/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    gpio_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 27
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_gpio_interface_
#define _H_gpio_interface_

// --------------------------------------------------------------------------------

#include "system/system_interface.h"
#include "platine/board_common_config.h"

// --------------------------------------------------------------------------------

#define GPIO_GET_PIN_NUMBER(p_descr)                (10)
#define GPIO_DRIVER_PIN_IS_DEACTIVATED(p_descr)     ((p_pin_descr->pin_cfg & GPIO_DEACTIVATE) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_OUTPUT(p_pin_descr)          ((p_pin_descr->pin_cfg & GPIO_OUTPUT) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr)        ((p_pin_descr->pin_cfg & GPIO_IDLE_LOW) != 0 ? 1 : 0)
#define GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr)       ((p_pin_descr->pin_cfg & GPIO_IDLE_HIGH) != 0 ? 1 : 0)
#define GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr)    ((p_pin_descr->pin_cfg & GPIO_INVERTED) != 0 ? 1 : 0)

// --------------------------------------------------------------------------------

#endif // _H_gpio_interface_

// --------------------------------------------------------------------------------
