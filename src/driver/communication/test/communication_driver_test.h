#ifndef _COM_DRIVER_TEST_H_
#define _COM_DRIVER_TEST_H_

#include "driver/cfg_driver_interface.h"

void test_driver_initialize(void);

void test_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg);

void test_driver_power_off(void);

u8 test_driver_bytes_available (void);

u8 test_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to);

u8 test_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from);

u8 test_driver_is_ready_for_tx (void);

u8 test_driver_is_ready_for_rx(void);

void test_driver_start_rx (u16 num_of_rx_bytes);

void test_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms);

void test_driver_stop_rx (void);

void test_driver_start_tx (void);

void test_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms);

void test_driver_stop_tx (void);

void test_driver_clear_rx_buffer (void);

void test_driver_clear_tx_buffer (void);

void test_driver_set_address (u8 addr);

u8 test_driver_mutex_request(void);

void test_driver_mutex_release(u8 m_id);

void test_driver_set_rx_bytes(u8 num_bytes, u8* p_buffer_from);

u8 test_driver_get_tx_bytes(u8 num_bytes, u8* p_buffer_to);

u8 test_driver_get_tx_bytes_num_available(void);

#endif // _COM_DRIVER_TEST_H_
