/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"

#include "common/local_context.h"
#include "common/local_mutex.h"
#include "common/local_msg_buffer.h"
#include "common/signal_slot_interface.h"

#include "time_management/time_management.h"
#include "power_management/power_management.h"

#include "local_i2c_driver.h"
#include "asic_information_ads115.h"

#include "local_ads1115_mcu_task.h"

//-----------------------------------------------------------------------------

#define ADS1115_TASK_SCHEDULE_INTERVAL			50
#define ADS1115_TASK_RUN_INTERVAL_MS			60000 /* once in a minute */
#define ADS1115_TASK_COMMAND_BUFFER_LENGHT		5
#define ADS1115_TASK_ANSWER_BUFFER_LENGTH		6

#define ADS1115_TASK_NUMBER_OF_HISTORY_VALUES		10

//-----------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	ADS1115_TASK_STATE_IDLE,               //!< ADS1115_TASK_STATE_IDLE
	ADS1115_TASK_STATE_INIT_ADC,                   //!< ADS1115_TASK_STATE_INIT_ADC
	ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN,     //!< ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN
	ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN,    //!< ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN
	ADS1115_TASK_STATE_WAIT_FOR_COMPLETION,//!< ADS1115_TASK_STATE_WAIT_FOR_COMPLETION
	ADS1115_TASK_STATE_GET_DATA_CHAN,              //!< ADS1115_TASK_STATE_GET_DATA_CHAN
	ADS1115_TASK_STATE_PROCESS_DATA_CHAN,          //!< ADS1115_TASK_STATE_PROCESS_DATA_CHAN
	ADS1115_TASK_STATE_CANCEL_OPERATION,   //!< ADS1115_TASK_STATE_CANCEL_OPERATION
} ADS1115_TASK_STATE;

//-----------------------------------------------------------------------------

/*!
 *
 */
//static u16 task_run_interval_reference = 0;
TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)

/*!
 *
 */
//static u16 operation_refrence_time = 0;
TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

POWER_MGMN_INCLUDE_UNIT(POWER_UNIT_5V)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_ADC_NEW_VALUES_AVAILABLE)

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
static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

/*!
 *
 */
static ADS1115_TASK_STATE actual_task_state = ADS1115_TASK_STATE_INIT_ADC;

/*!
 *
 */
static u8 com_driver_mutex_id = 0;

/*!
 *
 */
static const u8 adc_address_list[] = {ADS1115_CFG_CHANNEL_INDEX_0, ADS1115_CFG_CHANNEL_INDEX_1, ADS1115_CFG_CHANNEL_INDEX_2, ADS1115_CFG_CHANNEL_INDEX_3};

//-----------------------------------------------------------------------------

void local_ads1115_module_init(TRX_DRIVER_INTERFACE* p_driver) {

	PASS(); // local_ads1115_module_init()

	driver_cfg.module.i2c.answer_general_call = 0;
	driver_cfg.module.i2c.bit_rate = DRIVER_I2C_BITRATE_1KHZ;
	driver_cfg.module.i2c.enable_ack = 0;
	driver_cfg.module.i2c.interrupt_enable = 1;
	driver_cfg.module.i2c.is_master = 1;
	driver_cfg.module.i2c.slave_addr = 0;

	p_com_driver = p_driver;

	GET_SYSTEM(data).temperature.maximal = 0;
	GET_SYSTEM(data).temperature.minimal = 127;

	SIGNAL_ADC_NEW_VALUES_AVAILABLE_init();
}

void local_ads1115_mcu_task_init(void) {

	PASS(); // local_ads1115_mcu_task_init()

	actual_task_state = ADS1115_TASK_STATE_IDLE;
	task_timer_start(); // task_run_interval_reference = i_system.time.now_u16();

	task_state = MCU_TASK_SLEEPING;
}


u16 local_ads1115_mcu_task_get_schedule_interval(void) {
	return ADS1115_TASK_SCHEDULE_INTERVAL;
}


MCU_TASK_INTERFACE_TASK_STATE local_ads1115_mcu_task_get_state(void) {

	if (task_state == MCU_TASK_SLEEPING) {
		if (task_timer_is_up(ADS1115_TASK_RUN_INTERVAL_MS) /* i_system.time.isup_u16(task_run_interval_reference, ADS1115_TASK_RUN_INTERVAL_MS) */ != 0) {
			task_state = MCU_TASK_RUNNING;
		}
	}

	return task_state;
}


void local_ads1115_mcu_task_run(void) {

	static u8 adc_channel_index = 0;

	u8 command_buffer[ADS1115_TASK_COMMAND_BUFFER_LENGHT];
	u8 answer_buffer[ADS1115_TASK_ANSWER_BUFFER_LENGTH];

	#ifdef TRACES_ENABLED
	u16 actual_time = i_system.time.now_u16();
	TRACE_word(actual_time); // local_ads1115_mcu_task_run() -----------------
	#endif

	switch (actual_task_state) {

		default:

			actual_task_state = ADS1115_TASK_STATE_IDLE; PASS(); // -----------------
			// no break;

		case ADS1115_TASK_STATE_IDLE :

			if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
				PASS(); // local_ads1115_mcu_task_run() - Can't get Mutex of communication-driver
				break;
			}

			POWER_UNIT_5V_request();
			
			p_com_driver->configure(&driver_cfg);

			task_timer_start(); // task_run_interval_reference = i_system.time.now_u16();
			actual_task_state = ADS1115_TASK_STATE_INIT_ADC;

			PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_IDLE - startting measurement ---

			// no break;

		case ADS1115_TASK_STATE_INIT_ADC:

			if (POWER_UNIT_5V_is_on() == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_ADC - Waiting for power-management
				break;
			}

			if (p_com_driver->is_ready_for_tx() == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_ADC - Waiting for communication-driver
				break;
			}

			if (task_timer_is_up(ADS1115_STARTUP_TIME_MS) /* i_system.time.isup_u16(task_run_interval_reference, ADS1115_STARTUP_TIME_MS) */ == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_ADC - Waiting for ADS1115 startup
				break;
			}

			ADS1115_BUILD_WRITE_CMD(command_buffer, ADS1115_CONVERSION_REG_ADDR, ADS1115_CONVERSION_RESET_VALUE);

			p_com_driver->clear_tx_buffer();
			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->set_N_bytes(ADS1115_WRITE_COMMAND_LENGTH, command_buffer);
			p_com_driver->start_tx();	// write reset commands to conversion register

			actual_task_state = ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN;
			operation_timer_start();
			// no break;

		case ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN:

			if (p_com_driver->is_ready_for_tx() == 0) {

				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN - check if driver is ready for RX

				if (operation_timer_is_up(ADS1115_OPERATION_TIMEOUT_MS) != 0) {
					PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_INIT_MEASSUREMENT_CHAN - Waiting for com-driver has FAIELD !!! ---
					actual_task_state = ADS1115_TASK_STATE_CANCEL_OPERATION;
					p_com_driver->stop_tx();
				}

				break;
			}

			TRACE_byte(adc_channel_index); // -------ADC Channel Index ----------------------------

			ADS1115_BUILD_CONFIG_CMD(command_buffer);
			ADS1115_SET_SINGLE_SHOT_MODE(command_buffer);
			ADS1115_SET_OP_MODE(command_buffer, ADS1115_OP_MODE_SINGLE_SHOT);
			ADS1115_SET_CHANNEL_INDEX(command_buffer, adc_address_list[adc_channel_index]);
			ADS1115_SET_DATARATE(command_buffer, ADS1115_DATARATE_SPS_08);
			ADS1115_SET_GAIN(command_buffer, ADS1115_CFG_GAIN_4096);

			p_com_driver->set_N_bytes(ADS1115_CONFIG_COMMAND_LENGTH, command_buffer);
			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_tx();

			actual_task_state = ADS1115_TASK_STATE_WAIT_FOR_COMPLETION;
			// no break;

		case ADS1115_TASK_STATE_WAIT_FOR_COMPLETION :

			if (p_com_driver->is_ready_for_rx() == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_WAIT_FOR_COMPLETION - check if driver is ready for RX
				break;
			}

			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_rx(ADS1115_STATUS_ANSWER_LENGTH);

			operation_timer_start();// operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN;
			// no break;

		case ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN :

			if (p_com_driver->bytes_available() < ADS1115_STATUS_ANSWER_LENGTH) {

				task_state = MCU_TASK_IDLE;

				if (operation_timer_is_up(ADS1115_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u16(operation_refrence_time, ADS1115_OPERATION_TIMEOUT_MS) */ != 0) {
					PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN - Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->clear_rx_buffer();

					p_com_driver->stop_rx();
					p_com_driver->clear_tx_buffer();

					actual_task_state = ADS1115_TASK_STATE_CANCEL_OPERATION;
					break;
				}

				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN - wait until all bytes are received
				break;
			}

			task_state = MCU_TASK_RUNNING;

			p_com_driver->get_N_bytes(ADS1115_CONFIG_COMMAND_LENGTH, answer_buffer);
			if (ADS1115_MEASUREMENT_IS_READY(answer_buffer) == 0) {
				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_START_MEASSUREMENT_CHAN - Waiting for Measurement to be finished
				actual_task_state = ADS1115_TASK_STATE_WAIT_FOR_COMPLETION;
				break;
			}

			ADS1115_BUILD_MEASUREMENT_CMD(command_buffer);

			p_com_driver->set_N_bytes(ADS1115_MEASUREMENT_COMMAND_LENGTH, command_buffer);
			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_tx();

			operation_timer_start(); // operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADS1115_TASK_STATE_GET_DATA_CHAN;
			// no break;

		case ADS1115_TASK_STATE_GET_DATA_CHAN :

			if (p_com_driver->is_ready_for_tx() == 0) {

				task_state = MCU_TASK_IDLE;

				if (operation_timer_is_up(ADS1115_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u16(operation_refrence_time, ADS1115_OPERATION_TIMEOUT_MS) */ != 0) {
					PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_PROCESS_DATA_CHAN - Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->clear_rx_buffer();
					
					p_com_driver->stop_rx();
					p_com_driver->clear_tx_buffer();

					actual_task_state = ADS1115_TASK_STATE_CANCEL_OPERATION;
					break;
				}

				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_GET_DATA_CHAN - check if driver is ready for RX
				break;
			}

			if (operation_timer_is_up(5) /* i_system.time.isup_u16(operation_refrence_time, 5) */ == 0) {
				break;
			}

			p_com_driver->set_address(ADS1115_BUS_ADDRESS_01);
			p_com_driver->start_rx(ADS1115_MEASUREMENT_ANSWER_LENGTH);

			operation_timer_start(); // operation_refrence_time = i_system.time.now_u16();
			actual_task_state = ADS1115_TASK_STATE_PROCESS_DATA_CHAN;
			// no break;

		case ADS1115_TASK_STATE_PROCESS_DATA_CHAN :

			if (p_com_driver->bytes_available() < ADS1115_MEASUREMENT_ANSWER_LENGTH) {

				task_state = MCU_TASK_IDLE;

				if (operation_timer_is_up(ADS1115_OPERATION_TIMEOUT_MS) /* i_system.time.isup_u16(operation_refrence_time, ADS1115_OPERATION_TIMEOUT_MS) */ != 0) {
					PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_PROCESS_DATA_CHAN - Timeout on waiting for data

					p_com_driver->stop_tx();
					p_com_driver->clear_rx_buffer();
					
					p_com_driver->stop_rx();
					p_com_driver->clear_tx_buffer();

					actual_task_state = ADS1115_TASK_STATE_CANCEL_OPERATION;
					break;
				}

				PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_PROCESS_DATA_CHAN - wait until all bytes are received
				break;
			}

			task_state = MCU_TASK_RUNNING;

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
				actual_task_state = ADS1115_TASK_STATE_INIT_ADC;
				break;

			}

			SIGNAL_ADC_NEW_VALUES_AVAILABLE_send();

			// no break;

		case ADS1115_TASK_STATE_CANCEL_OPERATION :

			adc_channel_index = 0;
			actual_task_state = ADS1115_TASK_STATE_IDLE;
			task_timer_start(); // task_run_interval_reference = i_system.time.now_u16();

			task_state = MCU_TASK_SLEEPING;
			PASS(); // local_ads1115_mcu_task_run() - ADS1115_TASK_STATE_PROCESS_DATA_CHAN - measurement complete -----

			p_com_driver->shut_down();
			p_com_driver->mutex_rel(com_driver_mutex_id);

			POWER_UNIT_5V_release();

			break;
	}
}

void local_ads1115_mcu_task_background_run(void) {

}

void local_ads1115_mcu_task_sleep(void) {

}

void local_ads1115_mcu_task_wakeup(void) {

}

void local_ads1115_mcu_task_finish(void) {

}

void local_ads1115_mcu_task_terminate(void) {

}
