
 /*
  * \@file	copro/copo.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"
#include "copro/copro_interface.h"
#include "common/local_mutex.h"

//-----------------------------------------------------------------------------

#ifdef COPRO1_AVAILABLE
COPRO_INTERFACE_CRATE(COPRO1)
#endif

#ifdef COPRO2_AVAILABLE
COPRO_INTERFACE_CRATE(COPRO2)
#endif

#ifdef COPRO3_AVAILABLE
COPRO_INTERFACE_CRATE(COPRO3)
#endif

#ifdef COPRO4_AVAILABLE
COPRO_INTERFACE_CRATE(COPRO4)
#endif

//-----------------------------------------------------------------------------

void copro_interface_initialize(void) {

	#ifdef COPRO1_I2C0
	{
		DEBUG_PASS("copro_interface_initialize() - COPRO1 over I2C0");
		COPRO1_initialize(i_system.driver.i2c0);
	}
	#endif
}

//-----------------------------------------------------------------------------

void copro_initialize(COPRO_INTERFACE* p_copro, TRX_DRIVER_INTERFACE* p_com_driver_interface) {
	p_copro->p_com_driver = p_com_driver_interface;
}

u8 copro_bytes_available(COPRO_INTERFACE* p_copro) {
	return p_copro->p_com_driver->bytes_available();
}

u8 copro_get_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, u8* p_buffer_to) {
	return p_copro->p_com_driver->get_N_bytes(num_bytes, p_buffer_to);
}

u8 copro_set_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, const u8* p_buffer_from) {
	return p_copro->p_com_driver->set_N_bytes(num_bytes, p_buffer_from);
}

u8 copro_is_ready_for_rx(COPRO_INTERFACE* p_copro) {
	return p_copro->p_com_driver->is_ready_for_rx();
}

void copro_start_rx(COPRO_INTERFACE* p_copro, u16 num_of_rx_bytes) {
	p_copro->p_com_driver->start_rx(num_of_rx_bytes);
}

void copro_wait_for_rx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms) {
	p_copro->p_com_driver->wait_for_rx(num_bytes, timeout_ms);
}

void copro_stop_rx(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->stop_rx();
}

u8 copro_is_ready_for_tx(COPRO_INTERFACE* p_copro) {
	return p_copro->p_com_driver->is_ready_for_tx();
}

void copro_start_tx(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->start_tx();
}

void copro_wait_for_tx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms) {
	p_copro->p_com_driver->wait_for_tx(num_bytes, timeout_ms);
}

void copro_stop_tx(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->stop_tx();
}

void copro_clear_rx_buffer(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->clear_rx_buffer();
}

void copro_clear_tx_buffer(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->clear_tx_buffer();
}

void copro_set_address(COPRO_INTERFACE* p_copro, u8 addr) {
	p_copro->address = addr;
}

u8 copro_get_address(COPRO_INTERFACE* p_copro) {
	return p_copro->address;
}

u8 copro_mutex_request(COPRO_INTERFACE* p_copro) {

	if (p_copro->mutex_id != MUTEX_INVALID_ID) {
		return 0;
	}

	p_copro->mutex_id = p_copro->p_com_driver->mutex_req();
	return 1;
}

void copro_mutex_release(COPRO_INTERFACE* p_copro) {
	p_copro->p_com_driver->mutex_rel(p_copro->mutex_id);
	p_copro->mutex_id = MUTEX_INVALID_ID;
}