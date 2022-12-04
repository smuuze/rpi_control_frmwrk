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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "ui/lcd/ui_lcd_interface.h"

// --------------------------------------------------------------------------------

// helping definitions for fast access to pins via function lcd_set_pins()
#define LCD_PIN_RS                          (1 << 7)
#define LCD_PIN_D4                          (1 << 0)
#define LCD_PIN_D5                          (1 << 1)
#define LCD_PIN_D6                          (1 << 2)
#define LCD_PIN_D7                          (1 << 3)

#define LCD_NUM_LINES                       2
#define LCD_NUM_CHARS                       16

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
 * 
 */
#define LCD_TASK_CHARACTER_TIMEOUT_MS       100

/**
 * @brief A empty line
 * 
 */
#define LCD_TASK_EMPTY_LINE                 "                "

// --------------------------------------------------------------------------------

/**
 * @brief Signals a new Line to write on the connected LCD
 * One line per Signal can be send. The supported length of the text-line
 * depends on the currently used LCD. Characters that does not fit will be discarded.
 * The new line must end with the termination character '\0'
 */
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_LCD_LINE)

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
     * @brief Moves the content of the LCD one line up
     * to add the new line at the bottom
     */
    LCD_TASK_STATE_MOVE_LINE_UP,

    /**
     * @brief Loads the next line from the fifo
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
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.finish
 */
static void lcd_task_finish(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void lcd_task_terminate(void);

/**
 * @brief structure of the task-implementation of this module
 */
static MCU_TASK_INTERFACE lcd_controller_task = {
    0,                                  //     identifier,
    0,                                  //     new_run_timeout,
    0,                                  //     last_run_time,
    &lcd_task_init,                     //     init,
    &lcd_task_get_schedule_interval,    //     get_schedule_interval,
    &lcd_task_get_state,                //     get_sate,
    &lcd_task_run,                      //     run,
    0,                                  //     background_run,
    0,                                  //     sleep,
    0,                                  //     wakeup,
    &lcd_task_finish,                   //     finish,
    &lcd_task_terminate,                //     terminate,
    0                                   //     next-task
};

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
 * @brief This buffer is a copy of the current display content.
 * All changes made here are applied to the display.
 */
static char line_buffer[LCD_NUM_LINES][LCD_NUM_CHARS + 1];

static u8 is_initialized = 0;

/**
 * @brief 
 * 
 * @param pins 
 */
static void lcd_set_pins(u8 pins) {
    
    if (pins & LCD_PIN_RS) LCD_RS_drive_high();  else  LCD_RS_drive_low();

    if (pins & LCD_PIN_D4) LCD_D4_drive_high();  else  LCD_D4_drive_low();
    if (pins & LCD_PIN_D5) LCD_D5_drive_high();  else  LCD_D5_drive_low();
    if (pins & LCD_PIN_D6) LCD_D6_drive_high();  else  LCD_D6_drive_low();
    if (pins & LCD_PIN_D7) LCD_D7_drive_high();  else  LCD_D7_drive_low();

    usleep(50); // reduce cpu-load

    LCD_EN_drive_high();
    usleep(50); // reduce cpu-load
    LCD_EN_drive_low();
}

/**
 * @brief 
 * 
 * @param line_index 
 */
static void lcd_driver_select_line(u8 line_index) {

    switch (line_index) {
        default :
        case 0 : // Line 1
            lcd_set_pins(LCD_PIN_D7 | 0x00);
            lcd_set_pins(0x00);
            break;

        case 1 : // Line 2
            lcd_set_pins(LCD_PIN_D7 | LCD_PIN_D6);
            lcd_set_pins(0);
            break;
    }
}

void lcd_driver_write_char(char character) {
    lcd_set_pins(LCD_PIN_RS | (u8)(character >> 4));
    lcd_set_pins(LCD_PIN_RS | (u8)(character & 0x0F));
}

void lcd_driver_init(void) {

    DEBUG_PASS("lcd_driver_init() - activate");

    LCD_RS_activate();    LCD_RS_drive_low();
    LCD_EN_activate();    LCD_EN_drive_low();

    LCD_D4_activate();    LCD_D4_drive_low();
    LCD_D5_activate();    LCD_D5_drive_low();
    LCD_D6_activate();    LCD_D6_drive_low();
    LCD_D7_activate();    LCD_D7_drive_low();

    DEBUG_PASS("lcd_driver_init() - running init sequence");

    // -----------------------------------------------------

    usleep(15 * 1000); // wait for LCD controller power-up

    lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5);     usleep(5 * 1000);
    lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5);     usleep(100);
    lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5);     usleep(5 * 1000);

    lcd_set_pins(LCD_PIN_D5);         usleep(5 * 1000); 

    // -----------------------------------------------------

    lcd_set_pins(LCD_PIN_D5);               // switch to 4-Bit interface
    lcd_set_pins(LCD_PIN_D7);               // 2 Lines / 5x8 Font

    lcd_set_pins(0);                        // cmd control
    lcd_set_pins(LCD_PIN_D7 | LCD_PIN_D6);  // enable display

    lcd_set_pins(0);
    lcd_set_pins(LCD_PIN_D4);               // display clear

    lcd_set_pins(0);                        // Entry mode set
    lcd_set_pins(LCD_PIN_D6 | LCD_PIN_D5);  // cursor increment

    usleep(2 * 1000);

    u8 line_cnt = 0;
    for ( ; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
        memset(line_buffer[line_cnt], ' ', LCD_NUM_CHARS);
        line_buffer[line_cnt][LCD_NUM_CHARS] = '\0';
    }

    is_initialized = 1;
}

void lcd_driver_deinit(void) {
    is_initialized = 0;
}

void lcd_driver_write_line(const char* message, u8 length) {

    if (is_initialized == 0) {
        DEBUG_PASS("lcd_driver_write_line() - Need to initialize LCD-Interface");
        return;
    }

    DEBUG_TRACE_STR(message, "lcd_driver_write_line() - New Line:");

    u8 line_cnt = 0;
    u8 char_cnt = 0;

    /**
     * @brief Shift up the lines that are actual on the LCD
     */
    for ( ; line_cnt < LCD_NUM_LINES - 1; line_cnt += 1) {

        memcpy(
            &line_buffer[line_cnt][0],
            &line_buffer[line_cnt + 1][0],
            LCD_NUM_CHARS
        );

        // for ( ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
        //     line_buffer[line_cnt][char_cnt] = line_buffer[line_cnt + 1][char_cnt];
        // }
    }

    if (length > LCD_NUM_CHARS) {
        length = LCD_NUM_CHARS;
    }

    /**
     * @brief Copy the new line into the LCD-Buffer
     */
    memcpy(&line_buffer[LCD_NUM_LINES - 1][0], message, length);
    // for (char_cnt = 0 ; char_cnt < length; char_cnt += 1) {
    //     line_buffer[LCD_NUM_LINES - 1][char_cnt] = message[char_cnt];
    // }
        
    /**
     * @brief Fill the rest of the line with blanks
     */
    if (length < LCD_NUM_CHARS) {
        memset(
            &line_buffer[LCD_NUM_LINES - 1][length],
            ' ',
            LCD_NUM_CHARS - length
        );
        // for ( ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
        //     line_buffer[LCD_NUM_LINES - 1][char_cnt] = ' ';
        // }
    }

    DEBUG_PASS("lcd_driver_write_line() - LCD-Content:");

    /**
     * @brief Update the LCD content
     */
    for (line_cnt = 0; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
        
        lcd_driver_select_line(line_cnt);

        for (char_cnt = 0 ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
            lcd_driver_write_char(line_buffer[line_cnt][char_cnt]);
        }
    }
}

/**
 * @brief Get the number of lines of the current LCD.
 * 
 * @return number of lines of the current LCD.
 */
u8 lcd_driver_line_count(void) {
    return LCD_NUM_LINES;
}

/**
 * @brief Number of characters of a single line.
 * 
 * @return Number of characters of a single line.
 */
u8 lcd_driver_character_count(void) {
    return LCD_NUM_CHARS;
}

// --------------------------------------------------------------------------------

/**
 * @see ui/lcd/lcd_interface.h#lcd_init
 */
void lcd_controller_init(void) {

    SIGNAL_LCD_LINE_init();
    mcu_task_controller_register_task(&lcd_controller_task);
}

void lcd_controller_set_enabled(u8 enabled) {
    if (enabled) {
        LCD_CONTROLLER_STATUS_set(LCD_CONTROLLER_STATUS_IS_ENABLED);
    } else {
        LCD_CONTROLLER_STATUS_unset(LCD_CONTROLLER_STATUS_IS_ENABLED);
    }
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
        return 0;
    }

    if (LCD_LINE_QUEUE_mutex_get() == 0) {
        return 0;
    }

    LCD_LINE_QUEUE_deqeue(&lcd_task_new_line_buffer[0]);
    LCD_LINE_QUEUE_mutex_release();

    return 1;
}

/**
 * @brief Updates the last line with the currently loaded new line.
 * 
 * @return 1 if updating the line has finished, 0 otherwise
 */
static u8 lcd_controller_write_line(void) {

    static u8 character_counter = 0;

    if (character_counter < lcd_driver_character_count()) {

        lcd_driver_write_char(lcd_task_new_line_buffer[character_counter]);
        character_counter += 1;
        return 0;

    } else {

        character_counter = 0;
        return 1;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void lcd_task_init(void) {

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

            DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_MOVE_LINE_UP");
            lcd_task_state = LCD_TASK_STATE_MOVE_LINE_UP;
            // no break;

        case LCD_TASK_STATE_MOVE_LINE_UP:

            /**
             * @brief This causes the lcd driver to move to whole
             * content upwards and writing a new (empty) line
             * at the bottom of the display.
             */
            lcd_driver_write_line(LCD_TASK_EMPTY_LINE, LCD_NUM_CHARS);

            DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_LOAD_NEXT_LINE");
            lcd_task_state = LCD_TASK_STATE_LOAD_NEXT_LINE;

            break;

        case LCD_TASK_STATE_LOAD_NEXT_LINE:

            if (lcd_controller_load_new_line()) {

                LCD_TASK_OP_TIMER_stop();

                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_PRINT_NEW_LINE");
                lcd_task_state = LCD_TASK_STATE_PRINT_NEW_LINE;
            }
            
            break;

        case LCD_TASK_STATE_PRINT_NEW_LINE:

            if (LCD_TASK_OP_TIMER_is_up(LCD_TASK_CHARACTER_TIMEOUT_MS) == 0) {
                // enable a smooth update of the new line
                break;
            }

            if (lcd_controller_write_line() == 0) {

                // updating the line not finished
                // do nothing

            } else if (LCD_CONTROLLER_STATUS_is_set(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN)) {

                LCD_CONTROLLER_STATUS_unset(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN);
                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_WAIT");
                lcd_task_state = LCD_TASK_STATE_WAIT;

            } else {

                LCD_CONTROLLER_STATUS_set(LCD_CONTROLLER_STATUS_SECOND_LINE_WRITTEN);
                DEBUG_PASS("lcd_task_run() - LCD_TASK_STATE_IDLE");
                lcd_task_state = LCD_TASK_STATE_IDLE;
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
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.finish
 */
static void lcd_task_finish(void) {
    
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void lcd_task_terminate(void) {
    
}

// --------------------------------------------------------------------------------

/**
 * @see ui_lcd_16x2.c#lcd_task_slot_new_line
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
}

// --------------------------------------------------------------------------------
