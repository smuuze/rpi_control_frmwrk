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
 * @file    io_input_controller.h
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 03
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_io_input_controller_
#define _H_io_input_controller_

// --------------------------------------------------------------------------------

#include "common/iterator_interface.h"
#include "system/system_interface.h"

// --------------------------------------------------------------------------------

/*!
 * Timespan that the button will be in pressed state, until the down state is activated
 */
#define IO_BUTTON_PRESSED_TIMESPAN_MS                                       30000

/*!
 * Timespan the button will be in released state afte it was down and has been released
 */
#define IO_BUTTON_RELEASED_TIMESPAN_MS                                      30000

// --------------------------------------------------------------------------------

/*!
 * Function-Call-refrence to the logic that will produce the actual hw-state of that particular button
 * @return     1 if button is pressed
 *         0 if button is not pressed
 */
typedef u8 (*IO_BUTTON_WATCHER_PIN_STATE_CALLBACK)     (void);

/*!
 *
 */
typedef struct IO_INPUT_DESCRIPTOR_T {

    u8 id;

    GPIO_DRIVER_PIN_DESCRIPTOR* pin_descriptor;
    GPIO_DRIVER_LEVEL actual_pin_state;

    u8 down;
    u8 pressed;
    u8 released;

    u32 __down_time;
    u16 __press_time;
    u16 __release_time;

    struct IO_INPUT_DESCRIPTOR_T* __next_button;

} IO_INPUT_DESCRIPTOR;

/*!
 *
 */
typedef struct BUTTON_STATE {

    u8 id;
    u8 down;
    u8 pressed;
    u8 released;
    u32 down_time;

} IO_INPUT_STATE;

// --------------------------------------------------------------------------------

/*!
 *
 */
void io_input_controller_init(void);

// --------------------------------------------------------------------------------

/*!
 *
 * @param p_new_button
 * @return
 */
u8 io_input_controller_register_input(IO_INPUT_DESCRIPTOR* p_new_button);

/**
 * @brief 
 * 
 * @param p_button_state 
 */
void io_input_controller_check_state(IO_INPUT_DESCRIPTOR* p_button_state);

/*!
 *
 * @param button_id
 * @param p_button_state
 */
void io_input_controller_get_state(u8 button_id, IO_INPUT_STATE* p_button_state);

// --------------------------------------------------------------------------------

/*!
 *
 * @param p_iterator
 */
void io_input_controller_get_iterator(ITERATOR_INTERFACE* p_iterator);

/*!
 *
 * @param p_iterator
 * @param p_button_state
 */
void io_input_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_INPUT_STATE* p_button_state);

/*!
 *
 * @param p_iterator
 */
void io_input_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator);

// --------------------------------------------------------------------------------

#endif // _H_io_input_controller_

// --------------------------------------------------------------------------------
