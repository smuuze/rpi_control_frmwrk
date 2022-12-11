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
 * @file    gpio_linux_driver.c
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 28
 * @brief   Adapter to the linux gpio driver
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

#include <fcntl.h>

// --------------------------------------------------------------------------------

#include "cpu.h"
#include "driver/gpio/gpio_interface.h"
#include "driver/gpio/gpio_driver_linux/linux_gpio_driver_interface.h"

// --------------------------------------------------------------------------------

#define GPIO_DRIVER_INVERT_LEVEL(level)             level = (level == GPIO_LEVEL_HIGH) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH

// --------------------------------------------------------------------------------

INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_2) // I2C_SDA
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_4) // I2C_SCL
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_6) // GPIO04
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_7) // UART_TXD

INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_1) // UART_RXD
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_2) // GPIO17
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_3) // GPIO18
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_4) // GPIO27
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_6) // GPIO22
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_7) // GPIO23

INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_1) // GPIO24
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_2) // SPI_MOSI
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_4) // SPI_MISO
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_5) // GPIO25
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_6) // SPI_SCLK
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_7) // SPI_CE0

INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_1) // SPI_CE1
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_4) // GPIO05
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_6) // GPIO06
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_7) // GPIO12

INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_0) // GPIO13
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_2) // GPIO19
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_3) // GPIO16
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_4) // GPIO26
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_5) // GPIO20
INCLUDE_GPIO_REFRENCE(GPIO_PORT_E, GPIO_PIN_7) // GPIO21

// --------------------------------------------------------------------------------

/**
 * @brief Handle to use for gpio-access
 * 
 */
static i32 gpio_handle = -1;

/**
 * @brief Command to use inside of this adapter.
 * Is udpated with new data if needed
 * 
 */
static GPIO_DRIVER_RW_CMD gpio_command;

// --------------------------------------------------------------------------------

/**
 * @brief Gets the ID of the GPIO pin given by p_pin_descr on the actual system
 * 
 * @param p_pin_descr gpio descriptor of the gpio pin
 * @return number of the gpio-pin on the actual system referenced by p_pin_descr
 */
static u8 gpio_driver_get_pin_id(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

    static u8 gpio_num_array[5][8] = {
    
        /* Port / Pin   0       1       2       3       4       5       6       7  */
        /*/ Port A */ { 0xFF,   0xFF,   2,      0xFF,   3,      0xFF,   4,      14   }, 
        /*/ Port B */ { 0xFF,   15,     17,     18,     27,     0xFF,   22,     23   }, 
        /*/ Port C */ { 0xFF,   24,     10,     0xFF,   9,      25,     11,     8    },
        /*/ Port D */ { 0xFF,   7,      0xFF,   0xFF,   5,      0xFF,   6,      12   },
        /*/ Port E */ { 13,     0xFF,   19,     16,     26,     20,     0xFF,   21   }
    };

    u8 pin_num = gpio_num_array[p_pin_descr->port_id][p_pin_descr->pin_id];

    // if (p_pin_descr->port_id == GPIO_PORT_A) {

    //     if (p_pin_descr->pin_id == GPIO_PIN_2) { pin_num = 2; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_4) { pin_num = 3; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_6) { pin_num = 4; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_7) { pin_num = 14; }

    // } else if (p_pin_descr->port_id == GPIO_PORT_B) {

    //     if (p_pin_descr->pin_id == GPIO_PIN_1) { pin_num = 15; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_2) { pin_num = 17; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_3) { pin_num = 18; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_4) { pin_num = 27; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_6) { pin_num = 22; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_7) { pin_num = 23; }

    // } else if (p_pin_descr->port_id == GPIO_PORT_C) {

    //     if (p_pin_descr->pin_id == GPIO_PIN_1) { pin_num = 24; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_2) { pin_num = 10; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_4) { pin_num = 9; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_5) { pin_num = 25; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_6) { pin_num = 11; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_7) { pin_num = 8; }

    // } else if (p_pin_descr->port_id == GPIO_PORT_D) {

    //     if (p_pin_descr->pin_id == GPIO_PIN_1) { pin_num = 7; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_4) { pin_num = 5; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_6) { pin_num = 6; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_7) { pin_num = 12; }

    // } else if (p_pin_descr->port_id == GPIO_PORT_E) {

    //     if (p_pin_descr->pin_id == GPIO_PIN_0) { pin_num = 13; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_2) { pin_num = 19; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_3) { pin_num = 16; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_4) { pin_num = 26; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_5) { pin_num = 20; }
    //     if (p_pin_descr->pin_id == GPIO_PIN_7) { pin_num = 21; }
    // }

    return pin_num;
}

// --------------------------------------------------------------------------------

void gpio_driver_init(void) {

    DEBUG_PASS("gpio_driver_init_pin() - Going to initialize gpio-subset");

    GPIO_DRIVER_OPEN(gpio_handle);
    if (gpio_handle < 0) {
        DEBUG_TRACE_long(gpio_handle, "- Initializing GPIOD has FAILED !!! --- (err-code)");
        return;
    }

    #ifndef GPIO_NO_INIT_ON_START
    {
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_2)); // I2C_SDA
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_4)); // I2C_SCL
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_6)); // GPIO04
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_7)); // UART_TXD
        
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_1)); // UART_RXD
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_2)); // GPIO17
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_3)); // GPIO18
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_4)); // GPIO27
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_6)); // GPIO22
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_7)); // GPIO23
        
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_1)); // GPIO24
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_2)); // SPI_MOSI
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_4)); // SPI_MISO
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_5)); // GPIO25
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_6)); // SPI_SCLK
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_7)); // SPI_CE0
        
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_1)); // SPI_CE1
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_4)); // GPIO05
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_6)); // GPIO06
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_7)); // GPIO12

        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_0)); // GPIO13
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_2)); // GPIO19
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_3)); // GPIO16
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_4)); // GPIO26
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_5)); // GPIO20
        gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_E, GPIO_PIN_7)); // GPIO20

        DEBUG_PASS("gpio_driver_init_pin() - Initializing gpio-subset has succeeded!");
    }
    #endif
}

// --------------------------------------------------------------------------------

void gpio_driver_deinit(void) {
    if (gpio_handle >= 0) {
        GPIO_DRIVER_CLOSE(gpio_handle);
        gpio_handle = -1;
    }
}

// --------------------------------------------------------------------------------

void gpio_driver_init_pin(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

    #ifdef TRACER_ON
    {
        u8 pin_number = GPIO_GET_PIN_NUMBER(p_pin_descr);
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),  "gpio_driver_init_pin() - Pin-Number:");
    }
    #endif

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {

        DEBUG_PASS("gpio_driver_init_pin() - ignored");
        return;

    } else if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) != 0) {

        DEBUG_PASS("gpio_driver_init_pin() - is output");
        gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_OUTPUT);

    } else {

        DEBUG_PASS("gpio_driver_init_pin() - is input");
        gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_INPUT);
    }

    GPIO_DRIVER_LEVEL level = GPIO_LEVEL_HIGH_Z;

    if (GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr) != 0) {

        DEBUG_PASS("gpio_driver_init_pin() - is idle low");
        level = GPIO_LEVEL_LOW;

    } else if (GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr) != 0) {

        DEBUG_PASS("gpio_driver_init_pin() - is idle high");
        level = GPIO_LEVEL_HIGH;

    } else {
        DEBUG_PASS("gpio_driver_init_pin() - is idle high-z");
    }

    gpio_driver_set_level(p_pin_descr, level);
}

// --------------------------------------------------------------------------------

void gpio_driver_set_direction(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    if (direction == GPIO_DIRECTION_INPUT) {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr), "gpio_driver_set_direction() - new direction: INPUT - PIN:");
    } else {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr), "gpio_driver_set_direction() - new direction: OUTPUT - PIN:");
    }

    GPIO_DRIVER_WRITE_CMD(
        gpio_command, 
        gpio_driver_get_pin_id(p_pin_descr), 
        (direction == GPIO_DIRECTION_INPUT) ? GPIO_DRIVER_DIRECTION_INPUT : GPIO_DRIVER_DIRECTION_OUTPUT, 
        GPIO_DRIVER_LEVEL_UNCHANGED
    );

    if (GPIO_DRIVER_WRITE(gpio_handle, gpio_command) != 0) {
        DEBUG_PASS("gpio_driver_set_direction() - GPIO_DRIVER_WRITE() has FAIELD!");
    }
}

// --------------------------------------------------------------------------------

void gpio_driver_set_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),   "gpio_driver_set_level() - DEACTIVATED - PIN:");
        return;
    }

    if (level == GPIO_LEVEL_HIGH) {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),   "gpio_driver_set_level() - new level: HIGH - PIN:");
    } else if (level == GPIO_LEVEL_LOW) {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),   "gpio_driver_set_level() - new level: LOW - PIN:");
    } else {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),   "gpio_driver_set_level() - new level: HIGH-Z - PIN:");
    }

    if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
        GPIO_DRIVER_INVERT_LEVEL(level);
        DEBUG_TRACE_byte(level, "gpio_driver_get_level() - LEVEL inverted");
    }

    GPIO_DRIVER_WRITE_CMD(
        gpio_command, 
        gpio_driver_get_pin_id(p_pin_descr), 
        GPIO_DRIVER_DIRECTION_UNCHANGED,
        (level == GPIO_LEVEL_HIGH) ? GPIO_DRIVER_LEVEL_HIGH : GPIO_DRIVER_LEVEL_LOW
    );

    if (GPIO_DRIVER_WRITE(gpio_handle, gpio_command) != 0) {
        DEBUG_PASS("gpio_driver_set_direction() - GPIO_DRIVER_WRITE() has FAIELD!");
    }
}

// --------------------------------------------------------------------------------

void gpio_driver_toggle_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    
    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) == 0) {
        DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr), "gpio_driver_toggle_level() - This is not an output");
        return;
    }

    GPIO_DRIVER_WRITE_CMD(
        gpio_command, 
        gpio_driver_get_pin_id(p_pin_descr), 
        GPIO_DRIVER_DIRECTION_UNCHANGED,
        GPIO_DRIVER_LEVEL_TOGGLE
    );

    if (GPIO_DRIVER_WRITE(gpio_handle, gpio_command) != 0) {
        DEBUG_PASS("gpio_driver_set_direction() - GPIO_DRIVER_WRITE() has FAIELD!");
    }
}

// --------------------------------------------------------------------------------

GPIO_DRIVER_LEVEL gpio_driver_get_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    
    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return GPIO_LEVEL_HIGH_Z;
    }

    GPIO_DRIVER_READ_CMD(
        gpio_command, 
        gpio_driver_get_pin_id(p_pin_descr)
    );

    if (GPIO_DRIVER_READ(gpio_handle, gpio_command) != 0) {
        DEBUG_PASS("gpio_driver_set_direction() - GPIO_DRIVER_WRITE() has FAIELD!");
    }

    GPIO_DRIVER_LEVEL pin_level = (GPIO_DRIVER_GET_LEVEL(gpio_command) == GPIO_DRIVER_LEVEL_HIGH) ?
                                    GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;

    if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
        GPIO_DRIVER_INVERT_LEVEL(pin_level);
    }

    DEBUG_TRACE_byte(gpio_driver_get_pin_id(p_pin_descr),   "gpio_driver_get_level() - PIN");
    DEBUG_TRACE_byte(pin_level, "gpio_driver_get_level() - LEVEL");

    return pin_level;
}

// --------------------------------------------------------------------------------

void gpio_driver_print_pin_state(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
}

// --------------------------------------------------------------------------------

void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    p_pin_descr->pin_cfg |= GPIO_DEACTIVATE;
}

// --------------------------------------------------------------------------------

void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        p_pin_descr->pin_cfg &= ~GPIO_DEACTIVATE;
        gpio_driver_init_pin(p_pin_descr);
    }
}

// --------------------------------------------------------------------------------
