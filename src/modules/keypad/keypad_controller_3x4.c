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
 * @file    keypad_controller_3x4.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 18
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#include "driver/keypad/keypad_driver.h"
#include "modules/keypad/keypad_interface.h"

// --------------------------------------------------------------------------------

#ifndef KEYPAD_CONTROLLER_3X4_SCHEDULE_INTERVAL_MS
#define KEYPAD_CONTROLLER_3X4_SCHEDULE_INTERVAL_MS                              50
#endif

#ifndef KEYPAD_CONTROLLER_3X4_PAUSE_TIME_MS
#define KEYPAD_CONTROLLER_3X4_PAUSE_TIME_MS                                     100
#endif

// --------------------------------------------------------------------------------

typedef enum {
    KEYPAD_3X4_TASK_STATE_IDLE,
    KEYPAD_3X4_TASK_STATE_GET_KEYS,
    KEYPAD_3X4_TASK_STATE_PAUSE
} KEYPAD_3X$_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_0_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_1_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_2_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_3_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_4_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_5_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_6_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_7_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_8_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_9_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_1_PRESSED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_2_PRESSED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
static void keypad_task_init(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 keypad_task_get_schedule_interval(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE keypad_task_get_state(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void keypad_task_run(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void keypad_task_terminate(void);

TASK_CREATE (
    KEYPAD_CONTROL_TASK,
    TASK_PRIORITY_MIDDLE,
    keypad_task_get_schedule_interval,
    keypad_task_init,
    keypad_task_run,
    keypad_task_get_state,
    keypad_task_terminate
)

// --------------------------------------------------------------------------------

/**
 * @brief Sends the keypressed signal of every key taht is currently pressed
 * 
 * @param p_pressed_keys list of keys that are currently pressed
 */
static void keypad_controller_send_signals(KEYPAD_KEYS* p_pressed_keys);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static keypad_task_state = KEYPAD_3X4_TASK_STATE_IDLE;

/**
 * @brief 
 * 
 */
static KEYPAD_KEYS keypad_controller_pressed_key;

// --------------------------------------------------------------------------------

/**
 * @see keypad_interface.h#keypad_init
 */
void keypad_init(void) {

    KEY_0_PRESSED_SIGNAL_init();
    KEY_1_PRESSED_SIGNAL_init();
    KEY_2_PRESSED_SIGNAL_init();
    KEY_3_PRESSED_SIGNAL_init();
    KEY_4_PRESSED_SIGNAL_init();
    KEY_5_PRESSED_SIGNAL_init();
    KEY_6_PRESSED_SIGNAL_init();
    KEY_7_PRESSED_SIGNAL_init();
    KEY_8_PRESSED_SIGNAL_init();
    KEY_9_PRESSED_SIGNAL_init();
    KEY_SPECIAL_1_PRESSED_SIGNAL_init();
    KEY_SPECIAL_2_PRESSED_SIGNAL_init();

    KEYPAD_CONTROL_TASK_init();
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
static void keypad_task_init(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 keypad_task_get_schedule_interval(void) {
    return KEYPAD_CONTROLLER_3X4_SCHEDULE_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE keypad_task_get_state(void) {
    return MCU_TASK_RUNNING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void keypad_task_run(void) {
    
    switch (keypad_task_state) {
        case KEYPAD_3X4_TASK_STATE_IDLE:

            if (keypad_driver_is_key_pressed() != 0) {
                keypad_task_state = KEYPAD_3X4_TASK_STATE_GET_KEYS;
            }

            break;

        case KEYPAD_3X4_TASK_STATE_GET_KEYS:

            keypad_driver_get_keys(&keypad_controller_pressed_key);
            keypad_controller_send_signals(&keypad_controller_pressed_key);

            break;

        case KEYPAD_3X4_TASK_STATE_PAUSE:
            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void keypad_task_terminate(void) {
    
}

// --------------------------------------------------------------------------------

static void keypad_controller_send_signals(KEYPAD_KEYS* p_pressed_keys) {

    if (p_pressed_keys->key_0) { KEY_0_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_01 { KEY_1_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_2) { KEY_2_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_3) { KEY_3_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_4) { KEY_4_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_5) { KEY_5_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_6) { KEY_6_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_7) { KEY_7_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_8) { KEY_8_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_9) { KEY_9_PRESSED_SIGNAL_send(NULL); }

    if (p_pressed_keys->) { KEY_8_PRESSED_SIGNAL_send(NULL); }
    if (p_pressed_keys->key_9) { KEY_9_PRESSED_SIGNAL_send(NULL); }
}

// --------------------------------------------------------------------------------
