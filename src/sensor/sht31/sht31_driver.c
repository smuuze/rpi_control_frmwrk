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
 * @file    local_sht31_mcu_task.c
 * @author  Sebastian Lesse
 * @date    2023 / 08 / 19
 * @brief   Implementation of the SHT31 sensor interface
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

#include "mcu_task_interface.h"

#include "system_interface.h"

#include "common/local_context.h"
#include "common/local_mutex.h"
#include "common/local_msg_buffer.h"
#include "common/local_data_storage_array.h"

#include "time_management/time_management.h"
#include "power_management/power_module_5V.h"

#include "cfg_driver_interface.h"

// --------------------------------------------------------------------------------

#include "sensor/sht31/sht31_driver.h"

// --------------------------------------------------------------------------------

#define SHT31_TASK_SCHEDULE_INTERVAL_MS         50
#define SHT31_TASK_RUN_INTERVAL_MS              60000     /* once in a minute */
#define SHT32_TASK_MAXMIN_INTERVAL_MS           900000    /* every 15 minutes*/
#define SHT31_TASK_COMMAND_BUFFER_LENGHT        5
#define SHT31_TASK_ANSWER_BUFFER_LENGTH         6

#define SHT31_TASK_NUMBER_OF_HISTORY_VALUES     96 /* for the last 24 Hours */

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
    SHT31_TASK_STATE_IDLE,                       //!< SHT31_TASK_STATE_IDLE
    SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR,       //!< SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR
    SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT,//!< SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT
    SHT31_TASK_STATE_GET_TEMP_HUM_DATA,          //!< SHT31_TASK_STATE_GET_TEMP_HUM_DATA
    SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA,      //!< SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA
    SHT31_TASK_STATE_CANCEL_OPERATION
} SHT31_TASK_STATE;

// --------------------------------------------------------------------------------

/*!
 *
 */
TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)

/*!
 *
 */
TIME_MGMN_BUILD_STATIC_TIMER_U32(task_timer_maxmin)

/*!
 *
 */
TIME_MGMN_BUILD_STATIC_TIMER_U32(operation_timer)

// --------------------------------------------------------------------------------

BUILD_LOCAL_DATA_STORAGE_ARRAY_I8(sht31_temp_24hour, SHT31_TASK_NUMBER_OF_HISTORY_VALUES)
BUILD_LOCAL_DATA_STORAGE_ARRAY_U8(sht31_hum_24hour, SHT31_TASK_NUMBER_OF_HISTORY_VALUES)

// --------------------------------------------------------------------------------

/**
 * @brief pointer to call functions from driver through the interface
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/**
 * @brief The configuration used to communication with the SHT31.
 * Configuraiton is keept during runtime, because it ight be possible
 * that an other task reconfigures the communicaiton device. We need
 * to set it back to our configuration.
 */
static TRX_DRIVER_CONFIGURATION driver_cfg;

/*!
 *
 */
static SHT31_TASK_STATE operation_stage = SHT31_TASK_STATE_IDLE;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

/*!
 *
 */
static u8 com_driver_mutex_id = 0;

// --------------------------------------------------------------------------------

/**
 * @see mch_task_management/mcu_task_interface#MCU_TASK_INTERFACE.init
 */
static void sht31_task_start(void) {

    DEBUG_PASS("sht31_task_start()");

    operation_stage = SHT31_TASK_STATE_IDLE;
    task_timer_start();
    task_timer_maxmin_start();

    GET_SYSTEM(data).temperature.minimal = SHT31_MAXIMUM_MEASSUREABLE_TEMPERATURE;
    GET_SYSTEM(data).humidity.minimal = SHT31_MAXIMUM_MEASSUREABLE_HUMIDITY;

    task_state = MCU_TASK_SLEEPING;
}

/**
 * @see mch_task_management/mcu_task_interface#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 sht31_task_get_schedule_interval(void) {
    return SHT31_TASK_SCHEDULE_INTERVAL_MS;
}

/**
 * @see mch_task_management/mcu_task_interface#MCU_TASK_INTERFACE.get_sate
 */
static MCU_TASK_INTERFACE_TASK_STATE sht31_task_get_state(void) {

    if (task_state == MCU_TASK_SLEEPING) {

        if (task_timer_is_up(SHT31_TASK_RUN_INTERVAL_MS)) {
            task_state = MCU_TASK_RUNNING;
        }
    }

    return task_state;
}

/**
 * @see mch_task_management/mcu_task_interface#MCU_TASK_INTERFACE.run
 */
static void sht31_task_execute(void) {

    u8 command_buffer[SHT31_TASK_COMMAND_BUFFER_LENGHT];
    u8 answer_buffer[SHT31_TASK_ANSWER_BUFFER_LENGTH];

    u32 calculation_temp = 0;

    DEBUG_CODE_BLOCK (
        u16 actual_time = i_system.time.now_u16();
        DEBUG_TRACE_word(actual_time, "sht31_task_execute() -----------------");
    )

    switch (operation_stage) {

        default:

            operation_stage = SHT31_TASK_STATE_IDLE;
            // no break;

        case SHT31_TASK_STATE_IDLE :

            if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
                DEBUG_PASS("sht31_task_execute() - Can't get Mutex of communication-driver");
                break;
            }

            POWER_UNIT_5V_request();

            p_com_driver->configure(&driver_cfg);
            
            task_timer_start();
            operation_stage = SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT;

            DEBUG_PASS("sht31_task_execute() - Going to start measurement ----");

            // no break;
            /* fall through */

        case SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR :

            if (POWER_UNIT_5V_is_on() == 0) {
                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Waiting for power-management");
                break;
            }

            if (p_com_driver->is_ready_for_tx() == 0) {
                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Waiting for communication-driver");
                break;
            }
            
            if (POWER_UNIT_5V_is_on() == 0) {
                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Wait for Start-Up of Sensor");
                break;
            }

            SHT31_SET_COMMAND(SHT31_COMMAND_CLEAR_STATUS_REGISTER, command_buffer);

            p_com_driver->clear_rx_buffer();
            p_com_driver->clear_tx_buffer();

            p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
            p_com_driver->set_N_bytes(SHT31_MEASUREMENT_COMMAND_LENGTH, command_buffer);
            p_com_driver->start_tx();

            task_timer_start();
            operation_stage = SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT;

            // no break;
            /* fall through */

        case SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT:

            if (p_com_driver->is_ready_for_tx() == 0) {

                task_state = MCU_TASK_IDLE;
                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT - check if driver is ready for RX");

                if (task_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) != 0) {

                    DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT - Waiting for Driver to be Ready has FAILED !!! ---");
                    p_com_driver->stop_tx();
                    operation_stage = SHT31_TASK_STATE_CANCEL_OPERATION;
                }

                break;
            }

            task_state = MCU_TASK_RUNNING;

            SHT31_SET_COMMAND(SHT31_COMMAND_NO_CLK_STRECHTING_LOW_REPEATABILITY, command_buffer);

            p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
            p_com_driver->set_N_bytes(SHT31_MEASUREMENT_COMMAND_LENGTH, command_buffer);
            p_com_driver->start_tx();

            operation_timer_start();
            operation_stage = SHT31_TASK_STATE_GET_TEMP_HUM_DATA;
            task_state = MCU_TASK_IDLE;

            // no break;
            /* fall through */

        case SHT31_TASK_STATE_GET_TEMP_HUM_DATA :

            if (p_com_driver->is_ready_for_rx() == 0) {

                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA - Waiting for communication-driver");

                if (task_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) != 0) {

                    DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA -Waiting for Driver to be Ready has FAILED !!! ---");
                    p_com_driver->stop_tx();
                    operation_stage = SHT31_TASK_STATE_CANCEL_OPERATION;
                }

                break;
            }

            if (operation_timer_is_up(SHT31_MEASUREMENT_TIME_LOW_REPEATABILITY_MS) == 0) {
                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA - Wait for meassurement to be done");
                break;
            }

            task_state = MCU_TASK_RUNNING;

            p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
            p_com_driver->start_rx(SHT31_MEASRUEMENT_ANSWER_LENGTH);

            operation_timer_start();
            operation_stage = SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA;

            // no break;
            /* fall through */

        case SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA:

            if (p_com_driver->bytes_available() < SHT31_MEASRUEMENT_ANSWER_LENGTH) {

                task_state = MCU_TASK_IDLE;

                if (operation_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) != 0) {
                    DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA - Timeout on waiting for data");

                    p_com_driver->stop_tx();
                    p_com_driver->clear_tx_buffer();
                    
                    p_com_driver->stop_rx();
                    p_com_driver->clear_rx_buffer();

                    operation_stage = SHT31_TASK_STATE_CANCEL_OPERATION;
                    break;
                }

                DEBUG_PASS("sht31_task_execute() - SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA - wait until all bytes are received");
                break;
            }

            task_state = MCU_TASK_RUNNING;

            DEBUG_PASS("sht31_task_execute() -  Temperature ------");

            p_com_driver->get_N_bytes(SHT31_MEASRUEMENT_ANSWER_LENGTH, answer_buffer);

            GET_SYSTEM(data).adc.temperature = (answer_buffer[SHT31_INDEX_OF_TEMPERATURE_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_TEMPERATURE_LSB_IN_ANSWER];
            DEBUG_TRACE_word(GET_SYSTEM(data).adc.temperature, "sht31_task_execute() - actual raw-value of temperature");

            calculation_temp = (u32)GET_SYSTEM(data).adc.temperature * (u32)SHT31_TEMPERATURE_FIXPOINT_FACTOR_M;
            calculation_temp = calculation_temp >> 16;

            GET_SYSTEM(data).temperature.actual = (i8)((i32)calculation_temp - SHT31_TEMPERATURE_FIXPOINT_FACTOR_B) + SHT31_TEMPERATURE_CORRECTION_OFFSET;
            DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.actual, "sht31_task_execute() - actual temperature in Celsius");

            if (GET_SYSTEM(data).temperature.actual > GET_SYSTEM(data).temperature.maximal) {
                GET_SYSTEM(data).temperature.maximal = GET_SYSTEM(data).temperature.actual;
                DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.maximal, "sht31_task_execute() - New Maximum Temperature");
            }

            if (GET_SYSTEM(data).temperature.actual < GET_SYSTEM(data).temperature.minimal) {
                GET_SYSTEM(data).temperature.minimal = GET_SYSTEM(data).temperature.actual;
                DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.minimal, "sht31_task_execute() - New Minimum Temperature");
            }

            DEBUG_PASS("sht31_task_execute() -  Humidity ------");

            GET_SYSTEM(data).adc.humidity = (answer_buffer[SHT31_INDEX_OF_HUMIDITY_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_HUMIDITY_LSB_IN_ANSWER];
            DEBUG_TRACE_N(2, (u8*)&GET_SYSTEM(data).adc.humidity, "sht31_task_execute() - raw value of humidity");

            calculation_temp = (u32)GET_SYSTEM(data).adc.humidity * (u32)SHT31_HUMIDITY_FIXPOINT_FACTOR_M;
            calculation_temp = calculation_temp >> 16;

            GET_SYSTEM(data).humidity.actual = calculation_temp;
            DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.actual, "sht31_task_execute() - actual relative humidity in %");

            if (GET_SYSTEM(data).humidity.actual > GET_SYSTEM(data).humidity.maximal) {
                GET_SYSTEM(data).humidity.maximal = GET_SYSTEM(data).temperature.actual;
                DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.maximal, "sht31_task_execute() - New Maximum Humidity");
            }

            if (GET_SYSTEM(data).humidity.actual < GET_SYSTEM(data).humidity.minimal) {
                GET_SYSTEM(data).humidity.minimal = GET_SYSTEM(data).humidity.actual;
                DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.minimal, "sht31_task_execute() - New Minimum Humidity");
            }

            DEBUG_PASS("sht31_task_execute() -  Max Min Values ------");

            if (task_timer_maxmin_is_up(SHT32_TASK_MAXMIN_INTERVAL_MS) == 0) {
                task_timer_maxmin_start();

                sht31_temp_24hour_data_storage_array_add_value(GET_SYSTEM(data).temperature.actual);
                GET_SYSTEM(data).temperature.maximal = sht31_temp_24hour_data_storage_array_get_max();
                GET_SYSTEM(data).temperature.minimal = sht31_temp_24hour_data_storage_array_get_min();
                GET_SYSTEM(data).temperature.mean = sht31_temp_24hour_data_storage_array_get_mean();

                DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.maximal, "sht31_task_execute() - maximum temperature in Degree of Celsius");
                DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.minimal, "sht31_task_execute() - minimum temperature in Degree of Celsius");
                DEBUG_TRACE_byte(GET_SYSTEM(data).temperature.mean, "sht31_task_execute() - mean temperature in Degree of Celsius");

                sht31_hum_24hour_data_storage_array_add_value(GET_SYSTEM(data).humidity.actual);
                GET_SYSTEM(data).humidity.maximal = sht31_hum_24hour_data_storage_array_get_max();
                GET_SYSTEM(data).humidity.minimal = sht31_hum_24hour_data_storage_array_get_min();
                GET_SYSTEM(data).humidity.mean = sht31_hum_24hour_data_storage_array_get_mean();

                DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.maximal, "sht31_task_execute() - maximum humidity in %");
                DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.minimal, "sht31_task_execute() - minimum humidity in %");
                DEBUG_TRACE_byte(GET_SYSTEM(data).humidity.mean, "sht31_task_execute() - mean humidity in %");
            }

            //no break;
            /* fall through */

        case SHT31_TASK_STATE_CANCEL_OPERATION :

            operation_stage = SHT31_TASK_STATE_IDLE;
            task_timer_start();
            task_state = MCU_TASK_SLEEPING;

            DEBUG_PASS("sht31_task_execute() - Measurement has been finished ----");

            p_com_driver->shut_down();
            p_com_driver->mutex_rel(com_driver_mutex_id);

            POWER_UNIT_5V_release();

            break;
    }
}

/**
 * @see mch_task_management/mcu_task_interface#MCU_TASK_INTERFACE.terminate
 */
static void sht31_task_terminate(void) {

}

// --------------------------------------------------------------------------------

TASK_CREATE(
    SHCT31_TASK,
    TASK_PRIORITY_MIDDLE,
    sht31_task_get_schedule_interval,
    sht31_task_start,
    sht31_task_execute,
    sht31_task_get_state,
    sht31_task_terminate
)

// --------------------------------------------------------------------------------

/**
 * @see sht32_driver.h#sht31_driver_init
 */
void sht31_driver_init(TRX_DRIVER_INTERFACE* p_driver) {

    DEBUG_PASS("local_sht31_module_init()");

    driver_cfg.module.i2c.answer_general_call = 0;
    driver_cfg.module.i2c.bit_rate = DRIVER_I2C_BITRATE_1KHZ;
    driver_cfg.module.i2c.enable_ack = 0;
    driver_cfg.module.i2c.interrupt_enable = 1;
    driver_cfg.module.i2c.is_master = 1;
    driver_cfg.module.i2c.slave_addr = 0;

    p_com_driver = p_driver;

    GET_SYSTEM(data).temperature.maximal = 0;
    GET_SYSTEM(data).temperature.minimal = 127;

    sht31_temp_24hour_data_storage_array_init();
    sht31_hum_24hour_data_storage_array_init();

    SHCT31_TASK_init();
}

// --------------------------------------------------------------------------------
