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
 * @file    keypad_to_lcd_task.c
 * @author  Sebastian Lesse
 * @date    2023 / 02 / 12
 * @brief   This task receives the key pressed and released events of the keypad.
 *          The number of a key taht is pressed is shown on the lcd until the
 *          is released.
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

#include "mcu_task_interface.h"
#include "signal_slot_interface.h"
#include "modules/keypad/keypad_interface.h"
#include "modules/lcd/lcd_interface.h"

// --------------------------------------------------------------------------------

#define KEY2LCD_KEY_0_INDEX     0
#define KEY2LCD_KEY_1_INDEX     2
#define KEY2LCD_KEY_2_INDEX     4
#define KEY2LCD_KEY_3_INDEX     6
#define KEY2LCD_KEY_4_INDEX     8
#define KEY2LCD_KEY_5_INDEX     10
#define KEY2LCD_KEY_6_INDEX     12
#define KEY2LCD_KEY_7_INDEX     14
#define KEY2LCD_KEY_8_INDEX     17
#define KEY2LCD_KEY_9_INDEX     19

// --------------------------------------------------------------------------------

/**
 * @brief Text that ist shown on the screen
 * Every key has a static position on the LCD.
 */
static u8 lcd_text[32];

/**
 * @brief This variable is set to 1 if a key-signal is received.
 * It will be set back if the lcd is updated.
 */
static u8 lcd2key_is_active = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Callback to handle the KEY_0_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key0_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_0_INDEX] = '0';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_PRESSED, KEY2LCD_KEY0_PRESSED_SLOT, key2lcd_key0_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_1_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key1_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_1_INDEX] = '1';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_PRESSED, KEY2LCD_KEY1_PRESSED_SLOT, key2lcd_key1_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_2_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key2_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_2_INDEX] = '2';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_PRESSED, KEY2LCD_KEY2_PRESSED_SLOT, key2lcd_key2_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_3_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key3_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_3_INDEX] = '3';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_PRESSED, KEY2LCD_KEY3_PRESSED_SLOT, key2lcd_key3_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_4_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key4_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_4_INDEX] = '4';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_4_PRESSED, KEY2LCD_KEY4_PRESSED_SLOT, key2lcd_key4_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_5_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key5_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_5_INDEX] = '5';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_PRESSED, KEY2LCD_KEY5_PRESSED_SLOT, key2lcd_key5_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_6_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key6_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_6_INDEX] = '6';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_6_PRESSED, KEY2LCD_KEY6_PRESSED_SLOT, key2lcd_key6_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_7_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key7_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_7_INDEX] = '7';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_7_PRESSED, KEY2LCD_KEY7_PRESSED_SLOT, key2lcd_key7_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_8_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key8_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_8_INDEX] = '8';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_8_PRESSED, KEY2LCD_KEY8_PRESSED_SLOT, key2lcd_key8_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_9_PRESSED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key9_pressed_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_9_INDEX] = '9';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_PRESSED, KEY2LCD_KEY9_PRESSED_SLOT, key2lcd_key9_pressed_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_0_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key0_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_0_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_RELEASED, KEY2LCD_KEY0_RELEASED_SLOT, key2lcd_key0_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_1_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key1_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_1_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_RELEASED, KEY2LCD_KEY1_RELEASED_SLOT, key2lcd_key1_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_2_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key2_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_2_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_RELEASED, KEY2LCD_KEY2_RELEASED_SLOT, key2lcd_key2_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_3_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key3_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_3_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_3_RELEASED, KEY2LCD_KEY3_RELEASED_SLOT, key2lcd_key3_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_4_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key4_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_4_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_4_RELEASED, KEY2LCD_KEY4_RELEASED_SLOT, key2lcd_key4_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_5_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key5_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_5_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_RELEASED, KEY2LCD_KEY5_RELEASED_SLOT, key2lcd_key5_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_6_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key6_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_6_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_6_RELEASED, KEY2LCD_KEY6_RELEASED_SLOT, key2lcd_key6_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_7_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key7_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_7_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_7_RELEASED, KEY2LCD_KEY7_RELEASED_SLOT, key2lcd_key7_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_8_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key8_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_8_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_8_RELEASED, KEY2LCD_KEY8_RELEASED_SLOT, key2lcd_key8_released_SLOT_CALLBACK)

/**
 * @brief Callback to handle the KEY_9_RELEASED event in keypad_to_lcd_task
 * 
 * @param p_argument 
 */
static void key2lcd_key9_released_SLOT_CALLBACK(const void* p_argument) {
    (void) p_argument;
    lcd_text[KEY2LCD_KEY_9_INDEX] = ' ';
    lcd2key_is_active = 1;
}
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_RELEASED, KEY2LCD_KEY9_RELEASED_SLOT, key2lcd_key9_released_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void KEYPAD_TO_LCD_TASK_start(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 KEYPAD_TO_LCD_TASK_get_schedule_interval(void) {
    return 100;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE KEYPAD_TO_LCD_task_get_state(void) {
    return lcd2key_is_active != 0 ? MCU_TASK_RUNNING : MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void KEYPAD_TO_LCD_TASK_execute(void) {

    // one lcd-line per signal call.
    SIGNAL_LCD_LINE_send(&lcd_text[0]);
    SIGNAL_LCD_LINE_send(&lcd_text[16]);

    lcd2key_is_active = 0;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void KEYPAD_TO_LCD_TASK_terminate(void) {
    
}

// --------------------------------------------------------------------------------

TASK_CREATE(
   KEYPAD_TO_LCD_TASK,
   TASK_PRIORITY_MIDDLE,
   KEYPAD_TO_LCD_TASK_get_schedule_interval,
   KEYPAD_TO_LCD_TASK_start,
   KEYPAD_TO_LCD_TASK_execute,
   KEYPAD_TO_LCD_task_get_state,
   KEYPAD_TO_LCD_TASK_terminate
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
void key_2_lcd_init(void) {

    for (u8 i = 0; i < sizeof(lcd_text); i++) {
        lcd_text[i] = ' ';
    }

    lcd_set_enabled(LCD_ENABLE);

    KEY2LCD_KEY0_PRESSED_SLOT_connect();
    KEY2LCD_KEY1_PRESSED_SLOT_connect();
    KEY2LCD_KEY2_PRESSED_SLOT_connect();
    KEY2LCD_KEY3_PRESSED_SLOT_connect();
    KEY2LCD_KEY4_PRESSED_SLOT_connect();
    KEY2LCD_KEY5_PRESSED_SLOT_connect();
    KEY2LCD_KEY6_PRESSED_SLOT_connect();
    KEY2LCD_KEY7_PRESSED_SLOT_connect();
    KEY2LCD_KEY8_PRESSED_SLOT_connect();
    KEY2LCD_KEY9_PRESSED_SLOT_connect();

    KEY2LCD_KEY0_RELEASED_SLOT_connect();
    KEY2LCD_KEY1_RELEASED_SLOT_connect();
    KEY2LCD_KEY2_RELEASED_SLOT_connect();
    KEY2LCD_KEY3_RELEASED_SLOT_connect();
    KEY2LCD_KEY4_RELEASED_SLOT_connect();
    KEY2LCD_KEY5_RELEASED_SLOT_connect();
    KEY2LCD_KEY6_RELEASED_SLOT_connect();
    KEY2LCD_KEY7_RELEASED_SLOT_connect();
    KEY2LCD_KEY8_RELEASED_SLOT_connect();
    KEY2LCD_KEY9_RELEASED_SLOT_connect();

    KEYPAD_TO_LCD_TASK_init();
}

// --------------------------------------------------------------------------------
