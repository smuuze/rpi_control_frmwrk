/**
 * @file 	unittest_json_parser.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-06
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "initialization.h"

#include "common/common_tools_string.h"
#include "ui/command_line/command_line_interface.h"
#include "common/signal_slot_interface.h"

#include "ui/command_line/command_line_handler_gpio.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_GPIO_READ_ALL                      1

// --------------------------------------------------------------------------------

// counters

static u8 counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED = 0;
static u8 counter_CLI_INVALID_PARAMETER_SIGNAL = 0;

static void unittest_reset_counter(void) {

	counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED = 0;
    counter_CLI_INVALID_PARAMETER_SIGNAL = 0;
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots callbacks

/**
 * @brief Command received by signal CLI_ARGUMENT_GPIO_SIGNAL
 * 
 */
static GPIO_CLI_ARGUMENT gpio_command;

static void ut_CLI_ARGUMENT_GPIO_SIGNAL_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("ut_CLI_ARGUMENT_GPIO_SIGNAL_CALLBACK() - argument is NULL");
        return;
    }

    const GPIO_CLI_ARGUMENT* p_cmd = (const GPIO_CLI_ARGUMENT*)p_argument;

    gpio_command.operation = p_cmd->operation;
    gpio_command.pin = p_cmd->pin;
    gpio_command.mode = p_cmd->mode;
    gpio_command.level = p_cmd->level;

    counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED += 1;

    DEBUG_PASS("ut_CLI_ARGUMENT_GPIO_SIGNAL_CALLBACK()");
}

static void ut_CLI_INVALID_PARAMETER_SIGNAL_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("ut_CLI_INVALID_PARAMETER_SIGNAL_CALLBACK() - argument is NULL");
        return;
    }
    
	counter_CLI_INVALID_PARAMETER_SIGNAL += 1;

	DEBUG_PASS("ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK()");
}

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_GPIO_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_ARGUMENT_GPIO_SIGNAL, UT_CLI_ARGUMENT_GPIO_SLOT, ut_CLI_ARGUMENT_GPIO_SIGNAL_CALLBACK)
void CLI_ARGUMENT_GPIO_SIGNAL_set_timeout(u16 timeout_ms);

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, CLI_INVALID_PARAMETER_SLOT, ut_CLI_INVALID_PARAMETER_SIGNAL_CALLBACK)
void CLI_INVALID_PARAMETER_SIGNAL_set_timeout(u16 timeout_ms);

// --------------------------------------------------------------------------------

static void TEST_CASE_read_all_lower_case(void) {

	UT_START_TEST_CASE("GPIO READ ALL (lower case)")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));


        char* argv[] = { "program", "-gpio", "read:all" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_READ);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_ALL);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_all_upper_case(void) {

	UT_START_TEST_CASE("GPIO READ ALL (upper)")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "READ:ALL" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_READ);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_ALL);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_write_10_out_high(void) {

	UT_START_TEST_CASE("GPIO WRITE 10 OUT HIGH")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:10:out:high" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_WRITE);
        UT_CHECK_IS_EQUAL(gpio_command.pin, 10);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_OUT);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_HIGH);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_write_9_out_low(void) {

	UT_START_TEST_CASE("GPIO WRITE 9 OUT LOW")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "WRITE:9:OUT:low" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_WRITE);
        UT_CHECK_IS_EQUAL(gpio_command.pin, 9);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_OUT);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_LOW);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_write_2_in_low(void) {

	UT_START_TEST_CASE("GPIO WRITE 2 OUT LOW")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:2:in:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_WRITE);
        UT_CHECK_IS_EQUAL(gpio_command.pin, 2);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_IN);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_LOW);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_read_11(void) {

	UT_START_TEST_CASE("GPIO READ 11")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "read:10" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_READ);
        UT_CHECK_IS_EQUAL(gpio_command.pin, 10);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_pin_with_leading_zero(void) {

	UT_START_TEST_CASE("GPIO INVALID PIN")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:000000000000107:in:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_WRITE);
        UT_CHECK_IS_EQUAL(gpio_command.pin, 107);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_IN);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_LOW);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_operation_write(void) {

	UT_START_TEST_CASE("GPIO INVALID OPERATION WRITE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "writ:2:in:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_operation_read(void) {

	UT_START_TEST_CASE("GPIO INVALID OPERATION READ")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "reat:2" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_mode_out(void) {

	UT_START_TEST_CASE("GPIO INVALID MODE OUT")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:2:OUUT:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_mode_level(void) {

	UT_START_TEST_CASE("GPIO INVALID LEVEL")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:2:OUT:LO" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_no_seperator(void) {

	UT_START_TEST_CASE("GPIO INVALID NO SEPERATOR")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write2OUTLOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_pin_1(void) {

	UT_START_TEST_CASE("GPIO INVALID PIN 1")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:1x0:in:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_invalid_pin_2(void) {

	UT_START_TEST_CASE("GPIO INVALID PIN 2")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:10x:in:LOW" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_only_seperator_chars(void) {

	UT_START_TEST_CASE("GPIO INVALID PIN")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", ":::" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_missing_level(void) {

	UT_START_TEST_CASE("GPIO MISSING LEVEL")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:10:in" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_missing_direction(void) {

	UT_START_TEST_CASE("GPIO MISSING DIRECTION")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_GPIO_READ_ALL);

        unittest_reset_counter();
        memset(&gpio_command, 0xFF, sizeof(GPIO_CLI_ARGUMENT));

        char* argv[] = { "program", "-gpio", "write:10:high" };
        command_line_interface(3, argv);

        UT_CHECK_IS_EQUAL(gpio_command.operation, CLI_HANDLER_OP_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.pin, CLI_HANDLER_GPIO_PIN_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.mode, CLI_HANDLER_GPIO_DIRECTION_INVALID);
        UT_CHECK_IS_EQUAL(gpio_command.level, CLI_HANDLER_GPIO_LEVEL_INVALID);

		UT_CHECK_IS_EQUAL(counter_CLI_ARGUMENT_GPIO_SIGNAL_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    TRACER_RESTART(); 
    TRACER_ENABLE();

    initialization();

	CLI_ARGUMENT_GPIO_SIGNAL_set_timeout(0);
	UT_CLI_ARGUMENT_GPIO_SLOT_connect();

    CLI_INVALID_PARAMETER_SIGNAL_set_timeout(0);
	CLI_INVALID_PARAMETER_SLOT_connect();

	UT_START_TESTBENCH("Welcome the the UNITTEST CLI-HANDLER GPIO 1.0")
	{
		TEST_CASE_read_all_lower_case();
        TEST_CASE_read_all_upper_case();
        TEST_CASE_write_10_out_high();
        TEST_CASE_write_9_out_low();
        TEST_CASE_write_2_in_low();

        TEST_CASE_read_11();

        TEST_CASE_invalid_operation_write();
        TEST_CASE_invalid_operation_read();
        TEST_CASE_invalid_mode_out();
        TEST_CASE_invalid_pin_with_leading_zero();
        TEST_CASE_invalid_mode_level();
        TEST_CASE_invalid_no_seperator();
        TEST_CASE_invalid_pin_2();
        TEST_CASE_invalid_pin_2();
        TEST_CASE_only_seperator_chars();
        TEST_CASE_missing_level();
        TEST_CASE_missing_direction();
        TEST_CASE_invalid_pin_1();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
