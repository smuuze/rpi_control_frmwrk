/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_jvc_commands.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "button_watcher.h"
#include "io_output_controller.h"
#include "system_interface.h"

// --------------------------------------------------------------------------------

#include "local_context.h"
#include "local_adc_mcu_task.h"
#include "local_mutex.h"

#include "rpi_command_handler.h"
#include "local_msg_buffer.h"

#include "local_context.h"
#include "local_i2c_driver.h"
#include "asic_information_sht31.h"
#include "asic_information_ads115.h"

#include "system_interface.h"

// --------------------------------------------------------------------------------

#define ADC_TASK_RUN_INTERVAL_MS		60000
#define ADC_TASK_COMMAND_BUFFER_LENGHT		5
#define ADC_TASK_ANSWER_BUFFER_LENGTH		6

#define ADC_TASK_NUMBER_OF_HISTORY_VALUES	10


/*!
 *
 */
typedef enum {
	ADC_TASK_STATE_IDLE,                       //!< ADC_TASK_STATE_IDLE
	ADC_TASK_STATE_INIT_TEMP_HUM_SENSOR,       //!< ADC_TASK_STATE_INIT_TEMP_HUM_SENSOR
	ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT,//!< ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT
	ADC_TASK_STATE_GET_TEMP_HUM_DATA,          //!< ADC_TASK_STATE_GET_TEMP_HUM_DATA
	ADC_TASK_STATE_PROCESS_TEMP_HUM_DATA,      //!< ADC_TASK_STATE_PROCESS_TEMP_HUM_DATA
	ADC_TASK_STATE_INIT_ADC,                   //!< ADC_TASK_STATE_INIT_ADC
	ADC_TASK_STATE_INIT_MEASSUREMENT_CHAN,     //!< ADC_TASK_STATE_INIT_MEASSUREMENT_CHAN
	ADC_TASK_STATE_START_MEASSUREMENT_CHAN,    //!< ADC_TASK_STATE_START_MEASSUREMENT_CHAN
	ADC_TASK_STATE_WAIT_FOR_COMPLETEION,
	ADC_TASK_STATE_GET_DATA_CHAN,              //!< ADC_TASK_STATE_GET_DATA_CHAN
	ADC_TASK_STATE_PROCESS_DATA_CHAN,          //!< ADC_TASK_STATE_PROCESS_DATA_CHAN
} ADC_TASK_STATE;

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
static ADC_TASK_STATE actual_task_state = ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT;

/*!
 *
 */
static u16 task_run_interval_reference = 0;

/*!
 *
 */
static u16 operation_refrence_time = 0;

/*!
 *
 */
static const u8 adc_address_list[] = {ADS1115_CFG_CHANNEL_INDEX_0, ADS1115_CFG_CHANNEL_INDEX_1, ADS1115_CFG_CHANNEL_INDEX_2, ADS1115_CFG_CHANNEL_INDEX_3};


/*!
 *
 */
static u16 adss115_history_value_list[ADS1115_NUMBER_OF_CHANNELS][ADC_TASK_NUMBER_OF_HISTORY_VALUES];

/*!
 *
 */
static u16 sht31_history_value_list[2][ADC_TASK_NUMBER_OF_HISTORY_VALUES];

/*!
 *
 * @param divident
 * @param divisor
 * @return
 */
static u32 division(u32 divident, u32 divisor) {
	u32 result = 0;

	while (divident > divisor) {
		divident -= divisor;
		result += 1;
	}

	return result;
}

/*!
 *
 * @param new_value
 * @param value_array
 * @param array_length
 * @return
 */
static u16 local_adc_module_calculate_mean(u16 new_value, u16* value_array, u8 array_length) {

	u32 mean_value = new_value;

	u8 i = 0;
	for ( ; i < array_length - 1; i++) {

		mean_value += value_array[i];
		value_array[i] = value_array[i + 1];
	}

	value_array[i] = new_value;
	return (u16)division(mean_value, array_length);
}


void local_adc_module_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // local_adc_module_init()

	p_com_driver = p_driver;
	p_com_driver->configure(&driver_cfg);

	GET_SYSTEM(data).temperature.maximal = 0;
	GET_SYSTEM(data).temperature.minimal = 0;


	u8 i = 0;
	for ( ; i < ADC_TASK_NUMBER_OF_HISTORY_VALUES; i++) {
		sht31_history_value_list[0][i] = 0x66DB;
		sht31_history_value_list[1][i] = 0x6076;
	}

	for ( ; i < ADS1115_NUMBER_OF_CHANNELS; i++) {

		u8 j = 0;
		for ( ; i < ADC_TASK_NUMBER_OF_HISTORY_VALUES; i++) {
			adss115_history_value_list[i][j] = 0;
		}
	}
}

void local_adc_mcu_task_init(void) {

	actual_task_state = ADC_TASK_STATE_INIT_TEMP_HUM_SENSOR;
	task_run_interval_reference = i_system.time.now_u16();
}

u16 local_adc_mcu_task_get_schedule_interval(void) {
	return ADC_TASK_RUN_INTERVAL_MS;
}


MCU_TASK_INTERFACE_TASK_STATE local_adc_mcu_task_get_state(void) {
	return MCU_TASK_RUNNING;
}


void local_adc_mcu_task_run(void) {

	static u8 adc_channel_index = 0;

	u8 command_buffer[ADC_TASK_COMMAND_BUFFER_LENGHT];
	u8 answer_buffer[ADC_TASK_ANSWER_BUFFER_LENGTH];

	u32 calculation_temp = 0;

	switch (actual_task_state) {

		default:

			actual_task_state = ADC_TASK_STATE_IDLE; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_IDLE :

			if (i_system.time.isup_u16(task_run_interval_reference, ADC_TASK_RUN_INTERVAL_MS) == 0) {
				break;
			}

			task_run_interval_reference = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT; PASS(); // -----------------

			// no break;

		case ADC_TASK_STATE_INIT_TEMP_HUM_SENSOR :

			if (i_system.time.isup_u16(task_run_interval_reference, SHT31_STARTUP_TIME_MS) == 0) {
				PASS(); // Wait for Start-Up of Sensor
				break;
			}

			SHT31_SET_COMMAND(SHT31_COMMAND_CLEAR_STATUS_REGISTER, command_buffer);

			p_com_driver->clear_buffer();

			// clear status register
			p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
			p_com_driver->set_N_bytes(SHT31_MEASUREMENT_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();

			task_run_interval_reference = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT; PASS(); // -----------------

			// no break;

		case ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT:

			if (p_com_driver->is_ready_for_tx() == 0) {

				PASS(); // check if driver is ready for RX

				if (i_system.time.isup_u16(task_run_interval_reference, SHT31_OPERATION_TIMEOUT_MS) != 0) {
					PASS(); // Waiting for Driver to be Ready has FAILED !!! ---
					p_com_driver->stop_tx();
					actual_task_state = ADC_TASK_STATE_INIT_ADC;
				}

				break;
			}

			PASS(); // ADC_TASK_STATE_START_TEMP_HUM_MEASSUREMENT

			SHT31_SET_COMMAND(SHT31_COMMAND_NO_CLK_STRECHTING_LOW_REPEATABILITY, command_buffer);

			p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
			p_com_driver->set_N_bytes(SHT31_MEASUREMENT_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();

			operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_GET_TEMP_HUM_DATA; PASS(); // -----------------

			// no break;

		case ADC_TASK_STATE_GET_TEMP_HUM_DATA :

			if (p_com_driver->is_ready_for_tx() == 0) {

				PASS(); // check if driver is ready for RX

				if (i_system.time.isup_u16(task_run_interval_reference, SHT31_OPERATION_TIMEOUT_MS) != 0) {
					PASS(); // Waiting for Driver to be Ready has FAILED !!! ---
					p_com_driver->stop_tx();
					actual_task_state = ADC_TASK_STATE_INIT_ADC;
				}

				break;
			}

			if (i_system.time.isup_u16(operation_refrence_time, SHT31_MEASUREMENT_TIME_LOW_REPEATABILITY_MS) == 0) {
				PASS(); // Wait for meassurement to be done
				break;
			}

			PASS(); // ADC_TASK_STATE_GET_TEMP_HUM_DATA

			p_com_driver->set_address(SHT31_BUS_ADDRESS_1);
			p_com_driver->start_rx(SHT31_MEASRUEMENT_ANSWER_LENGTH);

			operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_PROCESS_TEMP_HUM_DATA; PASS(); // -----------------

			// no break;

		case ADC_TASK_STATE_PROCESS_TEMP_HUM_DATA:

			if (p_com_driver->bytes_available() < SHT31_MEASRUEMENT_ANSWER_LENGTH) {

				if (i_system.time.isup_u16(operation_refrence_time, SHT31_OPERATION_TIMEOUT_MS) != 0) {
					PASS(); // Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->stop_rx();
					p_com_driver->clear_buffer();

					actual_task_state = ADC_TASK_STATE_INIT_ADC; PASS(); // -----------------
					break;
				}

				PASS(); // wait until all bytes are received
				break;
			}

			PASS(); // ADC_TASK_STATE_PROCESS_TEMP_HUM_DATA

			///----- Temperature -----------------------------------------------------

			p_com_driver->get_N_bytes(SHT31_MEASRUEMENT_ANSWER_LENGTH, answer_buffer);

			GET_SYSTEM(data).adc.temperature = (answer_buffer[SHT31_INDEX_OF_TEMPERATURE_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_TEMPERATURE_LSB_IN_ANSWER];
			TRACE_word(GET_SYSTEM(data).adc.temperature); // actual raw-value of temperature
			GET_SYSTEM(data).adc.temperature = local_adc_module_calculate_mean(GET_SYSTEM(data).adc.temperature, sht31_history_value_list[0], ADC_TASK_NUMBER_OF_HISTORY_VALUES);
			TRACE_word(GET_SYSTEM(data).adc.temperature); // actual mean-value of temperature

			calculation_temp = (u32)GET_SYSTEM(data).adc.temperature * (u32)SHT31_TEMPERATURE_FIXPOINT_FACTOR_M;
			calculation_temp = calculation_temp >> 16;
			calculation_temp = calculation_temp - SHT31_TEMPERATURE_FIXPOINT_FACTOR_B;

			GET_SYSTEM(data).temperature.actual = calculation_temp;

			TRACE_word(GET_SYSTEM(data).adc.temperature); // actual raw-value of temperature
			TRACE_byte(GET_SYSTEM(data).temperature.actual); // actual temperature in �C

			if (GET_SYSTEM(data).temperature.maximal < GET_SYSTEM(data).temperature.actual) {
				GET_SYSTEM(data).temperature.maximal = GET_SYSTEM(data).temperature.actual;
			}

			if (GET_SYSTEM(data).temperature.minimal > GET_SYSTEM(data).temperature.actual) {
				GET_SYSTEM(data).temperature.minimal = GET_SYSTEM(data).temperature.actual;
			}

			///----- Humidity -----------------------------------------------------

			GET_SYSTEM(data).adc.humidity = (answer_buffer[SHT31_INDEX_OF_HUMIDITY_MSB_IN_ANSWER] << 8 ) | answer_buffer[SHT31_INDEX_OF_HUMIDITY_LSB_IN_ANSWER];
			TRACE_N(2, &GET_SYSTEM(data).adc.humidity); // raw value of humidity
			GET_SYSTEM(data).adc.humidity = local_adc_module_calculate_mean(GET_SYSTEM(data).adc.humidity, sht31_history_value_list[1], ADC_TASK_NUMBER_OF_HISTORY_VALUES);
			TRACE_N(2, &GET_SYSTEM(data).adc.humidity); // mean value of humidity

			calculation_temp = (u32)GET_SYSTEM(data).adc.humidity * (u32)SHT31_HUMIDITY_FIXPOINT_FACTOR_M;
			calculation_temp = calculation_temp >> 16;

			GET_SYSTEM(data).humidity.actual = calculation_temp;

			TRACE_N(2, &GET_SYSTEM(data).adc.humidity); // raw value of humidity
			TRACE_byte(GET_SYSTEM(data).humidity.actual); // actual relative humidity in %

			if (GET_SYSTEM(data).humidity.maximal < GET_SYSTEM(data).humidity.actual) {
				GET_SYSTEM(data).humidity.maximal = GET_SYSTEM(data).humidity.actual;
			}

			if (GET_SYSTEM(data).humidity.minimal > GET_SYSTEM(data).humidity.actual) {
				GET_SYSTEM(data).humidity.minimal = GET_SYSTEM(data).humidity.actual;
			}

			actual_task_state = ADC_TASK_STATE_INIT_ADC; PASS(); // -----------------
			//p_com_driver->mutex_rel(local_adc_mutex);

			// no break;

		case ADC_TASK_STATE_INIT_ADC:

			ADS1115_BUILD_WRITE_CMD(command_buffer, ADS1115_CONVERSION_REG_ADDR, ADS1115_CONVERSION_RESET_VALUE);

			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->set_N_bytes(ADS1115_WRITE_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();	// write reset commands to conversion register

			actual_task_state = ADC_TASK_STATE_INIT_MEASSUREMENT_CHAN; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_INIT_MEASSUREMENT_CHAN:

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // check if driver is ready for RX
				break;
			}

			ADS1115_BUILD_CONFIG_CMD(command_buffer);
			ADS1115_SET_SINGLE_SHOT_MODE(command_buffer);
			ADS1115_SET_OP_MODE(command_buffer, ADS1115_OP_MODE_SINGLE_SHOT);
			ADS1115_SET_CHANNEL_INDEX(command_buffer, adc_address_list[adc_channel_index]);
			ADS1115_SET_DATARATE(command_buffer, ADS1115_DATARATE_SPS_08);
			ADS1115_SET_GAIN(command_buffer, ADS1115_CFG_GAIN_2048);

			p_com_driver->set_N_bytes(ADS1115_CONFIG_COMMAND_LENGTH, command_buffer);
			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_tx();

			actual_task_state = ADC_TASK_STATE_WAIT_FOR_COMPLETEION; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_WAIT_FOR_COMPLETEION :

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // check if driver is ready for RX
				break;
			}

			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_rx(ADS1115_STATUS_ANSWER_LENGTH);

			operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_START_MEASSUREMENT_CHAN; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_START_MEASSUREMENT_CHAN :

			if (p_com_driver->bytes_available() < ADS1115_STATUS_ANSWER_LENGTH) {

				if (i_system.time.isup_u16(operation_refrence_time, ADS1115_OPERATION_TIMEOUT_MS) != 0) {
					PASS(); // Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->stop_rx();
					p_com_driver->clear_buffer();

					actual_task_state = ADC_TASK_STATE_INIT_ADC; PASS(); // -----------------
					break;
				}

				PASS(); // wait until all bytes are received
				break;
			}

			p_com_driver->get_N_bytes(ADS1115_CONFIG_COMMAND_LENGTH, answer_buffer);
			if (ADS1115_MEASUREMENT_IS_READY(answer_buffer) == 0) {
				PASS(); // Waiting for Measurement to be finished
				actual_task_state = ADC_TASK_STATE_WAIT_FOR_COMPLETEION;
				break;
			}

			ADS1115_BUILD_MEASUREMENT_CMD(command_buffer);

			p_com_driver->set_N_bytes(ADS1115_MEASUREMENT_COMMAND_LENGTH, command_buffer);
			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_tx();

			operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADC_TASK_STATE_GET_DATA_CHAN; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_GET_DATA_CHAN :

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // check if driver is ready for RX
				break;
			}

			if (i_system.time.isup_u16(operation_refrence_time, 5) == 0) {
				break;
			}

			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_rx(ADS1115_MEASUREMENT_ANSWER_LENGTH);

			actual_task_state = ADC_TASK_STATE_PROCESS_DATA_CHAN; PASS(); // -----------------
			// no break;

		case ADC_TASK_STATE_PROCESS_DATA_CHAN :

			if (p_com_driver->bytes_available() < ADS1115_MEASUREMENT_ANSWER_LENGTH) {
				PASS(); // wait until all bytes are received
				break;
			}

			p_com_driver->get_N_bytes(ADS1115_MEASUREMENT_ANSWER_LENGTH, answer_buffer);
			TRACE_N(ADS1115_MEASUREMENT_ANSWER_LENGTH, answer_buffer); // raw value of adc-channel <<<<<<<<<<<<<<

			switch (adc_channel_index) {
				case 0:
					GET_SYSTEM(data).adc.channel_0 = (answer_buffer[0] << 8 ) | answer_buffer[1];
					break;

				case 1:
					GET_SYSTEM(data).adc.channel_1 = (answer_buffer[0] << 8 ) | answer_buffer[1];
					break;

				case 2:
					GET_SYSTEM(data).adc.channel_2 = (answer_buffer[0] << 8 ) | answer_buffer[1];
					break;

				case 3 :
					GET_SYSTEM(data).adc.channel_3 = (answer_buffer[0] << 8 ) | answer_buffer[1];
					break;

				default: break;
			}

			if (++adc_channel_index < 4) {
				actual_task_state = ADC_TASK_STATE_INIT_ADC; PASS(); // -----------------
			} else {
				adc_channel_index = 0;
				actual_task_state = ADC_TASK_STATE_IDLE; PASS(); // -----------------
			}

			break;
	}
}

void local_adc_mcu_task_background_run(void) {

}

void local_adc_mcu_task_sleep(void) {

}

void local_adc_mcu_task_wakeup(void) {

}

void local_adc_mcu_task_finish(void) {

}

void local_adc_mcu_task_terminate(void) {

}
