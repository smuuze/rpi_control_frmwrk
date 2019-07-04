/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "driver/cfg_driver_interface.h"
#include "driver/trx_driver_interface.h"
#include "expansion/driver_PCA9670.h"

#include "time_management/time_management.h"

//-----------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(DUT_RUN_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(DUT_BYTES_AVAILABLE_TIMER)

//-----------------------------------------------------------------------------

void i2c_driver_initialize(void) {
	DEBUG_PASS("i2c_driver_initialize()");
}

void i2c_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {
	(void) p_cfg;
	DEBUG_PASS("i2c_driver_configure()");
}

void i2c_driver_power_off(void) {
	DEBUG_PASS("i2c_driver_power_off()");

}

u8 i2c_driver_bytes_available (void) {

	static u8 timeout = 0;
	
	if (timeout == 0) {
		timeout = 1;
		DUT_BYTES_AVAILABLE_TIMER_start();
	}
	
	if (DUT_BYTES_AVAILABLE_TIMER_is_up(50) == 0) {
		return 0;
	}
	
	timeout = 0;
	return 1;
}

u8 i2c_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {
	
	u8 i = 0;
	for ( ; i < num_bytes; i++) {
		p_buffer_to[i] = (i + 1);
	}
	
	DEBUG_TRACE_N(num_bytes, (u8*)p_buffer_to, "i2c_driver_set_N_bytes()");
	
	return i;
}

u8 i2c_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	DEBUG_TRACE_N(num_bytes, (u8*)p_buffer_from, "i2c_driver_set_N_bytes()");
	return 0;
}

u8 i2c_driver_is_ready_for_tx (void) {
	DEBUG_PASS("i2c_driver_is_ready_for_tx()");
	return 1;
}

u8 i2c_driver_is_ready_for_rx(void) {
	DEBUG_PASS("i2c_driver_is_ready_for_rx()");
	return 1;
}

void i2c_driver_start_rx (u16 num_of_rx_bytes) {
	DEBUG_TRACE_byte(num_of_rx_bytes, "i2c_driver_start_rx()");

}

void i2c_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
	DEBUG_TRACE_byte(num_bytes, "i2c_driver_wait_for_rx()");
}

void i2c_driver_stop_rx (void) {
	DEBUG_PASS("i2c_driver_stop_rx()");

}

void i2c_driver_start_tx (void) {
	DEBUG_PASS("i2c_driver_initialize()");

}

void i2c_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
	DEBUG_TRACE_byte(num_bytes, "i2c_driver_wait_for_tx()");
}

void i2c_driver_stop_tx (void) {
	DEBUG_PASS("i2c_driver_stop_tx()");

}

void i2c_driver_clear_buffer (void) {
	DEBUG_PASS("i2c_driver_clear_buffer()");

}

void i2c_driver_set_address (u8 addr) {
	(void) addr;
	DEBUG_TRACE_byte(addr, "i2c_driver_set_address()");
}

u8 i2c_driver_mutex_request(void) {
	DEBUG_PASS("i2c_driver_mutex_request()");
	return 1;
}

void i2c_driver_mutex_release(u8 m_id) {
	DEBUG_PASS("i2c_driver_mutex_release()");

}

PCA9670_BUILD_INSTANCE(DUT_pca9670, 0x20)
PC9670_BUILD_OUTPUT(DUT_out01, 0x20, PCA9670_PIN_NUM_1)

int main( void ) {

	DEBUG_PASS("main() - Welcome to Module Test for PCA9670 device");
	
	DEBUG_PASS("main() - init i2c dummy driver module");
	TRX_DRIVER_INTERFACE i2c0_driver = {
		I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;
		&i2c_driver_initialize,
		&i2c_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
		&i2c_driver_power_off,			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
		&i2c_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
		&i2c_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
		&i2c_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
		&i2c_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
		&i2c_driver_wait_for_rx,		//
		&i2c_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
		&i2c_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&i2c_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&i2c_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
		&i2c_driver_wait_for_tx,		//
		&i2c_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
		&i2c_driver_clear_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
		&i2c_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
		&i2c_driver_mutex_request,		//
		&i2c_driver_mutex_release		//
	};

	DEBUG_PASS("main() - init PC9670 module");
	pca9670_init(&i2c0_driver);
	
	DEBUG_PASS("main() - Initialize PCA9670 Task");
	pca9670_task_init();
	
	DEBUG_PASS("main() - Initializing PCA9670-Instance");
	DUT_pca9670_init();
	
	DEBUG_PASS("main() - Initializing Output01");
	DUT_out01_init();
	
	u8 state_changed = 0;
	
	DUT_RUN_TIMER_start();
	
	DEBUG_PASS("main() - Going to run test-cases");
	for (;;) {  // Endlosschleife
					
		usleep(500);	
	
		if (pca9670_task_get_state() == MCU_TASK_RUNNING) {
			DEBUG_TRACE_word(DUT_RUN_TIMER_elapsed(), "main() ------------ RUNNING DEVICE UNDER TEST ------------");
			pca9670_task_run();
		}
		
		if (state_changed == 0) {
			DEBUG_PASS("main() - Change output state ----------");
			state_changed = 1;
			DUT_out01_set_on();
		}
	}
	
	return 0;
}
