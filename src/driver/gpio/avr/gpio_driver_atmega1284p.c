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
 * @file    gpio_driver_atmega1284p.c
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 20
 * @brief   Implementation of the gpio-interface to drive the GPIOs of
 *          the ATMega 1284p mcu
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

#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_A, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_B, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_C, GPIO_PIN_7)

INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_0)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_1)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_2)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_3)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_4)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_5)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_6)
INCLUDE_GPIO_REFRENCE(GPIO_PORT_D, GPIO_PIN_7)

// --------------------------------------------------------------------------------

/**
 * @brief GEts the pin-number usable for the atmega1284p gpio-registers
 * from the pin-id of the gpio-interface.
 * 
 * @param pin pin-id of the gpio-interface
 * @return pin-number useable for the atmega1284p gpio-registers
 */
static u8 gpio_driver_atmega1284p_get_pin_number(u8 pin) {
    static u8 pin_array[] = {
        0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x020, 0x40
    };

    return pin_array[pin];
}

/**
 * @brief Sets directopn to output of the given port-pin pair
 * 
 * @param port port where to set output direction
 * @param pin_num pin number where to set output direction
 */
static inline void gpio_driver_atmega1284p_set_direction_output(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : DDRA |= (pin_num); break;
        case GPIO_PORT_B : DDRB |= (pin_num); break;
        case GPIO_PORT_C : DDRC |= (pin_num); break;
        case GPIO_PORT_D : DDRD |= (pin_num); break;
        default: break;
    }
}

/**
 * @brief Sets directopn to input of the given port-pin pair
 * 
 * @param port port where to set input direction
 * @param pin_num pin number where to set input direction
 */
static inline void gpio_driver_atmega1284p_set_direction_input(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : DDRA &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_B : DDRB &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_C : DDRC &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_D : DDRD &= (unsigned char)~(pin_num); break;
        default: break;
    }
}

/**
 * @brief Sets the direction of the given pin on the given port.
 * 
 * @param port port where to set the direction
 * @param pin_num pin-number where to set the direction
 * @param direction direction to apply on the given port-pin pair.s
 */
static void gpio_driver_atmega1284p_set_direction(u8 port, u8 pin_num, GPIO_DRIVER_DIRECTION direction) {
    if (direction == GPIO_DIRECTION_INPUT) {
        gpio_driver_atmega1284p_set_direction_input(port, pin_num);
    } else {
        gpio_driver_atmega1284p_set_direction_output(port, pin_num);
    }
}

/**
 * @brief Sets the level of the given pin at the given port to high.
 * If the port is unknown, nothing happens.
 * 
 * @param port port where to set the high-level
 * @param pin_num pin where to set the high-level
 */
static inline void gpio_driver_atmega1284p_set_pin_level_high(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : PORTA |= (pin_num); break;
        case GPIO_PORT_B : PORTB |= (pin_num); break;
        case GPIO_PORT_C : PORTC |= (pin_num); break;
        case GPIO_PORT_D : PORTD |= (pin_num); break;
        default: break;
    }
}

/**
 * @brief Sets the level of the given pin at the given port to low.
 * If the port is unknown, nothing happens.
 * 
 * @param port port where to set the low-level
 * @param pin_num pin where to set the low-level
 */
static inline void gpio_driver_atmega1284p_set_pin_level_low(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : PORTA &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_B : PORTB &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_C : PORTC &= (unsigned char)~(pin_num); break;
        case GPIO_PORT_D : PORTD &= (unsigned char)~(pin_num); break;
        default: break;
    }
}

/**
 * @brief Sets the level of the given pin at the given port to high-z.
 * If the port is unknown, nothing happens.
 * 
 * @param port port where to set high-z
 * @param pin_num pin where to set high-z
 */
static inline void gpio_driver_atmega1284p_set_pin_level_high_z(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A :  PORTA &= (unsigned char)~(pin_num);
                            DDRA  &= (unsigned char)~(pin_num);
                            break;
        case GPIO_PORT_B :  PORTB &= (unsigned char)~(pin_num);
                            DDRB  &= (unsigned char)~(pin_num);
                            break;
        case GPIO_PORT_C :  PORTC &= (unsigned char)~(pin_num);
                            DDRC  &= (unsigned char)~(pin_num);
                            break;
        case GPIO_PORT_D :  PORTD &= (unsigned char)~(pin_num);
                            DDRD  &= (unsigned char)~(pin_num);
                            break;
        default: break;
    }
}

/**
 * @brief Sets the level of the given pin at the given port.
 * If an unknown level is given, the pin-level will be set to GPIO_LEVEL_HIGH_Z
 * 
 * @param port port where to set the new level ( A,B,C,D )
 * @param pin_num pin-number where to set the new level ( 0,1,2,3, ... )
 * @param level Level to be set (GPIO_LEVEL_HIGH, GPIO_LEVEL_LOW, GPIO_LEVEL_HIGH_Z)
 */
static void gpio_driver_atmega1284p_set_pin_level(u8 port, u8 pin_num, GPIO_DRIVER_LEVEL level) {

    if (level == GPIO_LEVEL_HIGH) {
        gpio_driver_atmega1284p_set_pin_level_high(
            port,
            pin_num
        );

    } else if (level == GPIO_LEVEL_LOW) {
        gpio_driver_atmega1284p_set_pin_level_low(
            port,
            pin_num
        );

    } else {
        gpio_driver_atmega1284p_set_pin_level_high_z(
            port,
            pin_num
        );
    }
}

/**
 * @brief Toggles the current level of the given port pin pair.
 * Does not toggles the direction.
 * 
 * @param port port where to toggle the level
 * @param pin_num pin-number where to toggle the level
 */
static void gpio_driver_atmega1284p_toggle_pin_level(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : PORTA ^= (pin_num); break;
        case GPIO_PORT_B : PORTB ^= (pin_num); break;
        case GPIO_PORT_C : PORTC ^= (pin_num); break;
        case GPIO_PORT_D : PORTD ^= (pin_num); break;
        default: break;
    }
}

/**
 * @brief Gets the current pin level of the given port pin pair.
 * 
 * @param port port from which to get the current level from
 * @param pin_num pin from which to get the current level from
 * @return GPIO_LEVEL_HIGH if the current pin-level is high, GPIO_LEVEL_LOW otherwise
 */
static u8 gpio_driver_atmega1284p_get_pin_level(u8 port, u8 pin_num) {
    switch (port) {
        case GPIO_PORT_A : return (PINA & pin_num) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        case GPIO_PORT_B : return (PINB & pin_num) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        case GPIO_PORT_C : return (PINC & pin_num) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        case GPIO_PORT_D : return (PIND & pin_num) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
        default: return GPIO_LEVEL_LOW;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_init
 */
void gpio_driver_init(void) {

    DEBUG_PASS("gpio_driver_init()");
    
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_0));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_1));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_2));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_3));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_4));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_5));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_6));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_A, GPIO_PIN_7));
    
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_0));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_1));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_2));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_3));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_4));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_5));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_6));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_B, GPIO_PIN_7));
    
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_0));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_1));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_2));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_3));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_4));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_5));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_6));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_C, GPIO_PIN_7));
    
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_0));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_1));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_2));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_3));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_4));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_5));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_6));
    gpio_driver_init_pin(GET_GPIO_REFERENCE(GPIO_PORT_D, GPIO_PIN_7));
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_deinit
 */
void gpio_driver_deinit(void) {
    
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_init_pin
 */
void gpio_driver_init_pin(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    DEBUG_TRACE_byte(p_pin_descr->port_id, "gpio_driver_init_pin() - PORT");
    DEBUG_TRACE_byte(p_pin_descr->pin_id, "gpio_driver_init_pin() - PIN");

    if (GPIO_DRIVER_IS_OUTPUT(p_pin_descr) != 0) {
        DEBUG_PASS("gpio_driver_init_pin() - OUTPUT");
        gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_OUTPUT);
    } else {
        DEBUG_PASS("gpio_driver_init_pin() - INPUT");
        gpio_driver_set_direction(p_pin_descr, GPIO_DIRECTION_INPUT);
    }

    if (GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr) != 0) {
        DEBUG_PASS("gpio_driver_init_pin() - LOW");
        gpio_driver_set_level(p_pin_descr, GPIO_LEVEL_LOW);

    } else if (GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr) != 0) {
        DEBUG_PASS("gpio_driver_init_pin() - HIGH");
        gpio_driver_set_level(p_pin_descr, GPIO_LEVEL_HIGH);

    } else {
        DEBUG_PASS("gpio_driver_init_pin() - HIGH-Z");
        gpio_driver_set_level(p_pin_descr, GPIO_LEVEL_HIGH_Z);
    }
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_set_direction
 */
void gpio_driver_set_direction(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    DEBUG_TRACE_byte(p_pin_descr->port_id, "gpio_driver_set_direction() - PORT");
    DEBUG_TRACE_byte(p_pin_descr->pin_id, "gpio_driver_set_direction() - PIN");
    DEBUG_TRACE_byte(direction, "gpio_driver_set_direction() - DIRECTION");

    gpio_driver_atmega1284p_set_direction(
        p_pin_descr->port_id,
        gpio_driver_atmega1284p_get_pin_number(p_pin_descr->pin_id),
        direction
    );
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_get_level
 */
void gpio_driver_set_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    DEBUG_TRACE_byte(p_pin_descr->port_id, "gpio_driver_set_level() - PORT");
    DEBUG_TRACE_byte(p_pin_descr->pin_id, "gpio_driver_set_level() - PIN");
    DEBUG_TRACE_byte(level, "gpio_driver_set_level() - LEVEL");

    if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
        GPIO_DRIVER_INVERT_LEVEL(level);
        DEBUG_TRACE_byte(level, "gpio_driver_get_level() - LEVEL inverted");
    }

    gpio_driver_atmega1284p_set_pin_level(
        p_pin_descr->port_id,
        gpio_driver_atmega1284p_get_pin_number(p_pin_descr->pin_id),
        level
    );
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_toggle_level
 */
void gpio_driver_toggle_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {

    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return;
    }

    DEBUG_TRACE_byte(p_pin_descr->port_id, "gpio_driver_toggle_level() - PORT");
    DEBUG_TRACE_byte(p_pin_descr->pin_id, "gpio_driver_toggle_level() - PIN");

    gpio_driver_atmega1284p_toggle_pin_level(
        p_pin_descr->port_id,
        gpio_driver_atmega1284p_get_pin_number(p_pin_descr->pin_id)
    );
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_get_level
 */
GPIO_DRIVER_LEVEL gpio_driver_get_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    
    if (GPIO_DRIVER_PIN_IS_DEACTIVATED(p_pin_descr) != 0) {
        return GPIO_LEVEL_HIGH_Z;
    }

    u8 pin_level = gpio_driver_atmega1284p_get_pin_level(
        p_pin_descr->port_id,
        p_pin_descr->pin_id
    );

    if (GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr) != 0) {
        GPIO_DRIVER_INVERT_LEVEL(pin_level);
    }

    DEBUG_TRACE_byte(p_pin_descr->port_id, "gpio_driver_get_level() - PORT");
    DEBUG_TRACE_byte(p_pin_descr->pin_id, "gpio_driver_get_level() - PIN");
    DEBUG_TRACE_byte(pin_level, "gpio_driver_get_level() - LEVEL");

    return pin_level;
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_deactivate
 */
void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    p_pin_descr->pin_cfg |= GPIO_DEACTIVATE;
}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_activate
 */
void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    p_pin_descr->pin_cfg &= ~GPIO_DEACTIVATE;
    gpio_driver_init_pin(p_pin_descr);

}

// --------------------------------------------------------------------------------

/**
 * @see gpio_interface.h#gpio_driver_print_pin_state
 */
void gpio_driver_print_pin_state(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
}

// --------------------------------------------------------------------------------
