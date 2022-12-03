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
 * @file    unittest_gpio_driver_atmega1284p.c
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 28
 * @brief   Short description of this file
 * 
 */

#define TRACER_ON

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

#include "../unittest.h"

// --------------------------------------------------------------------------------

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION             0
#define TEST_CASE_ID_SET_OUTPUT_LOW             1
#define TEST_CASE_ID_CHANGE_STATES              2
#define TEST_CASE_ID_TOGGLE_LEVEL               3

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS    100

// --------------------------------------------------------------------------------

u8 DDRA = 0;
u8 DDRB = 0;
u8 DDRC = 0;
u8 DDRD = 0;

u8 PORTA = 0;
u8 PORTB = 0;
u8 PORTC = 0;
u8 PORTD = 0;

u8 PINA = 0;
u8 PINB = 0;
u8 PINC = 0;
u8 PIND = 0;

// --------------------------------------------------------------------------------

static u8 counter_GET_SIO_REG = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

    counter_GET_SIO_REG = 0;

    DDRA = 0;
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;

    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    PINA = 0;
    PINB = 0;
    PINC = 0;
    PIND = 0;
}

// --------------------------------------------------------------------------------

// stubs

u16 time_mgmnt_gettime_u16(void) {
    return 0;
}

u8 time_mgmnt_istimeup_raw_u16(u16 time_reference, u16 time_interval) {
    return 0;
}

// --------------------------------------------------------------------------------
// PORT A
//---------     Name,           Port,           Bit,            Pin-Cfg      FUNCTION

    BUILD_GPIO ( GPIO_00,       GPIO_PORT_A,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_01,       GPIO_PORT_A,    GPIO_PIN_1,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_02,       GPIO_PORT_A,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_03,       GPIO_PORT_A,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_04,       GPIO_PORT_A,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH ) // 
    BUILD_GPIO ( GPIO_05,       GPIO_PORT_A,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_06,       GPIO_PORT_A,    GPIO_PIN_6,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_07,       GPIO_PORT_A,    GPIO_PIN_7,     GPIO_INPUT | GPIO_IDLE_HIGH ) //

// --------------------------------------------------------------------------------
// PORT B
//---------     Name,           Port,           Bit,            Pin-Cfg      FUNCTION

    BUILD_GPIO ( GPIO_08,       GPIO_PORT_B,    GPIO_PIN_0,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_09,       GPIO_PORT_B,    GPIO_PIN_1,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_10,       GPIO_PORT_B,    GPIO_PIN_2,     GPIO_OUTPUT | GPIO_IDLE_LOW ) // 
    BUILD_GPIO ( GPIO_11,       GPIO_PORT_B,    GPIO_PIN_3,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_12,       GPIO_PORT_B,    GPIO_PIN_4,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_13,       GPIO_PORT_B,    GPIO_PIN_5,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_14,       GPIO_PORT_B,    GPIO_PIN_6,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_15,       GPIO_PORT_B,    GPIO_PIN_7,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //

// --------------------------------------------------------------------------------
// PORT C
//---------     Name,           Port,           Bit,            Pin-Cfg      FUNCTION

    BUILD_GPIO ( GPIO_16,       GPIO_PORT_C,    GPIO_PIN_0,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) // 
    BUILD_GPIO ( GPIO_17,       GPIO_PORT_C,    GPIO_PIN_1,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_18,       GPIO_PORT_C,    GPIO_PIN_2,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_19,       GPIO_PORT_C,    GPIO_PIN_3,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_20,       GPIO_PORT_C,    GPIO_PIN_4,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_21,       GPIO_PORT_C,    GPIO_PIN_5,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_22,       GPIO_PORT_C,    GPIO_PIN_6,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) // 
    BUILD_GPIO ( GPIO_23,       GPIO_PORT_C,    GPIO_PIN_7,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) // 

// --------------------------------------------------------------------------------
// PORT D
//---------     Name,           Port,           Bit,            Pin-Cfg      FUNCTION

    BUILD_GPIO ( GPIO_24,       GPIO_PORT_D,    GPIO_PIN_0,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_25,       GPIO_PORT_D,    GPIO_PIN_1,     GPIO_OUTPUT | GPIO_IDLE_HIGH ) //
    BUILD_GPIO ( GPIO_26,       GPIO_PORT_D,    GPIO_PIN_2,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_27,       GPIO_PORT_D,    GPIO_PIN_3,     GPIO_OUTPUT | GPIO_IDLE_LOW ) //
    BUILD_GPIO ( GPIO_28,       GPIO_PORT_D,    GPIO_PIN_4,     GPIO_INPUT  | GPIO_IDLE_HIGH_Z ) //
    BUILD_GPIO ( GPIO_29,       GPIO_PORT_D,    GPIO_PIN_5,     GPIO_INPUT  | GPIO_IDLE_HIGH_Z ) //
    BUILD_GPIO ( GPIO_30,       GPIO_PORT_D,    GPIO_PIN_6,     GPIO_INPUT  | GPIO_IDLE_LOW ) // 
    BUILD_GPIO ( GPIO_31,       GPIO_PORT_D,    GPIO_PIN_7,     GPIO_INPUT  | GPIO_IDLE_LOW ) // 

// --------------------------------------------------------------------------------

// slots callbacksstatic IR_COMMON_COMMAND sony_ir_command;

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

/**
 * @brief Initialize all GPIOS
 * Check if the values taht have been written to the cpu-regsiters match the expected values.
 * 
 */
static void TEST_CASE_initialization(void) {

    UT_START_TEST_CASE("Initialization")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
        unittest_reset_counter();

        // Check PIN A0 - A7
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);

        // Check PIN B0 - B7
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);

        // Check PIN C0 - C7
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);

        // Check PIN D0 - D7
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        initialization();

        // Check PIN A0 - A7
        UT_CHECK_IS_EQUAL(PORTA, BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1));
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);

        // Check PIN B0 - B7
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1));
        UT_CHECK_IS_EQUAL(PINB,  0);

        // Check PIN C0 - C7
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1));
        UT_CHECK_IS_EQUAL(PINC,  0);

        // Check PIN D0 - D7
        UT_CHECK_IS_EQUAL(PORTD, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,1,1));
        UT_CHECK_IS_EQUAL(DDRD,  BUILD_BIT_VECTOR_U8(0,0,0,0,1,1,1,1));
        UT_CHECK_IS_EQUAL(PIND,  0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Sets all gpios as outputs with low-level, except those who have a non-GPIO function
 * 
 */
static void TEST_CASE_set_as_output_low(void) {

    UT_START_TEST_CASE("Set Output Low")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_SET_OUTPUT_LOW);
        unittest_reset_counter();

        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_00_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRA,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1));
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);
        
        unittest_reset_counter();

        GPIO_08_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);
        
        unittest_reset_counter();

        GPIO_16_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1));
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);
        
        unittest_reset_counter();

        GPIO_24_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRD,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1));
        UT_CHECK_IS_EQUAL(PIND,  0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Sets all different states for one GPIO
 * 
 */
static void TEST_CASE_change_states(void) {

    UT_START_TEST_CASE("Change States")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_CHANGE_STATES);
        unittest_reset_counter();

        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_20_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_20_drive_high();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_20_pull_up();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_20_no_pull();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRC,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Toggles the current level
 * 
 */
static void TEST_CASE_toggle_level(void) {

    UT_START_TEST_CASE("Toggle Level")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_TOGGLE_LEVEL);
        unittest_reset_counter();

        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, 0);
        UT_CHECK_IS_EQUAL(DDRB,  0);
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_drive_low();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_toggle_level();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_toggle_level();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_no_pull();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_toggle_level();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

        GPIO_10_toggle_level();
        UT_CHECK_IS_EQUAL(PORTA, 0);
        UT_CHECK_IS_EQUAL(DDRA,  0);
        UT_CHECK_IS_EQUAL(PINA,  0);
        UT_CHECK_IS_EQUAL(PORTB, BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(DDRB,  BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(PINB,  0);
        UT_CHECK_IS_EQUAL(PORTC, 0);
        UT_CHECK_IS_EQUAL(DDRC,  0);
        UT_CHECK_IS_EQUAL(PINC,  0);
        UT_CHECK_IS_EQUAL(PORTD, 0);
        UT_CHECK_IS_EQUAL(DDRD,  0);
        UT_CHECK_IS_EQUAL(PIND,  0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome the the UNITTEST for ATMega1284p GPIO-driver 1.0")
    {
        TEST_CASE_initialization();
        TEST_CASE_set_as_output_low();
        TEST_CASE_change_states();
        TEST_CASE_toggle_level();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
