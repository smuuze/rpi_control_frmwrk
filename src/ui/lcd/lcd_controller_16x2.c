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
 * @file    lcd_controller.c
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 05
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

#include "common/signal_slot_interface.h"
#include "common/local_module_status.h"
#include "common/qeue_interface.h"
#include "common/common_tools_string.h"

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "driver/lcd/lcd_driver_interface.h"
#include "ui/lcd/lcd_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief maximum number of lines taht can be stored into the
 * internal fifo. If the fifo is full additional lines will be discarded.
 */
#define LCD_FIFO_DEEP                       5

// --------------------------------------------------------------------------------

/**
 * @brief Interval in milliseconds to schedule the lcd task
 */
#define LCD_TASK_SCHEDULE_INTERVAL_MS       100

/**
 * @brief Time interval in milliseconds wihtin the LCD is not updated
 * even if there are new data available. This gives the user to possibility
 * to read the LCD content before it will be overwritten with new data.
 * The timeout is applied after every second line that was written.
 */
#define LCD_TASK_UPDATE_TIMEOUT_MS          3000

/**
 * @brief Timeout in milliseconds between every character that is new written.
 * This will casue a more smooth update of the LCD content
 * -1 to ensure time has passed.
 */
#define LCD_TASK_CHARACTER_TIMEOUT_MS       (LCD_TASK_SCHEDULE_INTERVAL_MS - 1)

// --------------------------------------------------------------------------------

#define LCD_NUM_LINES                       2
#define LCD_NUM_CHARS                       16

// --------------------------------------------------------------------------------

/**
 * @see ui_lcd_interface.h#SIGNAL_LCD_LINE
 */
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_LCD_LINE)

/**
 * @see ui_lcd_interface.h#SIGNAL_LCD_UPDATED
 */
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_LCD_UPDATED)

// --------------------------------------------------------------------------------

/**
 * @brief Slot to receive a new line that will be printed on the connected LCD.
 * The line is stored into a fifo first and will be processed within the next
 * tas-cycle of the lcd-task. The maximum number of lines that are buffered is defined by
 * LCD_FIFO_DEEP. If the fifo is full, new lines will be discarded.
 */
static void lcd_task_slot_new_line(const void* p_arg);
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_LCD_LINE, SLOT_LCD_LINE, lcd_task_slot_new_line)

// --------------------------------------------------------------------------------

/**
 * @brief Queue realizing the New-Line-Fifo
 * New Lines received by the SIGNAL_LCD_LINE are stored into this fifo.
 * Lines are taken from the fifo by the LCD-task within a task-cylce.
 */
QEUE_INTERFACE_BUILD_QEUE(LCD_LINE_QUEUE, char, LCD_NUM_CHARS, LCD_FIFO_DEEP)

// --------------------------------------------------------------------------------

/**
 * @brief Timer to realize time depending operations
 * of the LCD-task. E.g. waiting for the update timeout.
 */
TIME_MGMN_BUILD_STATIC_TIMER_U16(LCD_TASK_OP_TIMER)

// --------------------------------------------------------------------------------

/**
 * @brief The LCD-task will activate a timeout after every second line that
 * was written. This helps the user to read the content before it will be overwritten
 * 
 */
#define LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN         (1 << 0)

/**
 * @brief If set the LCD is enabled and can be used.
 * if not set, the LCD-task is disabled at all and will do nothing
 * until it is enabled.
 * 
 */
#define LCD_CONTROLLER_STATUS_IS_ENABLED                  (1 << 1)

/**
 * @brief Current status of the LCD-TASK
 */
BUILD_MODULE_STATUS_U8(LCD_CONTROLLER_STATUS)

// --------------------------------------------------------------------------------

/**
 * @brief Task states that are used by this module
 * 
 */
typedef enum {

    /**
     * @brief Initializes the LCD after power-on
     */
    LCD_TASK_STATE_INIT,

    /**
     * @brief Waits for a new line to write on the LCD
     */
    LCD_TASK_STATE_IDLE,

    /**
     * @brief Loads the next line from the fifo.
     * The content of the LCD is shifted up by one line in this state
     */
    LCD_TASK_STATE_LOAD_NEXT_LINE,

    /**
     * @brief Prints the new line on the LCD
     */
    LCD_TASK_STATE_PRINT_NEW_LINE,

    /**
     * @brief waits the update time interval before
     * writing a new line to the LCD
     * 
     */
    LCD_TASK_STATE_WAIT

} LCD_TASK_STATE_TYPE;

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
static void lcd_task_init(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 lcd_task_get_schedule_interval(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE lcd_task_get_state(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void lcd_task_run(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void lcd_task_terminate(void);

TASK_CREATE (
    LCD_TASK,
    TASK_PRIORITY_MIDDLE,
    lcd_task_get_schedule_interval,
    lcd_task_init,
    lcd_task_run,
    lcd_task_get_state,
    lcd_task_terminate
)

/**
 * @brief actual state of this task
 * 
 */
static LCD_TASK_STATE_TYPE lcd_task_state = LCD_TASK_STATE_INIT;

// --------------------------------------------------------------------------------

/**
 * @brief Buffer where the new line is temporarily stored.
 * 
 */
static char lcd_task_new_line_buffer[LCD_NUM_CHARS];

// --------------------------------------------------------------------------------

/**
 * @see lcd_interface.h#lcd_init
 */
void lcd_init(void) {

    DEBUG_PASS("lcd_init");

    SIGNAL_LCD_LINE_init();
    SIGNAL_LCD_UPDATED_init();

    SIGNAL_LCD_LINE_set_timeout(0);
    SLOT_LCD_LINE_connect();

    LCD_TASK_init();
}

/**
 * @see lcd_interface.h#lcd_set_enabled
 */
void lcd_set_enabled(u8 enabled) {
    if (enabled) {
        LCD_CONTROLLER_STATUS_set(LCD_CONTROLLER_STATUS_IS_ENABLED);
    } else {
        LCD_CONTROLLER_STATUS_unset(LCD_CONTROLLER_STATUS_IS_ENABLED);
    }
}

/**
 * @see lcd_interface.h#lcd_get_line_count
 */
u8 lcd_get_line_count(void) {
    return lcd_driver_line_count();
}

/**
 * @see lcd_interface.h#lcd_get_character_count
 */
u8 lcd_get_character_count(void) {
    return lcd_driver_character_count();
}

// --------------------------------------------------------------------------------

/**
 * @brief Loads the next line from the fifo.
 * 
 * @return 1 if the new line was loaded,
 *         otherwise 0, because the mutex could not be loadad
 */
static u8 lcd_controller_load_new_line(void) {

    if (LCD_LINE_QUEUE_is_empty()) {
        DEBUG_PASS("lcd_controller_load_new_line() - QUEUE is empty");
        return 0;
    }

    if (LCD_LINE_QUEUE_mutex_get() == 0) {
        DEBUG_PASS("lcd_controller_load_new_line() - Get QUEUE-mutex failed");
        return 0;
    }

    DEBUG_PASS("lcd_controller_load_new_line()");

    LCD_LINE_QUEUE_deqeue(&lcd_task_new_line_buffer[0]);
    LCD_LINE_QUEUE_mutex_release();

    u8 length = common_tools_string_length(&lcd_task_new_line_buffer[0]);

    lcd_driver_set_line(&lcd_task_new_line_buffer[0], length);
    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void lcd_task_init(void) {
    DEBUG_PASS("lcd_task_init()");
    lcd_task_state = LCD_TASK_STATE_INIT;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 lcd_task_get_schedule_interval(void) {
    return LCD_TASK_SCHEDULE_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE lcd_task_get_state(void) {

    if (LCD_CONTROLLER_STATUS_is_set(LCD_CONTROLLER_STATUS_IS_ENABLED) == 0) {
        return MCU_TASK_SLEEPING;
    }

    if (lcd_task_state != LCD_TASK_STATE_IDLE) {
        return MCU_TASK_RUNNING;
    }

    if (LCD_LINE_QUEUE_is_empty() == 0) {
        return MCU_TASK_RUNNING;
    }

    return MCU_TASK_SLEEPING;

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void lcd_task_run(void) {

    switch (lcd_task_state) {

        case LCD_TASK_STATE_INIT:

            lcd_driver_init();

            lcd_task_state = LCD_TASK_STATE_IDLE;
            DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_IDLE");

            break;

        case LCD_TASK_STATE_IDLE:

            if (LCD_LINE_QUEUE_is_empty()) {
                break;
            }

            DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_LOAD_NEXT_LINE");
            lcd_task_state = LCD_TASK_STATE_LOAD_NEXT_LINE;
            // no break;
            // fall through

        case LCD_TASK_STATE_LOAD_NEXT_LINE:

            if (lcd_controller_load_new_line()) {

                LCD_TASK_OP_TIMER_start();

                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_PRINT_NEW_LINE");
                lcd_task_state = LCD_TASK_STATE_PRINT_NEW_LINE;
            }
            
            break;

        case LCD_TASK_STATE_PRINT_NEW_LINE:

            if (LCD_TASK_OP_TIMER_is_up(LCD_TASK_CHARACTER_TIMEOUT_MS) == 0) {
                // enable a smooth update of the new line
                break;
            }

            if (lcd_driver_update_screen(LCD_DRIVER_UPDATE_MODE_LAST_LINE_SMOOTH) == 0) {

                // updating the line not finished
                // do nothing

            } else if (LCD_CONTROLLER_STATUS_is_set(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN)) {

                LCD_CONTROLLER_STATUS_unset(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN);
                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_WAIT");
                lcd_task_state = LCD_TASK_STATE_WAIT;

                if (LCD_LINE_QUEUE_is_empty()) {
                    SIGNAL_LCD_UPDATED_send(NULL);
                }

            } else {

                LCD_CONTROLLER_STATUS_set(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN);
                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_IDLE");
                lcd_task_state = LCD_TASK_STATE_IDLE;

                if (LCD_LINE_QUEUE_is_empty()) {
                    SIGNAL_LCD_UPDATED_send(NULL);
                }
            }

            LCD_TASK_OP_TIMER_start();
            break;

        case LCD_TASK_STATE_WAIT:

            if (LCD_TASK_OP_TIMER_is_up(LCD_TASK_UPDATE_TIMEOUT_MS)) {
                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_IDLE");
                lcd_task_state = LCD_TASK_STATE_IDLE;
            }

            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void lcd_task_terminate(void) {
    
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_interface.c#lcd_task_slot_new_line
 */
static void lcd_task_slot_new_line(const void* p_arg) {

    if (p_arg == NULL) {
        DEBUG_PASS("lcd_task_slot_new_line() - NULL-POINTER");
        return;
    }

    if (LCD_LINE_QUEUE_is_full()) {
        DEBUG_PASS("lcd_task_slot_new_line() - OVERFLOW");
        return;
    }

    const char* p_new_line = (const char*)p_arg;

    LCD_LINE_QUEUE_mutex_get();
    LCD_LINE_QUEUE_enqeue(p_new_line);
    LCD_LINE_QUEUE_mutex_release();
    
    DEBUG_TRACE_STR(p_new_line, "lcd_task_slot_new_line() - New Line:");
}

// --------------------------------------------------------------------------------
