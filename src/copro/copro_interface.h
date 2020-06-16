
 /*
  * \@file	copro/copro_interface.h
  * \author	sebastian lesse
  */

#ifndef _COPRO_INTERFACE_H_
#define _COPRO_INTERFACE_H_

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"

//-----------------------------------------------------------------------------

#define COPRO_INTERFACE_UNLIMITED_RX_LENGTH			0xFFFF

//-----------------------------------------------------------------------------

/*
 */
typedef void (*COPRO_INTERFACE_INITIALIZE_CALLBACK)		(TRX_DRIVER_INTERFACE* p_configuration, TRX_DRIVER_CONFIGURATION* p_com_driver_cfg);

/*
 */
typedef void (*COPRO_INTERFACE_POWER_ON_CALLBACK)		(void);

/*
 */
typedef void (*COPRO_INTERFACE_POWER_OFF_CALLBACK)		(void);

/*
 */
typedef void (*COPRO_INTERFACE_MODULE_OFF_CALLBACK)		(void);

/*
 */
typedef u8 (*COPRO_INTERFACE_BYTES_AVAILABLE_CALLBACK)		(void);

/*
 */
typedef u8 (*COPRO_INTERFACE_GET_N_BYTES_CALLBACK)		(u8 num_byts_max, u8* p_buffer_to);

/*
 */
typedef u8 (*COPRO_INTERFACE_SET_N_BYTES_CALLBACK)		(u8 num_byts_max, const u8* p_buffer_from);

/*
 */
typedef void (*COPRO_INTERFACE_START_RX_CALLBACK)		(u16 num_of_rx_bytes);

/*
 */
typedef void (*COPRO_INTERFACE_WAIT_FOR_RX_CALLBACK)		(u8 num_byts_max, u16 timeout_ms);


/*
 */
typedef void (*COPRO_INTERFACE_START_TX_CALLBACK)		(void);

/*
 */
typedef void (*COPRO_INTERFACE_WAIT_FOR_TX_CALLBACK)		(u8 num_byts_max, u16 timeout_ms);

/*
 */
typedef void (*COPRO_INTERFACE_STOP_TX_CALLBACK)		(void);

/*
 */
typedef void (*COPRO_INTERFACE_STOP_RX_CALLBACK)		(void);

/*
 *
 */
typedef void (*COPRO_INTERFACE_CLEAR_RX_BUFFER_CALLBACK)	(void);

/*
 *
 */
typedef void (*COPRO_INTERFACE_CLEAR_TX_BUFFER_CALLBACK)	(void);

/*
 *
 */
typedef u8 (*COPRO_INTERFACE_IS_READ_FOR_TRX_CALLBACK)		(void);

/*
 *
 */
typedef void (*COPRO_INTERFACE_SET_ADDRESS_CALLBACK)		(u8 addr);

/*
 *
 */
typedef u8 (*COPRO_INTERFACE_GET_ADDRESS_CALLBACK)		(void);

/*
 *
 */
typedef u8 (*COPRO_INTERFACE_REQUEST_MUTEX_CALLBACK)		(void);

/*
 *
 */
typedef void (*COPRO_INTERFACE_RELEASE_MUTEX_CALLBACK)		(void);

//-----------------------------------------------------------------------------

/*
 *
 */
typedef struct {

	COPRO_INTERFACE_INITIALIZE_CALLBACK 		initialize;
	COPRO_INTERFACE_POWER_ON_CALLBACK		power_on;
	COPRO_INTERFACE_POWER_OFF_CALLBACK		power_off;
	COPRO_INTERFACE_BYTES_AVAILABLE_CALLBACK 	bytes_available;
	COPRO_INTERFACE_GET_N_BYTES_CALLBACK 		get_N_bytes;
	COPRO_INTERFACE_SET_N_BYTES_CALLBACK 		set_N_bytes;
	COPRO_INTERFACE_START_RX_CALLBACK 		start_rx;
	COPRO_INTERFACE_WAIT_FOR_RX_CALLBACK 		wait_for_rx;
	COPRO_INTERFACE_STOP_RX_CALLBACK 		stop_rx;
	COPRO_INTERFACE_IS_READ_FOR_TRX_CALLBACK 	is_ready_for_tx;
	COPRO_INTERFACE_IS_READ_FOR_TRX_CALLBACK 	is_ready_for_rx;
	COPRO_INTERFACE_START_TX_CALLBACK 		start_tx;
	COPRO_INTERFACE_WAIT_FOR_TX_CALLBACK 		wait_for_tx;
	COPRO_INTERFACE_STOP_TX_CALLBACK 		stop_tx;
	COPRO_INTERFACE_CLEAR_RX_BUFFER_CALLBACK 	clear_rx_buffer;
	COPRO_INTERFACE_CLEAR_RX_BUFFER_CALLBACK 	clear_tx_buffer;
	COPRO_INTERFACE_SET_ADDRESS_CALLBACK 		set_address;
	COPRO_INTERFACE_GET_ADDRESS_CALLBACK 		get_address;
	COPRO_INTERFACE_REQUEST_MUTEX_CALLBACK 		mutex_req;
	COPRO_INTERFACE_RELEASE_MUTEX_CALLBACK 		mutex_rel;

} COPRO_INTERFACE_OBJECT;

//-----------------------------------------------------------------------------

/*
 *
 */
typedef struct COPRO_INTERFACE {
	u8 address;
	u8 mutex_id;
	TRX_DRIVER_CONFIGURATION* p_com_driver_cfg;
	TRX_DRIVER_INTERFACE* p_com_driver;
} COPRO_INTERFACE;

//-----------------------------------------------------------------------------

#define COPRO_INTERFACE_CRATE(name, power_on_callback, power_off_callback)				\
													\
	static COPRO_INTERFACE _##name##_copro_interface;						\
													\
	void name##_initialize(TRX_DRIVER_INTERFACE* p_com_driver_interface, TRX_DRIVER_CONFIGURATION* p_com_driver_cfg) {	\
		copro_initialize(&_##name##_copro_interface, p_com_driver_interface, p_com_driver_cfg);				\
	}												\
													\
	void name##_power_on(void) {									\
		power_on_callback();									\
		copro_power_on(&_##name##_copro_interface);						\
	}												\
													\
	void name##_power_off(void) {									\
		power_off_callback();									\
		copro_power_on(&_##name##_copro_interface);						\
	}												\
													\
	u8 name##_bytes_available(void) {								\
		return copro_bytes_available(&_##name##_copro_interface);				\
	}												\
													\
	u8 name##_get_N_bytes(u8 num_bytes, u8* p_buffer_to) {						\
		return copro_get_N_bytes(&_##name##_copro_interface, num_bytes, p_buffer_to);		\
	}												\
													\
	u8 name##_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {					\
		return copro_set_N_bytes(&_##name##_copro_interface, num_bytes, p_buffer_from);		\
	}												\
													\
	u8 name##_is_ready_for_rx(void) {								\
		return copro_is_ready_for_rx(&_##name##_copro_interface);				\
	}												\
													\
	void name##_start_rx(u16 num_of_rx_bytes) {							\
		copro_start_rx(&_##name##_copro_interface, num_of_rx_bytes);				\
	}												\
													\
	void name##_wait_for_rx(u8 num_bytes, u16 timeout_ms) {						\
		copro_wait_for_rx(&_##name##_copro_interface, num_bytes, timeout_ms);			\
	}												\
													\
	void name##_stop_rx(void) {									\
		copro_stop_rx(&_##name##_copro_interface);						\
	}												\
													\
	u8 name##_is_ready_for_tx(void) {								\
		return copro_is_ready_for_tx(&_##name##_copro_interface);				\
	}												\
													\
	void name##_start_tx(void) {									\
		copro_start_tx(&_##name##_copro_interface);						\
	}												\
													\
	void name##_wait_for_tx(u8 num_bytes, u16 timeout_ms) {						\
		copro_wait_for_tx(&_##name##_copro_interface, num_bytes, timeout_ms);			\
	}												\
													\
	void name##_stop_tx(void) {									\
		copro_stop_tx(&_##name##_copro_interface);						\
	}												\
													\
	void name##_clear_rx_buffer(void) {								\
		copro_clear_rx_buffer(&_##name##_copro_interface);					\
	}												\
													\
	void name##_clear_tx_buffer(void) {								\
		copro_clear_tx_buffer(&_##name##_copro_interface);					\
	}												\
													\
	void name##_set_address(u8 addr) {								\
		copro_set_address(&_##name##_copro_interface, addr);					\
	}												\
													\
	u8 name##_get_address(void) {									\
		return copro_get_address(&_##name##_copro_interface);					\
	}												\
													\
	u8 name##_mutex_request(void) {									\
		return copro_mutex_request(&_##name##_copro_interface);					\
	}												\
													\
	void name##_mutex_release(void) {								\
		copro_mutex_release(&_##name##_copro_interface);					\
	}												\
													\
	const COPRO_INTERFACE_OBJECT name = {								\
		.initialize = &name##_initialize,							\
		.power_on = &name##_power_on,								\
		.power_off = &name##_power_off,								\
		.bytes_available = &name##_bytes_available,						\
		.get_N_bytes = &name##_get_N_bytes,							\
		.set_N_bytes = &name##_set_N_bytes,							\
		.start_rx = &name##_start_rx,								\
		.wait_for_rx = &name##_wait_for_rx,							\
		.stop_rx = &name##_stop_rx,								\
		.is_ready_for_tx = &name##_is_ready_for_tx,						\
		.is_ready_for_rx = &name##_is_ready_for_tx,						\
		.start_tx = &name##_start_tx,								\
		.wait_for_tx = &name##_wait_for_tx,							\
		.stop_tx = &name##_stop_tx,								\
		.clear_rx_buffer = &name##_clear_tx_buffer,						\
		.clear_tx_buffer = &name##_clear_tx_buffer,						\
		.set_address = &name##_set_address,							\
		.get_address = &name##_get_address,							\
		.mutex_req = &name##_mutex_request,							\
		.mutex_rel = &name##_mutex_release							\
	};												\

#define COPRO_INTERFACE_INCLUDE(name)									\
	extern const COPRO_INTERFACE_OBJECT name;							\
	void name##_initialize(TRX_DRIVER_INTERFACE* p_com_driver_interface, TRX_DRIVER_CONFIGURATION* p_com_driver_cfg); \
	void name##_power_on(void);									\
	void name##_power_off(void);									\
	u8 name##_bytes_available(void);								\
	u8 name##_get_N_bytes(u8 num_bytes, u8* p_buffer_to);						\
	u8 name##_set_N_bytes(u8 num_bytes, const u8* p_buffer_from);					\
	u8 name##_is_ready_for_tx(void);								\
	void name##_start_rx(u16 num_of_rx_bytes);							\
	void name##_wait_for_rx(u8 num_bytes, u16 timeout_ms);						\
	void name##_stop_rx(void);									\
	u8 name##_is_ready_for_tx(void);								\
	void name##_start_tx(void);									\
	void name##_wait_for_tx(u8 num_bytes, u16 timeout_ms);						\
	void name##_stop_tx(void);									\
	void name##_clear_rx_buffer(void);								\
	void name##_clear_tx_buffer(void);								\
	void name##_set_address(u8 addr);								\
	u8 name##_get_address(void);									\
	u8 name##_mutex_request(void);									\
	void name##_mutex_release(u8 m_id);

//-----------------------------------------------------------------------------

void copro_interface_initialize(void);

//-----------------------------------------------------------------------------

/*!
 *
 */
void copro_initialize(COPRO_INTERFACE* p_copro, TRX_DRIVER_INTERFACE* p_com_driver_interface, TRX_DRIVER_CONFIGURATION* p_com_driver_cfg);

/*!
 *
 */
void copro_power_on(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_power_off(COPRO_INTERFACE* p_copro);

/*!
 *
 */
u8 copro_bytes_available(COPRO_INTERFACE* p_copro);

/*!
 *
 */
u8 copro_get_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, u8* p_buffer_to);

/*!
 *
 */
u8 copro_set_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, const u8* p_buffer_from);

/*!
 *
 */
u8 copro_is_ready_for_rx(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_start_rx(COPRO_INTERFACE* p_copro, u16 num_of_rx_bytes);

/*!
 *
 */
void copro_wait_for_rx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms);

/*!
 *
 */
void copro_stop_rx(COPRO_INTERFACE* p_copro);

/*!
 *
 */
u8 copro_is_ready_for_tx(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_start_tx(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_wait_for_tx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms);

/*!
 *
 */
void copro_stop_tx(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_clear_rx_buffer(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_clear_tx_buffer(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_set_address(COPRO_INTERFACE* p_copro, u8 addr);

/*!
 *
 */
u8 copro_get_address(COPRO_INTERFACE* p_copro);

/*!
 *
 */
u8 copro_mutex_request(COPRO_INTERFACE* p_copro);

/*!
 *
 */
void copro_mutex_release(COPRO_INTERFACE* p_copro);

//-----------------------------------------------------------------------------

#endif // _COPRO_INTERFACE_H_