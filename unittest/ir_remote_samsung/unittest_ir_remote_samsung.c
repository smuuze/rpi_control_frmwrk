/**
 * @file     unittest_ir_remote_samsung.c
 * @author     sebastian lesse (sebastian lesse)
 * @brief 
 * @version     1.0
 * @date     2022 / 12 / 24
 * 
 * 
 */

#define TRACER_ON

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "driver/timer/timer_configuration.h"
#include "initialization/initialization.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_task.h"
#include "modules/ir/ir_protocol_interface.h"

#include "modules/ir/ir_protocol_rpi_cmd_handler.h"
#include "modules/ir/ir_protocol_samsung.h"
#include "modules/ir/ir_protocol_samsung_rpi_cmd_handler.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION                         0
#define TEST_CASE_ID_BUILD_IR_COMMANDS                      1
#define TEST_CASE_ID_DEVICE_OPERATION                       2
#define TEST_CASE_ID_INVALID_DEVICE_OPERATION               3
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND                    4
#define TEST_CASE_ID_CANCEL_IR_COMMAND                      5
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_POWER_ON           6
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_HOME               7

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS                                100

// --------------------------------------------------------------------------------

IR_COMMON_COMMAND_TYPE ir_command_received_via_signal;
TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK p_irq_callback;

// --------------------------------------------------------------------------------

static u8 counter_SLOT_SAMSUNG_IR_CMD_RECEIVED = 0;
static u8 counter_IR_MOD_OUT_DRIVE_LOW = 0;
static u8 counter_IR_MOD_OUT_DRIVE_HIGH = 0;
static u8 counter_TIMER0_START = 0;
static u8 counter_TIMER0_STOP = 0;
static u8 counter_TIMER1_START = 0;
static u8 counter_TIMER1_STOP = 0;
static u16 counter_IR_MOD_SEQUENCE = 0;

static u8 array_IR_MOD_SEQUENCE[2048];

static void unittest_reset_counter(void) {

    counter_SLOT_SAMSUNG_IR_CMD_RECEIVED = 0;
    counter_IR_MOD_OUT_DRIVE_LOW = 0;
    counter_IR_MOD_OUT_DRIVE_HIGH = 0;
    counter_TIMER0_START = 0;
    counter_TIMER0_STOP = 0;
    counter_TIMER1_START = 0;
    counter_TIMER1_STOP = 0;
    counter_IR_MOD_SEQUENCE = 0;

    memset(
        array_IR_MOD_SEQUENCE,
        0xFF,
        sizeof(array_IR_MOD_SEQUENCE)
    );

    memset(
        (u8*)&ir_command_received_via_signal,
        IR_COMMON_COMMAND_DATA_INVALID,
        sizeof(ir_command_received_via_signal)
    );
}

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

static void unittest_wait_ms(u16 waittime_ms) {

    TIMEOUT_TIMER_start();
    while (TIMEOUT_TIMER_is_up(waittime_ms) == 0) {

    }
}

// --------------------------------------------------------------------------------

// stubs


void IR_MOD_OUT_drive_low(void) {
    //DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_low()");
    counter_IR_MOD_OUT_DRIVE_LOW += 1;

    if (counter_IR_MOD_SEQUENCE < sizeof(array_IR_MOD_SEQUENCE) - 1) {
        array_IR_MOD_SEQUENCE[counter_IR_MOD_SEQUENCE++] = 0;
    }
}

void IR_MOD_OUT_drive_high(void) {
    //DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_high()");
    counter_IR_MOD_OUT_DRIVE_HIGH += 1;

    if (counter_IR_MOD_SEQUENCE < sizeof(array_IR_MOD_SEQUENCE) - 1) {
        array_IR_MOD_SEQUENCE[counter_IR_MOD_SEQUENCE++] = 1;
    }
}

void timer1_driver_init(void) {
    
}

void timer1_driver_deinit(void) {
    
}

void timer1_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {

    if (p_configuration->irq_callback != NULL) {
        DEBUG_PASS("UNITTEST - timer1_driver_configure() - callback != NULL");
        p_irq_callback = p_configuration->irq_callback;
    }
}

void timer1_driver_start(u32 time_us) {
    DEBUG_PASS("UNITTEST - timer1_driver_start()");
    counter_TIMER1_START += 1;
}

void timer1_driver_stop(void) {
    //DEBUG_PASS("UNITTEST - timer1_driver_stop()");
    counter_TIMER1_STOP += 1;
}

void timer0_driver_init(void) {
    
}

void timer0_driver_deinit(void) {
    
}
void timer0_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {

    if (p_configuration->irq_callback != NULL) {
        DEBUG_PASS("UNITTEST - timer0_driver_configure() - callback != NULL");
    }
}
void timer0_driver_start(u32 time_us) {
    DEBUG_PASS("UNITTEST - timer0_driver_start()");
    counter_TIMER0_START += 1;
    
}
void timer0_driver_stop(void) {
    //DEBUG_PASS("UNITTEST - timer0_driver_stop()");
    counter_TIMER0_STOP += 1;
}

// --------------------------------------------------------------------------------

// slots callbacksstatic IR_COMMON_COMMAND samsung_ir_command;

static void unittest_slot_IR_CMD_RECEIVED(const void* p_arg) {

    DEBUG_PASS("unittest_slot_IR_CMD_RECEIVED()");

    counter_SLOT_SAMSUNG_IR_CMD_RECEIVED += 1;

    const IR_COMMON_COMMAND_TYPE* p_command = (const IR_COMMON_COMMAND_TYPE*) p_arg;
    ir_command_received_via_signal.type = p_command->type;
    ir_command_received_via_signal.data_1  = p_command->data_1;
    ir_command_received_via_signal.data_2   = p_command->data_2;
    ir_command_received_via_signal.data_3 = p_command->data_3;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(IR_CMD_RECEIVED_SIGNAL, UT_SAMSUNG_IR_CMD_RECEIVED_SLOT, unittest_slot_IR_CMD_RECEIVED)

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SAMSUNG_IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

static void TEST_CASE_initialization(void) {

    UT_START_TEST_CASE("Initialization")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
        unittest_reset_counter();

        initialization();
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_ir_command_build(void) {

    UT_START_TEST_CASE("Build IR-Commands")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_BUILD_IR_COMMANDS);
        unittest_reset_counter();

        IR_COMMON_COMMAND_TYPE ir_command_samsung;

        ir_protocol_samsung_address_tv(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_power(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_volume_up(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_volume_down(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_volume_mute(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_up(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,1,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_down(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,0,0,1,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_0(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,1,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_1(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,1,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_2(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,1,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_3(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_4(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_5(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,0,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_6(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_7(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_8(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,1,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_channel_9(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,1,1,0,0,0,0));

        ir_protocol_samsung_cmd_tv_enter(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,0,1,1,0));

        ir_protocol_samsung_cmd_tv_exit(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,1,1,0,1,0,0));

        ir_protocol_samsung_cmd_tv_return(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,1,0,1,0));

        ir_protocol_samsung_cmd_tv_arrow_up(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,1,0));

        ir_protocol_samsung_cmd_tv_arrow_down(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,0,1,1,0));

        ir_protocol_samsung_cmd_tv_arrow_left(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,1,0,0,1,1,0));

        ir_protocol_samsung_cmd_tv_arrow_right(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,0,1,1,0));

        ir_protocol_samsung_cmd_tv_play(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,1,0));

        ir_protocol_samsung_cmd_tv_pause(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,1,0));

        ir_protocol_samsung_cmd_tv_stop(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,1,0));

        ir_protocol_samsung_cmd_tv_program_guide(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,1,0));

        ir_protocol_samsung_cmd_tv_home(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,0,1,1,1,1,0));

        ir_protocol_samsung_cmd_tv_channel_list(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,1,1,0));

        ir_protocol_samsung_cmd_tv_source(&ir_command_samsung);
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_samsung.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0));
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_ir_device_operation(void) {

    UT_START_TEST_CASE("Device Operation")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_DEVICE_OPERATION);

        unittest_reset_counter();
        u8 err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_POWER_ON);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_VOLUME_UP);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_VOLUME_DOWN);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_MUTE);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_UP);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,1,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_DOWN);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,0,0,1,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_0);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,1,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_1);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,1,0,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_2);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,1,0,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_3);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_4);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_5);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_6);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_7);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_8);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,1,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_9);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,1,1,0,0,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_ENTER);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_EXIT);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,1,1,0,1,0,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_RETURN);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,0,1,1,0,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_ARROW_UP);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_ARROW_DOWN);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_ARROW_LEFT);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,1,0,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_ARROW_RIGHT);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_PLAY);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_PAUSE);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_STOP);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_PROGRAM_GUIDE);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_HOME);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,1,1,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_CHANNEL_LIST);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,1,0,1,0,1,1,0));

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_SOURCE);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_SAMSUNG);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0));
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_ir_invalid_device_operation(void) {

    UT_START_TEST_CASE("Invalid Device Operation")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INVALID_DEVICE_OPERATION);

        unittest_reset_counter();
        u8 err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_EJECT);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 0);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, IR_COMMON_COMMAND_DATA_INVALID);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, IR_COMMON_COMMAND_DATA_INVALID);

        unittest_reset_counter();
        err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_PROGRAM_GUIDE);
        unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
        
        UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 0);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, IR_COMMON_COMMAND_DATA_INVALID);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, IR_COMMON_COMMAND_DATA_INVALID);
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_ir_cancel_ir_command(void) {

    UT_START_TEST_CASE("Cancel IR-Command")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_CANCEL_IR_COMMAND);

        unittest_reset_counter();

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(5000) == 0) {
            mcu_task_controller_schedule();

            if (p_irq_callback != NULL) {
                p_irq_callback();
            }
        }

        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, IR_COMMON_COMMAND_DATA_INVALID);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, IR_COMMON_COMMAND_DATA_INVALID);
    }
    UT_END_TEST_CASE()
}

static void TEST_CASE_ir_transmit_ir_command_power_on(void) {

    UT_START_TEST_CASE("Transmit IR-Command Power On")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_POWER_ON);

        unittest_reset_counter();
        u8 err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_POWER_ON);

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(600) == 0) {

            if (p_irq_callback == 0) {
                DEBUG_PASS("TEST_CASE_ir_transmit_ir_command_power_on() - p_irq_callback is NULL");
                break;
            }

            mcu_task_controller_schedule();

            p_irq_callback();
        }

        u8 compare_MOD_ARRAY[] = {
            0x00 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01  ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01,
            0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01,
            0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01,
            0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01,
            0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00
        };

        u16 expected_array_length = sizeof(compare_MOD_ARRAY);

        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(0,1,0,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, sizeof(compare_MOD_ARRAY));
        UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, expected_array_length);
    }
    UT_END_TEST_CASE()

    UNITTEST_TIMER_start();

    while (UNITTEST_TIMER_is_up(750) == 0) {
        // wait for signal timeout
    }
}

static void TEST_CASE_ir_transmit_ir_command_home(void) {

    UT_START_TEST_CASE("Transmit IR-Command Home")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_HOME);

        unittest_reset_counter();
        u8 err_code = rpi_cmd_handler_ir_remote_samsung(IR_DEVICE_TV, IR_COMMAND_HOME);

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(500) == 0) {

            if (p_irq_callback == 0) {
                break;
            }

            mcu_task_controller_schedule();

            p_irq_callback();
        }

        u8 compare_MOD_ARRAY[] = {
            0x00 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01  ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01,
            0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01,
            0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00,
            0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01  ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00
        };
        
        u16 expected_array_length = sizeof(compare_MOD_ARRAY);

        UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
        UT_CHECK_IS_EQUAL(counter_SLOT_SAMSUNG_IR_CMD_RECEIVED, 1);
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0));
        UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, BUILD_BIT_VECTOR_U8(1,0,0,1,1,1,1,0));
        UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, sizeof(compare_MOD_ARRAY));
        UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, expected_array_length);
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    TRACER_RESTART();

    UT_START_TESTBENCH("Welcome the the UNITTEST for IR-Remote Samsung 1.0")
    {
        TEST_CASE_initialization();

        UT_SAMSUNG_IR_CMD_RECEIVED_SLOT_connect();
        TEST_CASE_ir_command_build();
        TEST_CASE_ir_device_operation();
        TEST_CASE_ir_invalid_device_operation();
        TEST_CASE_ir_cancel_ir_command();
        TEST_CASE_ir_transmit_ir_command_power_on();
        TEST_CASE_ir_transmit_ir_command_home();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}
