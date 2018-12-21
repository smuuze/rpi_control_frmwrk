/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "local_msg_buffer.h"
#include "local_spi_driver.h"

#include "cfg_driver_interface.h"

#include "driver_specific_spi.h"
#include "local_spi_driver.h"
#include "local_module_status.h"
#include "local_mutex.h"

#define noTRACES
#include <traces.h>

#define noSPI_RX_TRACES
#define noSPI_TX_TRACES

#if defined TRACES_ENABLED && defined SPI_RX_TRACES
#define SPI_RX_PASS()			PASS()
#define SPI_RX_TRACE_byte(v)		TRACE_byte(v)
#define SPI_RX_TRACE_word(v)		TRACE_word(v)
#define SPI_RX_TRACE_long(v)		TRACE_long(v)
#define SPI_RX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define SPI_RX_PASS()
#define SPI_RX_TRACE_byte(v)
#define SPI_RX_TRACE_word(v)
#define SPI_RX_TRACE_long(v)
#define SPI_RX_TRACE_N(n,v)
#endif


#if defined TRACES_ENABLED && defined SPI_TX_TRACES
#define SPI_TX_PASS()			PASS()
#define SPI_TX_TRACE_byte(v)		TRACE_byte(v)
#define SPI_TX_TRACE_word(v)		TRACE_word(v)
#define SPI_TX_TRACE_long(v)		TRACE_long(v)
#define SPI_TX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define SPI_TX_PASS()
#define SPI_TX_TRACE_byte(v)
#define SPI_TX_TRACE_word(v)
#define SPI_TX_TRACE_long(v)
#define SPI_TX_TRACE_N(n,v)
#endif

BUILD_MUTEX(spi_mutex)

BUILD_LOCAL_MSG_BUFFER(static, __spi_tx_buffer, LOCAL_SPI_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static, __spi_rx_buffer, LOCAL_SPI_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

BUILD_MODULE_STATUS_FAST(spi_driver_status, 2)

#define SPI_DRIVER_STATUS_RX_ACTIVE	0
#define SPI_DRIVER_STATUS_TX_ACTIVE	1

void spi_driver_initialize(void) {

	PASS(); // spi_driver_initialize()

	SPI0_DISABLE_INTERFACE();

	__spi_rx_buffer_init();
	__spi_tx_buffer_init();
}

/*!
 *
 */
void spi_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	PASS(); // spi_driver_configure()

	//spi_driver_status_clear_all();

	SPI0_ENABLE_INTERFACE();

	SPI0_CLEAR_CONFIG();
	SPI0_SET_MODE(p_cfg->module.spi.op_mode);
	SPI0_SET_INTERRUPT(p_cfg->module.spi.interrupt_enable);
	SPI0_SET_DATA_ORDER(p_cfg->module.spi.data_order);
	SPI0_SET_CLOCK_POLARITY(p_cfg->module.spi.clk_polarity);
	SPI0_SET_CLOCK_PHASE(p_cfg->module.spi.clk_phase);
	SPI0_SET_CLOCK_RATE(p_cfg->module.spi.clk_divider);
	SPI0_SET_BYTE(DRIVER_SPI_PADDING_BYTE);

	SPI0_SET_ENABLED(DRIVER_SPI_ENABLED);
}


/*!
 *
 */
void spi_driver_power_off(void) {

	PASS(); // spi_driver_power_off()

	SPI0_SET_ENABLED(DRIVER_SPI_DISABLED);
	//spi_driver_status_clear_all();

	SPI0_DISABLE_INTERFACE();
}


u8 spi_driver_bytes_available(void) {

	#if defined TRACES_ENABLED && defined SPI_RX_TRACES
	{
		u8 bytes_available = __spi_rx_buffer_bytes_available();
		if (bytes_available) {
			SPI_RX_TRACE_byte(bytes_available); // spi_driver_bytes_available()
		}
	}
	#endif

	return __spi_rx_buffer_bytes_available();
}


u8 spi_driver_get_N_bytes(u8 num_bytes, u8* p_buffer_to) {

	__spi_rx_buffer_start_read();
	u16 num_bytes_read = __spi_rx_buffer_get_N_bytes(num_bytes, p_buffer_to);
	__spi_rx_buffer_stop_read();

	SPI_TX_TRACE_N(num_bytes_read, p_buffer_to); // spi_driver_get_N_bytes()

	return num_bytes_read;
}


u8 spi_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > __spi_tx_buffer_size()) {
		num_bytes = __spi_tx_buffer_size();
	}

	SPI_TX_TRACE_N(num_bytes, p_buffer_from); // spi_driver_set_N_bytes()

	__spi_tx_buffer_start_write(); // this will delete all data added so far
	__spi_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	__spi_tx_buffer_stop_write();

	return num_bytes;
}

u8 spi_driver_is_ready_for_tx(void) {
	return spi_driver_status_is_set(SPI_DRIVER_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

u8 spi_driver_is_ready_for_rx(void) {
	return spi_driver_status_is_set(SPI_DRIVER_STATUS_RX_ACTIVE) == 0 ? 1 : 0;
}


void spi_driver_start_rx(u16 num_of_rx_bytes) {

	SPI_RX_PASS(); // spi_driver_start_rx()

	(void) num_of_rx_bytes;

	__spi_rx_buffer_start_write();
	spi_driver_status_set(SPI_DRIVER_STATUS_RX_ACTIVE);
}


void spi_driver_stop_rx(void) {

	SPI_RX_PASS(); // spi_driver_stop_rx()

	spi_driver_status_unset(SPI_DRIVER_STATUS_RX_ACTIVE);
	__spi_rx_buffer_stop_write();
}


void spi_driver_start_tx(void) {

	SPI_TX_PASS(); // spi_driver_start_tx()

	spi_driver_status_set(SPI_DRIVER_STATUS_TX_ACTIVE);
	__spi_tx_buffer_start_read();

	if (__spi_tx_buffer_bytes_available() == 0) {

		SPI_TX_PASS(); // spi_driver_start_tx() - No Data available
		SPI0_SET_BYTE(DRIVER_SPI_PADDING_BYTE);
		spi_driver_stop_tx();

		return;
	}

	SPI_TX_TRACE_byte(__spi_tx_buffer_bytes_available()); // spi_driver_start_tx() - Bytes to transmit

	u8 byte = __spi_tx_buffer_get_byte();
	SPI0_SET_BYTE(byte);
}


void spi_driver_stop_tx(void) {

	//SPI_TX_PASS(); // spi_driver_stop_tx()

	__spi_tx_buffer_stop_read();
	spi_driver_status_unset(SPI_DRIVER_STATUS_TX_ACTIVE);
}


void spi_driver_clear_buffer(void) {

	//PASS(); // spi_driver_clear_buffer()

	__spi_rx_buffer_clear_all();
	__spi_tx_buffer_clear_all();
}

void spi_driver_set_address (u8 addr) {
	(void) addr;
}

u8 spi_driver_mutex_request(void) {

	if (spi_mutex_is_requested() != 0) {
		return MUTEX_INVALID_ID;
	}

	return spi_mutex_request();
}

void spi_driver_mutex_release(u8 m_id) {
	spi_mutex_release(m_id);
}


ISR(SPI_STC_vect) {

	PASS(); // ISR(SPI_STC_vect) -----------------------------------------------------------------

	u8 rx_byte = SPI0_GET_BYTE();

	if (spi_driver_status_is_set(SPI_DRIVER_STATUS_TX_ACTIVE)) {

		u8 tx_byte = __spi_tx_buffer_get_byte();
		SPI0_SET_BYTE(tx_byte);

		SPI_RX_TRACE_byte(tx_byte); // ISR(SPI_STC_vect) - new byte transmitted

		if (__spi_tx_buffer_bytes_available() == 0) {
			spi_driver_stop_tx();
		}

	} else {
		SPI0_SET_BYTE(DRIVER_SPI_PADDING_BYTE);
	}

	if (spi_driver_status_is_set(SPI_DRIVER_STATUS_RX_ACTIVE)) {
		SPI_RX_TRACE_byte(rx_byte); // ISR(SPI_STC_vect) - new byte received
		__spi_rx_buffer_add_byte(rx_byte);
	}
}


