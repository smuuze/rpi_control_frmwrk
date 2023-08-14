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

#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

#include "driver/keypad/keypad_driver.h"
#include "modules/keypad/keypad_interface.h"

// --------------------------------------------------------------------------------

#ifndef KEYPAD_CONTROLLER_3X4_SCHEDULE_INTERVAL_MS
#define KEYPAD_CONTROLLER_3X4_SCHEDULE_INTERVAL_MS                              50
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef enum {
    KEYPAD_3X4_TASK_STATE_IDLE,
    KEYPAD_3X4_TASK_STATE_GET_KEYS
} KEYPAD_3X4_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief A key is either pressed or released
 * The transition from released to down is send as pressed event.
 * The transition from down to released is send as released event.
 */
typedef enum {
    KEY_STATE_RELEASED = 0x00,
    KEY_STATE_DOWN = 0x01,
} KEYPAD_CONTROLLER_KEY_STATE;

/**
 * @brief 
 */
typedef struct{
    KEYPAD_CONTROLLER_KEY_STATE key_0;
    KEYPAD_CONTROLLER_KEY_STATE key_1;
    KEYPAD_CONTROLLER_KEY_STATE key_2;
    KEYPAD_CONTROLLER_KEY_STATE key_3;
    KEYPAD_CONTROLLER_KEY_STATE key_4;
    KEYPAD_CONTROLLER_KEY_STATE key_5;
    KEYPAD_CONTROLLER_KEY_STATE key_6;
    KEYPAD_CONTROLLER_KEY_STATE key_7;
    KEYPAD_CONTROLLER_KEY_STATE key_8;
    KEYPAD_CONTROLLER_KEY_STATE key_9;
    KEYPAD_CONTROLLER_KEY_STATE key_raute;
    KEYPAD_CONTROLLER_KEY_STATE key_star;
} KEYPAD_CONTROLLER_KEY_LIST;

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_0_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_1_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_2_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_3_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_4_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_5_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_6_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_7_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_8_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_9_PRESSED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_1_PRESSED) // Key STAR
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_2_PRESSED) // Key RAUTE

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_0_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_1_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_2_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_3_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_4_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_5_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_6_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_7_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_8_DOWN)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_9_DOWN)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_1_DOWN) // Key STAR
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_2_DOWN) // Key RAUTE

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_0_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_1_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_2_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_3_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_4_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_5_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_6_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_7_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_8_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_9_RELEASED)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_1_RELEASED) // Key STAR
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(KEY_SPECIAL_2_RELEASED) // Key RAUTE

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
 * @brief Checks if there is currently a key pressed on the keypad.
 * Also checks if there is a key is down. This means it was pressed
 * in a previous cycle.
 * 
 * @param p_keypad_driver_list status of all available key of the keypad
 * @return  1 a key is pressed or a key has its state set to down.
 *          Otherwise 0 is returned.
 */
static u8 keypad_controller_is_key_pressed (
    KEYPAD_KEYS* p_keypad_driver_list
);

/**
 * @brief Sends the keypressed signal of every key taht is currently pressed
 * 
 * @param p_keypad_controller_list list of the current state of the keys
 *        managed by the keypad controller
 * @param p_keypad_driver_list list of keys that are currently pressed
 *        on the hardware
 */
static void keypad_controller_send_signals (
    KEYPAD_CONTROLLER_KEY_LIST* p_keypad_controller_list,
    KEYPAD_KEYS* p_keypad_driver_list
);

// --------------------------------------------------------------------------------

/**
 * @brief current state of the keypad task.
 */
static KEYPAD_3X4_TASK_STATE_TYPE keypad_task_state = KEYPAD_3X4_TASK_STATE_IDLE;

/**
 * @brief Stores the state of each key that is avaialble on the keypad
 */
static KEYPAD_CONTROLLER_KEY_LIST keypad_controller_key_list;

/**
 * @brief Key-state read from the keypad driver
 */
static KEYPAD_KEYS keypad_driver_key_list;

// --------------------------------------------------------------------------------

/**
 * @see keypad_interface.h#keypad_init
 */
void keypad_init(void) {

    keypad_driver_init();

    KEY_0_PRESSED_init();
    KEY_1_PRESSED_init();
    KEY_2_PRESSED_init();
    KEY_3_PRESSED_init();
    KEY_4_PRESSED_init();
    KEY_5_PRESSED_init();
    KEY_6_PRESSED_init();
    KEY_7_PRESSED_init();
    KEY_8_PRESSED_init();
    KEY_9_PRESSED_init();
    KEY_SPECIAL_1_PRESSED_init();
    KEY_SPECIAL_2_PRESSED_init();

    KEY_0_DOWN_init();
    KEY_1_DOWN_init();
    KEY_2_DOWN_init();
    KEY_3_DOWN_init();
    KEY_4_DOWN_init();
    KEY_5_DOWN_init();
    KEY_6_DOWN_init();
    KEY_7_DOWN_init();
    KEY_8_DOWN_init();
    KEY_9_DOWN_init();
    KEY_SPECIAL_1_DOWN_init();
    KEY_SPECIAL_2_DOWN_init();

    KEY_0_RELEASED_init();
    KEY_1_RELEASED_init();
    KEY_2_RELEASED_init();
    KEY_3_RELEASED_init();
    KEY_4_RELEASED_init();
    KEY_5_RELEASED_init();
    KEY_6_RELEASED_init();
    KEY_7_RELEASED_init();
    KEY_8_RELEASED_init();
    KEY_9_RELEASED_init();
    KEY_SPECIAL_1_RELEASED_init();
    KEY_SPECIAL_2_RELEASED_init();

    KEYPAD_CONTROL_TASK_init();
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
static void keypad_task_init(void) {
    DEBUG_PASS("keypad_task_init()");
    keypad_task_state = KEYPAD_3X4_TASK_STATE_IDLE;
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

        default:
            keypad_task_state = KEYPAD_3X4_TASK_STATE_IDLE;
            // no break;
            /* fall through */

        case KEYPAD_3X4_TASK_STATE_IDLE:

            if (keypad_controller_is_key_pressed(&keypad_driver_key_list) != 0) {
                DEBUG_PASS("keypad_task_run() - STATE_IDLE -> STATE_GET_KEYS");
                keypad_task_state = KEYPAD_3X4_TASK_STATE_GET_KEYS;
            }

            break;

        case KEYPAD_3X4_TASK_STATE_GET_KEYS:

            keypad_driver_get_keys(&keypad_driver_key_list);
            keypad_controller_send_signals(
                &keypad_controller_key_list,
                &keypad_driver_key_list
            );

            DEBUG_PASS("keypad_task_run() - STATE_GET_KEYS -> STATE_IDLE");
            keypad_task_state = KEYPAD_3X4_TASK_STATE_IDLE;

            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void keypad_task_terminate(void) {
    
}

// --------------------------------------------------------------------------------

/**
 * @see keypad_controller_3x4.c#keypad_controller_is_key_pressed
 */
static u8 keypad_controller_is_key_pressed (
    KEYPAD_KEYS* p_keypad_driver_list
) {

    if (p_keypad_driver_list->key_0) { return 1; }
    if (p_keypad_driver_list->key_1) { return 1; }
    if (p_keypad_driver_list->key_2) { return 1; }
    if (p_keypad_driver_list->key_3) { return 1; }
    if (p_keypad_driver_list->key_4) { return 1; }
    if (p_keypad_driver_list->key_5) { return 1; }
    if (p_keypad_driver_list->key_6) { return 1; }
    if (p_keypad_driver_list->key_7) { return 1; }
    if (p_keypad_driver_list->key_8) { return 1; }
    if (p_keypad_driver_list->key_9) { return 1; }
    if (p_keypad_driver_list->key_raute) { return 1; }
    if (p_keypad_driver_list->key_star) { return 1; }

    if (keypad_driver_is_key_pressed()) {
        DEBUG_PASS("keypad_controller_is_key_pressed() - HW keys pressed");
        return 1;
    }

    return 0;
}

/**
 * @see keypad_controller_3x4.c#keypad_controller_send_signals
 */
static void keypad_controller_send_signals(
    KEYPAD_CONTROLLER_KEY_LIST* p_keypad_controller_list,
    KEYPAD_KEYS* p_keypad_driver_list
) {

    if (p_keypad_controller_list->key_0 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_0) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_0 PRESSED");
            p_keypad_controller_list->key_0 = KEY_STATE_DOWN;
            KEY_0_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_0) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_0 DOWN");
        KEY_0_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_0 RELEASED");
        p_keypad_controller_list->key_0 = KEY_STATE_RELEASED;
        KEY_0_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_1 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_1) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_1 PRESSED");
            p_keypad_controller_list->key_1 = KEY_STATE_DOWN;
            KEY_1_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_1) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_1 DOWN");
        KEY_1_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_1 RELEASED");
        p_keypad_controller_list->key_1 = KEY_STATE_RELEASED;
        KEY_1_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_2 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_2) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_2 PRESSED");
            p_keypad_controller_list->key_2 = KEY_STATE_DOWN;
            KEY_2_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_2) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_2 DOWN");
        KEY_2_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_2 RELEASED");
        p_keypad_controller_list->key_2 = KEY_STATE_RELEASED;
        KEY_2_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_3 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_3) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_3 PRESSED");
            p_keypad_controller_list->key_3 = KEY_STATE_DOWN;
            KEY_3_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_3) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_3 DOWN");
        KEY_3_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_3 RELEASED");
        p_keypad_controller_list->key_3 = KEY_STATE_RELEASED;
        KEY_3_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_4 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_4) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_4 PRESSED");
            p_keypad_controller_list->key_4 = KEY_STATE_DOWN;
            KEY_4_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_4) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_4 DOWN");
        KEY_4_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_4 RELEASED");
        p_keypad_controller_list->key_4 = KEY_STATE_RELEASED;
        KEY_4_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_5 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_5) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_5 PRESSED");
            p_keypad_controller_list->key_5 = KEY_STATE_DOWN;
            KEY_5_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_5) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_5 DOWN");
        KEY_5_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_5 RELEASED");
        p_keypad_controller_list->key_5 = KEY_STATE_RELEASED;
        KEY_5_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_6 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_6) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_6 PRESSED");
            p_keypad_controller_list->key_6 = KEY_STATE_DOWN;
            KEY_6_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_6) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_6 DOWN");
        KEY_6_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_6 RELEASED");
        p_keypad_controller_list->key_6 = KEY_STATE_RELEASED;
        KEY_6_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_7 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_7) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_7 PRESSED");
            p_keypad_controller_list->key_7 = KEY_STATE_DOWN;
            KEY_7_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_7) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_7 DOWN");
        KEY_7_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_7 RELEASED");
        p_keypad_controller_list->key_7 = KEY_STATE_RELEASED;
        KEY_7_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_8 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_8) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_8 PRESSED");
            p_keypad_controller_list->key_8 = KEY_STATE_DOWN;
            KEY_8_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_8) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_8 DOWN");
        KEY_8_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_8 RELEASED");
        p_keypad_controller_list->key_8 = KEY_STATE_RELEASED;
        KEY_8_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_9 == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_9) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_9 PRESSED");
            p_keypad_controller_list->key_9 = KEY_STATE_DOWN;
            KEY_9_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_9) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_9 DOWN");
        KEY_9_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_9 RELEASED");
        p_keypad_controller_list->key_9 = KEY_STATE_RELEASED;
        KEY_9_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_star == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_star) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_1 PRESSED");
            p_keypad_controller_list->key_star = KEY_STATE_DOWN;
            KEY_SPECIAL_1_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_star) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_1 DOWN");
        KEY_SPECIAL_1_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_1 RELEASED");
        p_keypad_controller_list->key_star = KEY_STATE_RELEASED;
        KEY_SPECIAL_1_RELEASED_send(NULL);
    }

    if (p_keypad_controller_list->key_raute == KEY_STATE_RELEASED) {
        if (p_keypad_driver_list->key_raute) {
            DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_2 PRESSED");
            p_keypad_controller_list->key_raute = KEY_STATE_DOWN;
            KEY_SPECIAL_2_PRESSED_send(NULL);
        }
    } else if (p_keypad_driver_list->key_raute) {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_2 DOWN");
        KEY_SPECIAL_2_DOWN_send(NULL);
    } else {
        DEBUG_PASS("keypad_controller_send_signals() - KEY_SPECIAL_2 RELEASED");
        p_keypad_controller_list->key_raute = KEY_STATE_RELEASED;
        KEY_SPECIAL_2_RELEASED_send(NULL);
    }
}

// --------------------------------------------------------------------------------
