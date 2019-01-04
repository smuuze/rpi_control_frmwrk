/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "system_interface.h"

#define noTRACES
#include <traces.h>

/*
 * How to set Port-Pins on a ATMega 128 4P
 * 
 * DDR
 */

#define SET_PIN_AS_INPUT(ddr_reg, pin)                              ddr_reg &= ~(pin)
#define SET_PIN_AS_OUTPUT(ddr_reg, pin)                             ddr_reg |= (pin)
#define GPIO_DRIVER_SET_PIN_DIRECTION(ddr_reg, pin, direction)      if (direction == GPIO_INPUT) {                  \
                                                                        SET_PIN_AS_INPUT(ddr_reg, pin);             \
                                                                    } else {                                        \
                                                                        SET_PIN_AS_OUTPUT(ddr_reg, pin);            \
                                                                    }                                               

#define SET_PIN_HIGH_LEVEL(port_reg, pin)                           port_reg |= (pin)
#define SET_PIN_LOW_LEVEL(port_reg, pin)                            port_reg &= ~(pin)
#define SET_PIN_HIGH_Z(ddr_reg, port_reg, pin)                      port_reg &= ~(pin); ddr_reg &= ~(pin)
#define GPIO_DRIVER_SET_PIN_LEVEL(ddr_reg, port_reg, pin, level)    if (level == GPIO_LEVEL_HIGH) {                 \
                                                                        SET_PIN_HIGH_LEVEL(port_reg, pin);          \
                                                                    } else if (level == GPIO_LEVEL_LOW) {           \
                                                                        SET_PIN_LOW_LEVEL(port_reg, pin);           \
                                                                    } else {                                        \
                                                                        SET_PIN_HIGH_Z(ddr_reg, port_reg, pin);     \
                                                                    }

#define GPIO_DRIVER_GET_PIN_LEVEL(pin_reg, pin)                    (pin_reg & pin) != 0 ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW

#define GPIO_DRIVER_GET_PORT(pin_id)                                (pin_id & 0xF0)
#define GPIO_DRIVER_GET_PIN(pin_id)                                 (pin_id & 0x0F)

#define GPIO_DRIVER_PORT_A                                          0x10
#define GPIO_DRIVER_PORT_B                                          0x20
#define GPIO_DRIVER_PORT_D                                          0x30
#define GPIO_DRIVER_PORT_C                                          0x40

void gpio_driver_set_direction(u8 pin_id, SYSTEM_INTERFACE_GPIO_DIRECTION direction) {

    u8 port_id = GPIO_DRIVER_GET_PORT(pin_id);
    u8 pin_num = GPIO_DRIVER_GET_PIN(pin_id));

    switch (port_id) {
        case GPIO_DRIVER_PORT_A :   GPIO_DRIVER_SET_PIN_DIRECTION(DDRA, pin_num, direction); break;
        case GPIO_DRIVER_PORT_B :   GPIO_DRIVER_SET_PIN_DIRECTION(DDRB, pin_num, direction); break;
        case GPIO_DRIVER_PORT_C :   GPIO_DRIVER_SET_PIN_DIRECTION(DDRC, pin_num, direction); break;
        case GPIO_DRIVER_PORT_D :   GPIO_DRIVER_SET_PIN_DIRECTION(DDRD, pin_num, direction); break;
        default : break;
    }
}

void gpio_driver_set_level(u8 pin_id, SYSTEM_INTERFACE_IO_PIN_LEVEL level) {

    u8 port_id = GPIO_DRIVER_GET_PORT(pin_id);
    u8 pin_num = GPIO_DRIVER_GET_PIN(pin_id));

    switch (port_id) {
        case GPIO_DRIVER_PORT_A :   GPIO_DRIVER_SET_PIN_LEVEL(DDRA, PORTA, pin_num, level); break;
        case GPIO_DRIVER_PORT_B :   GPIO_DRIVER_SET_PIN_LEVEL(DDRB, PORTA, pin_num, level); break;
        case GPIO_DRIVER_PORT_C :   GPIO_DRIVER_SET_PIN_LEVEL(DDRC, PORTA, pin_num, level); break;
        case GPIO_DRIVER_PORT_D :   GPIO_DRIVER_SET_PIN_LEVEL(DDRD, PORTA, pin_num, level); break;
        default : break;
    }
}

SYSTEM_INTERFACE_IO_PIN_LEVEL gpio_driver_get_level(u8 pin_num) {

    u8 port_id = GPIO_DRIVER_GET_PORT(pin_id);
    u8 pin_num = GPIO_DRIVER_GET_PIN(pin_id));

    switch (port_id) {
        case GPIO_DRIVER_PORT_A :   return GPIO_DRIVER_GET_PIN_LEVEL(PINA, pin_num);
        case GPIO_DRIVER_PORT_B :   return GPIO_DRIVER_GET_PIN_LEVEL(PINB, pin_num);
        case GPIO_DRIVER_PORT_C :   return GPIO_DRIVER_GET_PIN_LEVEL(PINC, pin_num);
        case GPIO_DRIVER_PORT_D :   return GPIO_DRIVER_GET_PIN_LEVEL(PIND, pin_num);
        default :                   return GPIO_LEVEL_LOW;
    }
}
