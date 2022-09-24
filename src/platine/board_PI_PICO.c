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
 * @file    board_PI_PICO.c
 * @author  Sebastian Lesse
 * @date    2022 / 08 / 01
 * @brief   Pin-Map :  Raspberry Pi Pico RP2040
 * 
 */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief See section 2.19 GPIO of the datasheet
 * 
 * Following functions are available, see section 2.19.2. Function Select for more information
 * 
 *  F1 - SPIn   RX / TX / CSn / SCK 
 *  F2 - UARTn  TX / RX / CTS
 *  F3 - I2Cn   SCL / SDA
 *  F4 - PWMn   A / B
 *  F5 - SIO
 *  F6 - PIO0
 *  F7 - PIO1
 * 
 */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT A
//---------     Name,           Port,           Bit,            Pin-Cfg                                                                FUNCTION

    BUILD_GPIO ( GPIO_00,       GPIO_PORT_A,    GPIO_PIN_0,     GPIO_FUNCTION_2                                                 ) //    UART0 - TX
    BUILD_GPIO ( GPIO_01,       GPIO_PORT_A,    GPIO_PIN_1,     GPIO_FUNCTION_2                                                 ) //    UART0 - RX
    BUILD_GPIO ( GPIO_02,       GPIO_PORT_A,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_03,       GPIO_PORT_A,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_04,       GPIO_PORT_A,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) // 
    BUILD_GPIO ( GPIO_05,       GPIO_PORT_A,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_06,       GPIO_PORT_A,    GPIO_PIN_6,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_07,       GPIO_PORT_A,    GPIO_PIN_7,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT B
//---------     Name,           Port,           Bit,            Pin-Cfg                                                                 FUNCTION

    BUILD_GPIO ( GPIO_08,       GPIO_PORT_B,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_09,       GPIO_PORT_B,    GPIO_PIN_1,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_10,       GPIO_PORT_B,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) // 
    BUILD_GPIO ( GPIO_11,       GPIO_PORT_B,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_12,       GPIO_PORT_B,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_13,       GPIO_PORT_B,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_14,       GPIO_PORT_B,    GPIO_PIN_6,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_15,       GPIO_PORT_B,    GPIO_PIN_7,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT C
//---------     Name,           Port,           Bit,            Pin-Cfg                                                                 FUNCTION

    BUILD_GPIO ( GPIO_16,       GPIO_PORT_C,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_17,       GPIO_PORT_C,    GPIO_PIN_1,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_18,       GPIO_PORT_C,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_19,       GPIO_PORT_C,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_20,       GPIO_PORT_C,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_21,       GPIO_PORT_C,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_22,       GPIO_PORT_C,    GPIO_PIN_6,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) // 
    BUILD_GPIO ( GPIO_23,       GPIO_PORT_C,    GPIO_PIN_7,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) // 

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// PORT D
//---------     Name,           Port,           Bit,            Pin-Cfg                                                                 FUNCTION

    BUILD_GPIO ( GPIO_24,       GPIO_PORT_D,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_25,       GPIO_PORT_D,    GPIO_PIN_1,     GPIO_OUTPUT | GPIO_IDLE_HIGH                                    ) //    ON_BOARD LED
    BUILD_GPIO ( GPIO_26,       GPIO_PORT_D,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_27,       GPIO_PORT_D,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //
    BUILD_GPIO ( GPIO_28,       GPIO_PORT_D,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH                                     ) //

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
