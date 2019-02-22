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
#include "io_controller.h"

#include "cfg_driver_interface.h"

#include "driver_specific_spi.h"
#include "local_spi_driver.h"
#include "local_module_status.h"
#include "local_mutex.h"

#define noTRACES
#include <traces.h>

#define noSPI_RX_TRACES
#define noSPI_TX_TRACES
#define noSPI_ISR_TRACES

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


#if defined TRACES_ENABLED && defined SPI_ISR_TRACES
#define SPI_ISR_PASS()			PASS()
#define SPI_ISR_TRACE_byte(v)		TRACE_byte(v)
#define SPI_ISR_TRACE_word(v)		TRACE_word(v)
#define SPI_ISR_TRACE_long(v)		TRACE_long(v)
#define SPI_ISR_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define SPI_ISR_PASS()
#define SPI_ISR_TRACE_byte(v)
#define SPI_ISR_TRACE_word(v)
#define SPI_ISR_TRACE_long(v)
#define SPI_ISR_TRACE_N(n,v)
#endif

BUILD_MUTEX(spi_mutex)

BUILD_LOCAL_MSG_BUFFER( , __spi_tx_buffer, LOCAL_SPI_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER( , __spi_rx_buffer, LOCAL_SPI_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

BUILD_MODULE_STATUS_FAST(spi_driver_status, 2)

IO_CONTROLLER_BUILD_INOUT(SPI_CE, HOST_SPI_CE)
IO_CONTROLLER_BUILD_INOUT(SPI_SCK, HOST_SPI_SCK)
IO_CONTROLLER_BUILD_INOUT(SPI_MOSI, HOST_SPI_MOSI)
IO_CONTROLLER_BUILD_INOUT(SPI_MISO, HOST_SPI_MISO)

SPI0_BUILD_CFG()

#define SPI_DRIVER_STATUS_RX_ACTIVE	0
#define SPI_DRIVER_STATUS_TX_ACTIVE	1

/*!
 *
 */
static volatile u8 _status_register = 0;

void spi_driver_initialize(void) {

	PASS(); // spi_driver_initialize()

	SPI_CE_init();
	SPI_SCK_init();
	SPI_MOSI_init();
	SPI_MISO_init();

	__spi_rx_buffer_init();
	__spi_tx_buffer_init();

	SPI0_CLEAR_CONFIG();
	SPI0_ENABLE_IRQ();
	SPI0_ENABLE_MODULE();
	SPI0_RESET_REGISTERS();

	SPI0_DISABLE_MODULE();
	SPI0_POWER_DOWN();

	{
		u8 temp = SPCR;
		TRACE_byte(temp); // SPI - CONTROL-REGISTER

		temp = SPSR;
		TRACE_byte(temp); // SPI - STATUS-REGISTER

		temp = PRR0;
		TRACE_byte(temp); // SPI - PRR0-REGISTER

		temp = PRR1;
		TRACE_byte(temp); // SPI - PRR1-REGISTER
	}
}

/*!
 *
 */
void spi_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	PASS(); // spi_driver_configure() - Start ---

	SPI0_POWER_UP();

	PASS(); // spi_driver_configure() - Apply ---

	if (p_cfg->module.spi.is_master != 0) {

		SPI0_ENABLE_MASTER_MODE(); PASS(); // spi_driver_configure()
		SPI_CE_drive_high();
		SPI_SCK_drive_high();
		SPI_MOSI_no_drive();
		SPI_MISO_no_pull();

	} else {

		SPI0_DISABLE_MASTER_MODE(); PASS(); // spi_driver_configure()
		SPI_CE_pull_up();
		SPI_SCK_pull_up();
		SPI_MOSI_no_pull();
		SPI_MISO_drive_low();
	}

	if (p_cfg->module.spi.data_order != 0) {
		SPI0_ENABLE_DATA_ORDER_LSB(); PASS(); // spi_driver_configure()
	} else {
		SPI0_DISABLE_DATA_ORDER_LSB(); PASS(); // spi_driver_configure()
	}

	if (p_cfg->module.spi.clk_double_speed != 0) {
		SPI0_ENABLE_DOUBLE_SPEED(); PASS(); // spi_driver_configure()
	} else {
		SPI0_DISABLE_DOUBLE_SPEED(); PASS(); // spi_driver_configure()
	}

	if (p_cfg->module.spi.interrupt_enable != 0) {
		SPI0_ENABLE_IRQ();
	} else {
		SPI0_DISABLE_IRQ();
	}

	switch (p_cfg->module.spi.mode) {

		default : // no break;
		case DRIVER_SPI_MODE_0 :
			SPI0_ENABLE_SPI_MODE_0(); PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_MODE_1 :
			SPI0_ENABLE_SPI_MODE_1(); PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_MODE_2 :
			SPI0_ENABLE_SPI_MODE_2(); PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_MODE_3 :
			SPI0_ENABLE_SPI_MODE_3(); PASS(); //spi_driver_configure()
			break;
	}

	TRACE_byte(p_cfg->module.spi.clk_divider); // spi_driver_configure()
	switch (p_cfg->module.spi.clk_divider) {

		default : // no break;
		case DRIVER_SPI_CLK_DEVIDER_4  :
			SPI0_ENABLE_CLOCK_DEVIDER(DRIVER_SPI_CLK_DEVIDE_BY_4);  PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_CLK_DEVIDER_8  :
			SPI0_ENABLE_CLOCK_DEVIDER(DRIVER_SPI_CLK_DEVIDE_BY_8);  PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_CLK_DEVIDER_16 :
			SPI0_ENABLE_CLOCK_DEVIDER(DRIVER_SPI_CLK_DEVIDE_BY_16); PASS(); // spi_driver_configure()
			break;

		case DRIVER_SPI_CLK_DEVIDER_32 :
			SPI0_ENABLE_CLOCK_DEVIDER(DRIVER_SPI_CLK_DEVIDE_BY_32); PASS(); // spi_driver_configure()
			break;
	}

	SPI0_ENABLE_MODULE();
	SPI0_RESET_REGISTERS();

	{
		u8 temp = SPCR;
		TRACE_byte(temp); // SPI - CONTROL-REGISTER

		temp = SPSR;
		TRACE_byte(temp); // SPI - STATUS-REGISTER

		temp = PRR0;
		TRACE_byte(temp); // SPI - PRR0-REGISTER

		temp = PRR1;
		TRACE_byte(temp); // SPI - PRR1-REGISTER

		temp = SREG;
		TRACE_byte(temp); // Global Status-Register
	}

	PASS(); // spi_driver_configure() - End ---
}


/*!
 *
 */
void spi_driver_power_off(void) {

	PASS(); // spi_driver_power_off()

	SPI0_DISABLE_MODULE();
	SPI0_POWER_DOWN();

	SPI_CE_pull_up();
	SPI_SCK_pull_up();
	SPI_MOSI_no_pull();
	SPI_MISO_no_pull();
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

	u8 bytes_available = __spi_rx_buffer_bytes_available();
	TRACE_byte(bytes_available); // spi_driver_bytes_available()

	return bytes_available;
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

void spi_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {

	if (num_bytes == 0) {
		PASS(); // spi_driver_wait_for_rx() - Nothing to receive
		return;
	}

	TRACE_byte(num_bytes); // spi_driver_wait_for_rx()

	u8 bytes_received = 0;
	u16 time_reference_ms = i_system.time.now_u16();

	while (i_system.time.isup_u16(time_reference_ms, timeout_ms) == 0) {

		if (SPI0_IS_TRX_COMPLETE() != 0) {

			u8 rx_byte = SPI0_GET_BYTE();
			__spi_rx_buffer_add_byte(rx_byte);
			bytes_received += 1;

			SPI_RX_TRACE_byte(rx_byte); // ISR(SPI_STC_vect) - new byte received

			if (spi_driver_status_is_set(SPI_DRIVER_STATUS_TX_ACTIVE)) {

				u8 tx_byte = __spi_tx_buffer_get_byte();
				SPI0_SET_BYTE(tx_byte);

				if (__spi_tx_buffer_bytes_available() == 0) {
					spi_driver_stop_tx();
				}

			} else {
				SPI0_SET_BYTE(DRIVER_SPI_PADDING_BYTE);
			}

			if (bytes_received >= num_bytes) {
				break;
			}
		}
	}
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

	} else {

		SPI_TX_TRACE_byte(__spi_tx_buffer_bytes_available()); // spi_driver_start_tx() - Bytes to transmit
		u8 byte = __spi_tx_buffer_get_byte();
		SPI0_SET_BYTE(byte);
	}
}

void spi_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {

	if (num_bytes > __spi_tx_buffer_bytes_available()) {
		num_bytes = __spi_tx_buffer_bytes_available();
	}

	if (num_bytes == 0) {
		PASS(); // spi_driver_wait_for_tx() - Nothing to transmit
		return;
	}

	TRACE_byte(num_bytes); // spi_driver_wait_for_tx()

	u8 bytes_transmitted = 0;
	u16 time_reference_ms = i_system.time.now_u16();

	while (i_system.time.isup_u16(time_reference_ms, timeout_ms) == 0) {

		if (SPI0_IS_TRX_COMPLETE() != 0) {

			u8 rx_byte = SPI0_GET_BYTE();

			if (spi_driver_status_is_set(SPI_DRIVER_STATUS_RX_ACTIVE)) {
				__spi_rx_buffer_add_byte(rx_byte);
				bytes_transmitted += 1;
			}

			u8 tx_byte = __spi_tx_buffer_get_byte();
			SPI0_SET_BYTE(tx_byte);

			SPI_TX_TRACE_byte(tx_byte); // ISR(SPI_STC_vect) - new byte transmitted

			if (bytes_transmitted >= num_bytes) {
				break;
			}
		}
	}
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

	SPCR &= ~DRIVER_SPI_CFGMASK_IRQ_EN;

	_status_register = SPI0_GET_STATUS();
	u8 rx_byte = SPI0_GET_BYTE();

	SPI_ISR_TRACE_byte(_status_register); // ISR(SPI_STC_vect) -----------------------------------------------------------------

	if (spi_driver_status_is_set(SPI_DRIVER_STATUS_TX_ACTIVE)) {

		u8 tx_byte = __spi_tx_buffer_get_byte();
		SPI0_SET_BYTE(tx_byte);

		SPI_ISR_TRACE_byte(tx_byte); // ISR(SPI_STC_vect) - new byte transmitted

		if (__spi_tx_buffer_bytes_available() == 0) {
			spi_driver_stop_tx();
		}

	} else {
		SPI0_SET_BYTE(DRIVER_SPI_PADDING_BYTE);
	}

	if (spi_driver_status_is_set(SPI_DRIVER_STATUS_RX_ACTIVE)) {
		SPI_ISR_TRACE_byte(rx_byte); // ISR(SPI_STC_vect) - new byte received
		__spi_rx_buffer_add_byte(rx_byte);
	}
}


