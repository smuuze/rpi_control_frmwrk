/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system_interface.h"

#include "common/local_context.h"
#include "common/local_mutex.h"
#include "common/local_msg_buffer.h"
#include "common/local_data_storage_array.h"

#include "local_sht31_mcu_task.h"

#include "time_management/time_management.h"
#include "power_management/power_management.h"

#include "local_i2c_driver.h"
#include "cfg_driver_interface.h"

#include "asic_information_sht31.h"

//-----------------------------------------------------------------------------

#define SHT31_TASK_RUN_INTERVAL_MS		60000 	/* once in a minute */
#define SHT32_TASK_MAXMIN_INTERVAL_MS		900000	/* every 15 minutes*/
#define SHT31_TASK_COMMAND_BUFFER_LENGHT	5
#define SHT31_TASK_ANSWER_BUFFER_LENGTH		6

#define SHT31_TASK_NUMBER_OF_HISTORY_VALUES	96 /* for the last 24 Hours */

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

/*!
 *
 */
//static u16 task_run_interval_reference_actual = 0;
TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)

/*!
 *
 */
//static u32 task_run_interval_reference_maxmin = 0;
TIME_MGMN_BUILD_STATIC_TIMER_U32(task_timer_maxmin)

/*!
 *
 */
//static u32 operation_refrence_time = 0;
TIME_MGMN_BUILD_STATIC_TIMER_U32(operation_timer)

BUILD_LOCAL_DATA_STORAGE_ARRAY_I8(sht31_temp_24hour, SHT31_TASK_NUMBER_OF_HISTORY_VALUES)
BUILD_LOCAL_DATA_STORAGE_ARRAY_U8(sht31_hum_24hour, SHT31_TASK_NUMBER_OF_HISTORY_VALUES)

POWER_MGMN_INCLUDE_UNIT(POWER_UNIT_5V)

//-----------------------------------------------------------------------------

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;	// pointer to call functions from driver through the interface

/*!
 *
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

//-----------------------------------------------------------------------------

void local_sht31_module_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // local_sht31_module_init()

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
}

void local_sht31_mcu_task_init(void) {

	PASS(); // local_sht31_mcu_task_init()

	operation_stage = SHT31_TASK_STATE_IDLE;
	task_timer_start(); // task_run_interval_reference_actual = 0; //i_system.time.now_u16();
	task_timer_maxmin_start(); // task_run_interval_reference_maxmin = 0;//i_system.time.now_u32();

	GET_SYSTEM(data).temperature.minimal = SHT31_MAXIMUM_MEASSUREABLE_TEMPERATURE;
	GET_SYSTEM(data).humidity.minimal = SHT31_MAXIMUM_MEASSUREABLE_HUMIDITY;

	task_state = MCU_TASK_SLEEPING;
}


MCU_TASK_INTERFACE_TASK_STATE local_sht31_mcu_task_get_state(void) {

	if (task_state == MCU_TASK_SLEEPING) {

		if (task_timer_is_up(SHT31_TASK_RUN_INTERVAL_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_TASK_RUN_INTERVAL_MS) */ != 0) {
			task_state = MCU_TASK_RUNNING;
		}
	}

	return task_state;
}


void local_sht31_mcu_task_run(void) {

	u8 command_buffer[SHT31_TASK_COMMAND_BUFFER_LENGHT];
	u8 answer_buffer[SHT31_TASK_ANSWER_BUFFER_LENGTH];

	u32 calculation_temp = 0;

	#ifdef TRACES_ENABLED
	u16 actual_time = i_system.time.now_u16();
	TRACE_word(actual_time); // local_sht31_mcu_task_run() -----------------
	#endif

	switch (operation_stage) {

		default:

			operation_stage = SHT31_TASK_STATE_IDLE;
			// no break;

		case SHT31_TASK_STATE_IDLE :

			if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
				PASS(); // local_sht31_mcu_task_run() - Can't get Mutex of communication-driver
				break;
			}

			POWER_UNIT_5V_request();

			p_com_driver->configure(&driver_cfg);

			EXPANSION_BOARD_POWER_request();
			
			task_timer_start(); // task_run_interval_reference_actual = i_system.time.now_u16();
			operation_stage = SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT;

			PASS(); // local_sht31_mcu_task_run() - Going to start measurement ----

			// no break;

		case SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR :

			if (POWER_UNIT_5V_is_on() == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_ADC - Waiting for power-management
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Waiting for communication-driver
				break;
			}

			//if (task_timer_is_up(SHT31_STARTUP_TIME_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_STARTUP_TIME_MS) */ == 0) {
			//	PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Wait for Start-Up of Sensor
			//	break;
			//}
			
			if (EXPANSION_BOARD_POWER_is_on() == 0) {
				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_INIT_TEMP_HUM_SENSOR - Wait for Start-Up of Sensor
				break;
			}

			SHT31_SET_COMMAND(SHT31_COMMAND_CLEAR_STATUS_REGISTER, command_buffer);

			p_com_driver->clear_buffer();
			p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
			p_com_driver->set_N_bytes(SHT31_MEASUREMENT_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();

			task_timer_start(); // task_run_interval_reference_actual = i_system.time.now_u16();
			operation_stage = SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT;

			// no break;

		case SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT:

			if (p_com_driver->is_ready_for_tx() == 0) {

				task_state = MCU_TASK_IDLE;
				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT - check if driver is ready for RX

				if (task_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_OPERATION_TIMEOUT_MS) */ != 0) {

					PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_START_TEMP_HUM_MEASSUREMENT - Waiting for Driver to be Ready has FAILED !!! ---
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

			operation_timer_start(); //  operation_refrence_time = i_system.time.now_u32();
			operation_stage = SHT31_TASK_STATE_GET_TEMP_HUM_DATA;
			task_state = MCU_TASK_IDLE;

			// no break;

		case SHT31_TASK_STATE_GET_TEMP_HUM_DATA :

			if (p_com_driver->is_ready_for_rx() == 0) {

				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA - Waiting for communication-driver

				if (task_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_OPERATION_TIMEOUT_MS) */ != 0) {

					PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA -Waiting for Driver to be Ready has FAILED !!! ---
					p_com_driver->stop_tx();
					operation_stage = SHT31_TASK_STATE_CANCEL_OPERATION;
				}

				break;
			}

			if (operation_timer_is_up(SHT31_MEASUREMENT_TIME_LOW_REPEATABILITY_MS) /* i_system.time.isup_u32(operation_refrence_time, SHT31_MEASUREMENT_TIME_LOW_REPEATABILITY_MS) */ == 0) {
				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_GET_TEMP_HUM_DATA - Wait for meassurement to be done
				break;
			}

			task_state = MCU_TASK_RUNNING;

			p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
			p_com_driver->start_rx(SHT31_MEASRUEMENT_ANSWER_LENGTH);

			operation_timer_start(); // operation_refrence_time = i_system.time.now_u32();
			operation_stage = SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA;

			// no break;

		case SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA:

			if (p_com_driver->bytes_available() < SHT31_MEASRUEMENT_ANSWER_LENGTH) {

				task_state = MCU_TASK_IDLE;

				if (operation_timer_is_up(SHT31_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u32(operation_refrence_time, SHT31_OPERATION_TIMEOUT_MS) */ != 0) {
					PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA - Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->stop_rx();
					p_com_driver->clear_buffer();

					operation_stage = SHT31_TASK_STATE_CANCEL_OPERATION;
					break;
				}

				PASS(); // local_sht31_mcu_task_run() - SHT31_TASK_STATE_PROCESS_TEMP_HUM_DATA - wait until all bytes are received
				break;
			}

			task_state = MCU_TASK_RUNNING;

			PASS(); ///----- Temperature ----------------------------------------------------

			p_com_driver->get_N_bytes(SHT31_MEASRUEMENT_ANSWER_LENGTH, answer_buffer);

			GET_SYSTEM(data).adc.temperature = (answer_buffer[SHT31_INDEX_OF_TEMPERATURE_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_TEMPERATURE_LSB_IN_ANSWER];
			TRACE_word(GET_SYSTEM(data).adc.temperature); // local_sht31_mcu_task_run() - actual raw-value of temperature

			calculation_temp = (u32)GET_SYSTEM(data).adc.temperature * (u32)SHT31_TEMPERATURE_FIXPOINT_FACTOR_M;
			calculation_temp = calculation_temp >> 16;

			GET_SYSTEM(data).temperature.actual = (i8)((i32)calculation_temp - SHT31_TEMPERATURE_FIXPOINT_FACTOR_B);
			TRACE_byte(GET_SYSTEM(data).temperature.actual); // local_sht31_mcu_task_run() - actual temperature in �C

			PASS(); ///----- Humidity -----------------------------------------------------

			GET_SYSTEM(data).adc.humidity = (answer_buffer[SHT31_INDEX_OF_HUMIDITY_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_HUMIDITY_LSB_IN_ANSWER];
			TRACE_N(2, (u8*)&GET_SYSTEM(data).adc.humidity); // local_sht31_mcu_task_run() - raw value of humidity

			calculation_temp = (u32)GET_SYSTEM(data).adc.humidity * (u32)SHT31_HUMIDITY_FIXPOINT_FACTOR_M;
			calculation_temp = calculation_temp >> 16;

			GET_SYSTEM(data).humidity.actual = calculation_temp;
			TRACE_byte(GET_SYSTEM(data).humidity.actual); // local_sht31_mcu_task_run() - actual relative humidity in %

			PASS(); ///----- Max Min Values -----------------------------------------------

			if (task_timer_maxmin_is_up(SHT32_TASK_MAXMIN_INTERVAL_MS) /* i_system.time.isup_u32(task_run_interval_reference_maxmin, SHT32_TASK_MAXMIN_INTERVAL_MS) */ == 0) {
				task_timer_maxmin_start();// task_run_interval_reference_maxmin = i_system.time.now_u32();

				sht31_temp_24hour_data_storage_array_add_value(GET_SYSTEM(data).temperature.actual);
				GET_SYSTEM(data).temperature.maximal = sht31_temp_24hour_data_storage_array_get_max();
				GET_SYSTEM(data).temperature.minimal = sht31_temp_24hour_data_storage_array_get_min();
				GET_SYSTEM(data).temperature.mean = sht31_temp_24hour_data_storage_array_get_mean();

				TRACE_byte(GET_SYSTEM(data).temperature.maximal); // local_sht31_mcu_task_run() - maximum temperature in �C
				TRACE_byte(GET_SYSTEM(data).temperature.minimal); // local_sht31_mcu_task_run() - minimum temperature in �C
				TRACE_byte(GET_SYSTEM(data).temperature.mean); // local_sht31_mcu_task_run() - mean temperature in �C

				sht31_hum_24hour_data_storage_array_add_value(GET_SYSTEM(data).humidity.actual);
				GET_SYSTEM(data).humidity.maximal = sht31_hum_24hour_data_storage_array_get_max();
				GET_SYSTEM(data).humidity.minimal = sht31_hum_24hour_data_storage_array_get_min();
				GET_SYSTEM(data).humidity.mean = sht31_hum_24hour_data_storage_array_get_mean();

				TRACE_byte(GET_SYSTEM(data).humidity.maximal); // maximum humidity in %
				TRACE_byte(GET_SYSTEM(data).humidity.minimal); // minimum humidity in %
				TRACE_byte(GET_SYSTEM(data).humidity.mean); // mean humidity in %
			}

			//no break;

		case SHT31_TASK_STATE_CANCEL_OPERATION :

			operation_stage = SHT31_TASK_STATE_IDLE;
			task_timer_start(); // task_run_interval_reference_actual = i_system.time.now_u16();
			task_state = MCU_TASK_SLEEPING;

			PASS(); // local_sht31_mcu_task_run() - Measurement has been finished ----

			p_com_driver->shut_down();
			p_com_driver->mutex_rel(com_driver_mutex_id);

			POWER_UNIT_5V_release();

			break;
	}
}

void local_sht31_mcu_task_background_run(void) {

}

void local_sht31_mcu_task_sleep(void) {

}

void local_sht31_mcu_task_wakeup(void) {

}

void local_sht31_mcu_task_finish(void) {

}

void local_sht31_mcu_task_terminate(void) {

}
