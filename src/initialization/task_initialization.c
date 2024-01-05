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
 * @file    task_initialization.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
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

#include "mcu_task_controller.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

#include "local_event_task.h"
#include "io_management/io_input_controller.h"
#include "io_management/io_output_controller.h"

// --------------------------------------------------------------------------------

#ifndef config_HAS_LED_MATRIX
#define config_HAS_LED_MATRIX 0
#endif

// --------------------------------------------------------------------------------

#if config_HAS_LED_MATRIX == 1
#include "local_led_mcu_task.h"
static MCU_TASK_INTERFACE led_mcu_task = {

    0,                         // u8 identifier,
    0,                         // u16 new_run_timeout,
    0,                         // u16 last_run_time,
    &local_led_mcu_task_init,             // MCU_TASK_INTERFACE_INIT_CALLBACK        init,
    &local_led_mcu_task_is_runable,         // MCU_TASK_INTERFACE_GET_STATE_CALLBACK    get_sate,
    &local_led_mcu_task_run,             // MCU_TASK_INTERFACE_RUN_CALLBACK        run,
    0,                        // MCU_TASK_INTERFACE_BG_RUN_CALLBACK        background_run,
    0,                         // MCU_TASK_INTERFACE_SLEEP_CALLBACK        sleep,
    0,                         // MCU_TASK_INTERFACE_WAKEUP_CALLBACK        wakeup,
    &local_led_mcu_task_finish,             // MCU_TASK_INTERFACE_FINISH_CALLBACK        finish,
    &local_led_mcu_task_terminate,             // MCU_TASK_INTERFACE_TERMINATE_CALLBACK    terminate,
    0                        // next-task
};
#endif

// --------------------------------------------------------------------------------

#ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115
#include "local_ads1115_mcu_task.h"
static MCU_TASK_INTERFACE ads1115_mcu_task = {

    0,                         // u8 identifier,
    0,                         // u16 new_run_timeout,
    0,                         // u16 last_run_time,
    &local_ads1115_mcu_task_init,             // MCU_TASK_INTERFACE_INIT_CALLBACK            init,
    &local_ads1115_mcu_task_get_schedule_interval,     // MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK    get_schedule_interval,
    &local_ads1115_mcu_task_get_state,         // MCU_TASK_INTERFACE_GET_STATE_CALLBACK        get_sate,
    &local_ads1115_mcu_task_run,             // MCU_TASK_INTERFACE_RUN_CALLBACK            run,
    0,                        // MCU_TASK_INTERFACE_BG_RUN_CALLBACK            background_run,
    0,                         // MCU_TASK_INTERFACE_SLEEP_CALLBACK            sleep,
    0,                         // MCU_TASK_INTERFACE_WAKEUP_CALLBACK            wakeup,
    &local_ads1115_mcu_task_finish,         // MCU_TASK_INTERFACE_FINISH_CALLBACK            finish,
    &local_ads1115_mcu_task_terminate,         // MCU_TASK_INTERFACE_TERMINATE_CALLBACK        terminate,
    0                        // next-task
};
#endif

// --------------------------------------------------------------------------------

#if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1
#include "test_tracer_mcu_task.h"
static MCU_TASK_INTERFACE test_tracer_task = {

    0,                         // u8 identifier,
    0,                         // u16 new_run_timeout,
    0,                         // u16 last_run_time,
    &test_tracer_task_init,             // MCU_TASK_INTERFACE_INIT_CALLBACK            init,
    &test_tracer_task_get_schedule_interval,    // MCU_TASK_INTERFACE_INIT_CALLBACK            get_schedule_interval,
    &test_tracer_task_get_state,             // MCU_TASK_INTERFACE_GET_STATE_CALLBACK        get_sate,
    &test_tracer_task_run,                 // MCU_TASK_INTERFACE_RUN_CALLBACK            run,
    &test_tracer_task_background_run,        // MCU_TASK_INTERFACE_BG_RUN_CALLBACK            background_run,
    0,                         // MCU_TASK_INTERFACE_SLEEP_CALLBACK            sleep,
    0,                         // MCU_TASK_INTERFACE_WAKEUP_CALLBACK            wakeup,
    0,                         // MCU_TASK_INTERFACE_FINISH_CALLBACK            finish,
    0,                         // MCU_TASK_INTERFACE_TERMINATE_CALLBACK        terminate,
    0                        // next-task
};
#endif

// --------------------------------------------------------------------------------

void task_initialization(void) {

    DEBUG_PASS("task_initialization()");

    mcu_task_controller_init();

    #ifdef HAS_MANAGEMENT_MODULE_IO
    DEBUG_PASS("task_initialization() - IO input-controller task");
    io_input_controller_init();
    #endif

    #ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115

    DEBUG_PASS("task_initialization() - SHT31 (Temperature / Humidity) task");
    mcu_task_controller_register_task(&sht31_mcu_task);

    DEBUG_PASS("task_initialization() - ADS1115 (ADC) task");
    mcu_task_controller_register_task(&ads1115_mcu_task);
    #endif

    #if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1
    mcu_task_controller_register_task(&test_tracer_task);
    #endif

    #if config_HAS_LED_MATRIX == 1
    mcu_task_controller_register_task(&led_mcu_task);
    #endif

    #ifdef HAS_APP_TASK_EVENT
    event_controller_init();
    #endif

    #ifdef HAS_MANAGEMENT_MODULE_IO
    DEBUG_PASS("task_initialization() - IO output-controller task");
    io_output_controller_init();
    #endif
}

// --------------------------------------------------------------------------------
