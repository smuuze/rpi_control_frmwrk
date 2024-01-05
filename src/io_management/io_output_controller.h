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
 * @file    io_output_controller.h
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 03
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_io_output_controller_
#define _H_io_output_controller_

// --------------------------------------------------------------------------------

#include "common/iterator_interface.h"

// --------------------------------------------------------------------------------

#define IO_OUTPUT_STATE_DISABLED                                            0xFF
#define IO_OUTPUT_STATE_ON                                                  0x01
#define IO_OUTPUT_STATE_OFF                                                 0x00

#define IO_OUTPUT_ID_INVALID                                                0xFF

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef void (*IO_OUTPUT_SET_PIN)     (u8 is_on);

/**
 * @brief 
 * 
 */
typedef enum {
    IO_TYPE_UNDEFINED,
    IO_TYPE_SYSTEM,
    IO_TYPE_USER
} IO_TYPE;

/**
 * @brief 
 * 
 */
typedef struct IO_OUTPUT_DESCRIPTOR {

    u8 id;

    GPIO_DRIVER_PIN_DESCRIPTOR* pin_descriptor;
    IO_TYPE type;

    u8 actual_pin_state;
    u8 next_pin_state;

    u32 reference_time;
    u32 duration;
    u32 toggle_period;

    IO_OUTPUT_SET_PIN set_pin;

    struct IO_OUTPUT_DESCRIPTOR* _next;

} IO_OUTPUT_DESCRIPTOR;

/**
 * @brief 
 * 
 */
typedef struct IO_OUTPUT_STATE {

    u8 id;
    u8 pin_state;
    u32 toggle_period;
    u32 duration;
    u32 on_time;

} IO_OUTPUT_STATE;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void io_output_controller_init(void);

/**
 * @brief 
 * 
 * @param p_new_output 
 * @return u8 
 */
u8 io_output_controller_register_output(IO_OUTPUT_DESCRIPTOR* p_new_output);

/**
 * @brief 
 * 
 * @param id 
 * @param state 
 * @param duration 
 * @param toggle_peridod 
 * @return u8 
 */
u8 io_output_controller_set_output(u8 id, u8 state, u32 duration, u32 toggle_peridod);

/**
 * @brief 
 * 
 * @param id 
 * @param p_state 
 */
void io_output_controller_get_output(u8 id, IO_OUTPUT_STATE* p_state);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_iterator 
 * @param type_filter 
 */
void io_output_controller_get_iterator(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter);

/**
 * @brief 
 * 
 * @param p_iterator 
 * @param p_output_state 
 */
void io_output_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_OUTPUT_STATE* p_output_state);

/**
 * @brief 
 * 
 * @param p_iterator 
 * @param type_filter 
 */
void io_output_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter);

// --------------------------------------------------------------------------------

#endif // _H_io_output_controller_

// --------------------------------------------------------------------------------
