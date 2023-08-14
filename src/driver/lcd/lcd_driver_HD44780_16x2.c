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
 * @file    lcd_driver_HD44780_16x2.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 17
 * @brief   Implementation of the lcd-driver interface
 *          to be used for a HD44780 based lcd
 *          with two lines and 16 characters per line.
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

#include "string.h"

// --------------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"
#include "driver/lcd/lcd_driver_interface.h"

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
 * @brief A empty line
 */
#define LCD_TASK_EMPTY_LINE                 "                "

// --------------------------------------------------------------------------------

/**
 * @brief This buffer is a copy of the current display content.
 * All changes made here are applied to the display.
 */
static char line_buffer[LCD_NUM_LINES][LCD_NUM_CHARS + 1];

// --------------------------------------------------------------------------------

static u8 is_initialized = 0;

/**
 * @brief 
 * 
 * @param pins 
 */
static void lcd_set_pins(u8 pins) {

    // DEBUG_TRACE_byte(pins, "lcd_set_pins() - pins:");
    
    if (pins & LCD_PIN_RS) LCD_RS_drive_high();  else  LCD_RS_drive_low();

    if (pins & LCD_PIN_D4) LCD_D4_drive_high();  else  LCD_D4_drive_low();
    if (pins & LCD_PIN_D5) LCD_D5_drive_high();  else  LCD_D5_drive_low();
    if (pins & LCD_PIN_D6) LCD_D6_drive_high();  else  LCD_D6_drive_low();
    if (pins & LCD_PIN_D7) LCD_D7_drive_high();  else  LCD_D7_drive_low();

    LCD_EN_drive_high();
    rtc_timer_delay(50);  // wait for LCD

    LCD_EN_drive_low();
    rtc_timer_delay(50); // wait for LCD
}

/**
 * @brief Sets the address-pointer of the LCD to the start position of the selected line
 * 
 * @param line_index Index of the line that will be udpated on next write
 *                   0: top line / 1 bottom line
 */
static void lcd_driver_select_line(u8 line_index) {

    static u8 line_index_array[] = {
        LCD_PIN_D7,                 // Index 0 - top line
        LCD_PIN_D7 | LCD_PIN_D6,    // Index 1 - bottom line
    };

    if (line_index < sizeof(line_index_array)) {

        DEBUG_PASS("lcd_driver_select_line() - Line 1");

        lcd_set_pins(line_index_array[line_index]);
        lcd_set_pins(0);
    }
}

/**
 * @brief Writes a single character on the LCD using the 4-bit interface
 * 
 * @param character the character to write on the LCD
 */
static void lcd_driver_write_char(char character) {
    lcd_set_pins(LCD_PIN_RS | (u8)(character >> 4));
    lcd_set_pins(LCD_PIN_RS | (u8)(character & 0x0F));
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_init
 */
void lcd_driver_init(void) {

    DEBUG_PASS("lcd_driver_init() - Init GPIO");

    LCD_RS_activate();    LCD_RS_drive_low();
    LCD_EN_activate();    LCD_EN_drive_low();

    LCD_D4_activate();    LCD_D4_drive_low();
    LCD_D5_activate();    LCD_D5_drive_low();
    LCD_D6_activate();    LCD_D6_drive_low();
    LCD_D7_activate();    LCD_D7_drive_low();

    DEBUG_PASS("lcd_driver_init() - Power Up");

    rtc_timer_delay(15 * 1000); // wait 15 ms for LCD controller power-up

    // Initialization sequence to activate 4-bit interface
    lcd_set_pins(LCD_PIN_D5 | LCD_PIN_D4);  rtc_timer_delay(5 * 1000);
    lcd_set_pins(LCD_PIN_D5 | LCD_PIN_D4);  rtc_timer_delay(100);
    lcd_set_pins(LCD_PIN_D5 | LCD_PIN_D4);
    lcd_set_pins(LCD_PIN_D5); // 4-Bit interface

    DEBUG_PASS("lcd_driver_init() - Configure Display");

    // FUNCTION SET - 2 Lines / 5x8 font
    lcd_set_pins(LCD_PIN_D5);
    lcd_set_pins(LCD_PIN_D7); // two lines

    // DISPLAY ON / Cursor off / Blink Cursor off 
    lcd_set_pins(0);
    lcd_set_pins(LCD_PIN_D7 | LCD_PIN_D6);

    // CLEAR DISPLAY
    lcd_set_pins(0);
    lcd_set_pins(LCD_PIN_D4);

    rtc_timer_delay(1640); // execution time of CLEAR-DISPLAY command

    // ENTRY MODE SET - Cursor auto increment
    lcd_set_pins(0);
    lcd_set_pins(LCD_PIN_D6 | LCD_PIN_D5);

    DEBUG_PASS("lcd_driver_init() - Clear buffer");

    u8 line_cnt = 0;
    for ( ; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
        memset(line_buffer[line_cnt], ' ', LCD_NUM_CHARS);
        line_buffer[line_cnt][LCD_NUM_CHARS] = '\0';
    }

    is_initialized = 1;
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_deinit
 */
void lcd_driver_deinit(void) {
    is_initialized = 0;
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_set_line
 */
void lcd_driver_set_line(const char* message, u8 length) {

    if (is_initialized == 0) {
        DEBUG_PASS("lcd_driver_set_line() - Need to initialize LCD-Interface");
        return;
    }

    DEBUG_TRACE_byte(length, "lcd_driver_set_line() - Length:");
    DEBUG_TRACE_STR(message, "lcd_driver_set_line() - New Line:");

    // Shift up the lines
    for (u8 line_cnt = 0 ; line_cnt < LCD_NUM_LINES - 1; line_cnt += 1) {

        memcpy(
            &line_buffer[line_cnt][0],
            &line_buffer[line_cnt + 1][0],
            LCD_NUM_CHARS
        );
    }

    // Ensure to only copy the maximum number of characters
    if (length > LCD_NUM_CHARS) {
        length = LCD_NUM_CHARS;
    }

    // Copy the new line into the LCD-Buffer
    memcpy(&line_buffer[LCD_NUM_LINES - 1][0], message, length);

    // if needed set the remaining lcd-characters to white-spaces
    u8 reaming_length = LCD_NUM_CHARS - length;
    if (reaming_length != 0 ) {
        memset(&line_buffer[LCD_NUM_LINES - 1][length], ' ', reaming_length);
    }
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_update_screen
 */
u8 lcd_driver_update_screen(u8 mode) {

    static u8 state = 0;
    static u8 char_cnt = 0;

    if (state == 0) {

        DEBUG_TRACE_STR(line_buffer[0], "lcd_driver_update_screen() - Select Line 1");
        lcd_driver_select_line(LCD_LINE_ONE);
        char_cnt = 0;
        state = 1;
    }
    
    if (state == 1) {

        /**
         * @brief update the content of the first line
         */
        for (char_cnt = 0; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
            lcd_driver_write_char(line_buffer[LCD_LINE_ONE][char_cnt]);
        }

        // Go on to the second line
        state = 2;
    } 
    
    if (state == 2) {

        DEBUG_TRACE_STR(line_buffer[1], "lcd_driver_update_screen() - Select Line 2");
        lcd_driver_select_line(LCD_LINE_TWO);

        if (mode & LCD_DRIVER_UPDATE_MODE_LAST_LINE_SMOOTH) {

            DEBUG_PASS("lcd_driver_update_screen() - Delete content of last line");

            for (char_cnt = 0; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
                lcd_driver_write_char(' ');
            }

            lcd_driver_select_line(LCD_LINE_TWO);
        }

        char_cnt = 0;
        state = 3;
    }
    
    if (state == 3) {

        /**
         * @brief update the content of the second line
         */
        for (; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
            lcd_driver_write_char(line_buffer[LCD_LINE_TWO][char_cnt]);

            if (mode & LCD_DRIVER_UPDATE_MODE_LAST_LINE_SMOOTH) {

                DEBUG_CODE_BLOCK (
                    char t_buffer[LCD_NUM_CHARS + 1];
                    memset(t_buffer, '\0', LCD_NUM_CHARS + 1);
                    memcpy(t_buffer, &line_buffer[LCD_LINE_TWO][0], char_cnt+1 );
                    DEBUG_TRACE_STR(t_buffer, "lcd_driver_update_screen() - Content Line 2:");
                )

                // we need to count up here, because we leave the loop.
                char_cnt += 1;
                return 0;
            }
        }

        // update finished
        state = 0;
    }

    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_line_count
 */
u8 lcd_driver_line_count(void) {
    return LCD_NUM_LINES;
}

// --------------------------------------------------------------------------------

/**
 * @see lcd_driver_interface.h#lcd_driver_character_count
 */
u8 lcd_driver_character_count(void) {
    return LCD_NUM_CHARS;
}

// --------------------------------------------------------------------------------
