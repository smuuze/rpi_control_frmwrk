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
 * @file    usart0_driver_RP2040.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 26
 * @brief   Implementation of a uart0 driver ussable for the Raspberry Pi Pico
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

#include "rp2040_reset.h"
#include "rp2040_clock_frequencies.h"

// --------------------------------------------------------------------------------

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "driver/communication/usart/usart0_driver.h"
#include "local_module_status.h"

// --------------------------------------------------------------------------------

#define USART_DRIVER_RX_UNLIMITED           0xFFFF

// --------------------------------------------------------------------------------

/**
 * @brief Deepth of the uart1 fifo of the rp2040
 * in number of bytes
 * 
 */
#define RP2040_UART0_FIFO_SIZE              __UNSIGNED(0)

// --------------------------------------------------------------------------------

#define USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER     __UNSIGNED(8)
#define USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER      __UNSIGNED(8)

// --------------------------------------------------------------------------------

#define LOCAL_USART_STATUS_RX_ACTIVE        __UNSIGNED(0)
#define LOCAL_USART_STATUS_TX_ACTIVE        __UNSIGNED(1)

BUILD_MODULE_STATUS_FAST_VOLATILE(UART0_STATUS_REG, 2)

// --------------------------------------------------------------------------------

#define UART_DRIVER_UARTCR_UART_ENABLE          __UNSIGNED(0x00000001)
#define UART_DRIVER_UARTCR_TX_ENABLE            __UNSIGNED(0x00000100)
#define UART_DRIVER_UARTCR_RX_ENABLE            __UNSIGNED(0x00000200)

#define UART_DRIVER_UARTLCR_H_PARITY_NONE       __UNSIGNED(0x00000000)
#define UART_DRIVER_UARTLCR_H_PARITY_EN         __UNSIGNED(0x00000002)
#define UART_DRIVER_UARTLCR_H_PARITY_EVEN       (__UNSIGNED(0x00000004) | UART_DRIVER_UARTLCR_H_PARITY_EN)
#define UART_DRIVER_UARTLCR_H_PARITY_ODD        UART_DRIVER_UARTLCR_H_PARITY_EN
#define UART_DRIVER_UARTLCR_H_FIFO_ENABLE       __UNSIGNED(0x00000010)

#define UART_DRIVER_UARTLCR_H_STOPBITS_1        __UNSIGNED(0x00000000)
#define UART_DRIVER_UARTLCR_H_STOPBITS_2        __UNSIGNED(0x00001000)

#define UART_DRIVER_UARTLCR_H_DATA_BITS_8       __UNSIGNED(0x01100000)
#define UART_DRIVER_UARTLCR_H_DATA_BITS_7       __UNSIGNED(0x01000000)
#define UART_DRIVER_UARTLCR_H_DATA_BITS_6       __UNSIGNED(0x00100000)
#define UART_DRIVER_UARTLCR_H_DATA_BITS_5       __UNSIGNED(0x00000000)

#define UART_DRIVER_UARTDMACR_TX_DMA_EN         __UNSIGNED(0x00000002)
#define UART_DRIVER_UARTDMACR_RX_DMA_EN         __UNSIGNED(0x00000001)

#define UART_DRIVER_UARTFR_TXFF_BITS            __UNSIGNED(0x00000020)

#define UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_1_8  __UNSIGNED(0x00000000)
#define UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_1_4  __UNSIGNED(0x00000001)
#define UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_1_2  __UNSIGNED(0x00000002)
#define UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_3_4  __UNSIGNED(0x00000003)
#define UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_7_8  __UNSIGNED(0x00000004)

#define UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_1_8  __UNSIGNED(0x00000000)
#define UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_1_4  __UNSIGNED(0x00000008)
#define UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_1_2  __UNSIGNED(0x00000010)
#define UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_3_4  __UNSIGNED(0x00000018)
#define UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_7_8  __UNSIGNED(0x00000020)

// --------------------------------------------------------------------------------

/**
 * @brief FIFO TX-threshold at which the IRQ is activated.
 * Can be set via config.h by the user-application.
 * The default value is 1/2
 * 
 */
#ifndef RP2040_UART_DRIVER_FIFO_TX_LEVEL
#define RP2040_UART_DRIVER_FIFO_TX_LEVEL    UART_DRIVER_UARTIFLS_TX_FIFO_LEVEL_1_2
#endif

#ifndef RP2040_UART_DRIVER_FIFO_RX_LEVEL
#define RP2040_UART_DRIVER_FIFO_RX_LEVEL    UART_DRIVER_UARTIFLS_RX_FIFO_LEVEL_1_2
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Structure to control the UART register of the RP2040
 * @see RP2040 Datasheet (build-date: 2021-03-05 / build-version: 9bf4a25-clean)
 * 
 */
typedef struct {

    /**
     * @brief Data Register, UARTDR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 447
     * for more details.
     */
    io_rw_32 data;

    /**
     * @brief Receive Status Register/Error Clear Register, UARTRSR/UARTECR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 448
     * for more details.
     * 
     */
    io_rw_32 rsr;

    /**
     * @brief Respest the offset of the UARTFR-register.
     * Do not use this.
     */
    uint32_t _pad0[4];

    /**
     * @brief Flag Register, UARTFR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 449
     * for more details.
     */
    io_ro_32 fr;

    /**
     * @brief Respest the offset of the UARTILPR Register
     * Do not use this.
     */
    uint32_t _pad1;

    /**
     * @brief IrDA Low-Power Counter Register, UARTILPR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 450
     * for more details.
     */
    io_rw_32 ilpr;

    /**
     * @brief Integer Baud Rate Register, UARTIBRD
     * See datasheet ch. 4.2.8 - UARTDR Register on page 450
     * for more details.
     */
    io_rw_32 ibrd;

    /**
     * @brief Fractional Baud Rate Register, UARTFBRD
     * See datasheet ch. 4.2.8 - UARTDR Register on page 450
     * for more details.
     */
    io_rw_32 fbrd;

    /**
     * @brief Line Control Register, UARTLCR_H
     * See datasheet ch. 4.2.8 - UARTDR Register on page 451
     * for more details.
     */
    io_rw_32 lcr_h;

    /**
     * @brief Control Register, UARTCR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 452
     * for more details.
     */
    io_rw_32 cr;

    /**
     * @brief Interrupt FIFO Level Select Register, UARTIFLS
     * See datasheet ch. 4.2.8 - UARTDR Register on page 453
     * for more details.
     */
    io_rw_32 ifls;

    /**
     * @brief Interrupt Mask Set/Clear Register, UARTIMSC
     * See datasheet ch. 4.2.8 - UARTDR Register on page 453
     * for more details.
     */
    io_rw_32 imsc;

    /**
     * @brief Raw Interrupt Status Register, UARTRIS
     * See datasheet ch. 4.2.8 - UARTDR Register on page 455
     * for more details.
     */
    io_ro_32 ris;

    /**
     * @brief Masked Interrupt Status Register, UARTMIS
     * See datasheet ch. 4.2.8 - UARTDR Register on page 456
     * for more details.
     */
    io_ro_32 mis;

    /**
     * @brief Interrupt Clear Register, UARTICR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 457
     * for more details.
     */
    io_rw_32 icr;

    /**
     * @brief DMA Control Register, UARTDMACR
     * See datasheet ch. 4.2.8 - UARTDR Register on page 457
     * for more details.
     */
    io_rw_32 dmacr;

} RP2040_UART_REG;

// --------------------------------------------------------------------------------

/**
 * @brief transmit and receive buffer of the uart1 instance of the rp2040
 * Both buffers must only b used within the UART0 isntance.
 * 
 */

BUILD_LOCAL_MSG_BUFFER(
    static inline,
    USART0_TX_BUFFER,
    USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER
)

BUILD_LOCAL_MSG_BUFFER(
    static inline,
    USART0_RX_BUFFER,
    USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER
)

// --------------------------------------------------------------------------------

/**
 * @brief Returns pointer to access the uart0 register
 * 
 * @return Pointer to the uart0 register
 */
static inline RP2040_UART_REG* uart_driver_get_uart0_reg(void) {
    return ((RP2040_UART_REG*)RP2040_UART0_REG_BASE_ADDRESS);
}

/**
 * @brief Returns pointer to access the uart1 register
 * 
 * @return Pointer to the uart1 register
 */
static inline RP2040_UART_REG* uart_driver_get_uart1_reg(void) {
    return ((RP2040_UART_REG*)RP2040_UART1_REG_BASE_ADDRESS);
}

// --------------------------------------------------------------------------------

/**
 * @brief Configures the given baudrate.
 * 
 * @param p_uart refefernce to the uart-instance where to set the baudrate
 * @param baudrate baudrate to be sued
 * @return -1 configuring the baudrate has failed,
 * otherwise baudrate was configured successful.
 */
static i32 uart_driver_configure_baudrate(RP2040_UART_REG* p_uart, u32 baudrate) {

    if (clock_driver_peripheral_clk_frequency() == 0) {
        // something went wrong
        return -1;
    }

    u32 baud_rate_div = (8 * clock_driver_peripheral_clk_frequency() / baudrate);
    u32 baud_ibrd = baud_rate_div >> 7;
    u32 baud_fbrd;

    if (baud_ibrd == __UNSIGNED(0)) {
        baud_ibrd = __UNSIGNED(1);
        baud_fbrd = __UNSIGNED(0);
    } else if (baud_ibrd >= __UNSIGNED(65535)) {
        baud_ibrd = __UNSIGNED(65535);
        baud_fbrd = __UNSIGNED(0);
    }  else {
        baud_fbrd = ((baud_rate_div & 0x7f) + 1) / 2;
    }

    // Load PL011's baud divisor registers
    p_uart->ibrd = baud_ibrd;
    p_uart->fbrd = baud_fbrd;

    // PL011 needs a (dummy) line control register write to latch in the
    // divisors. We don't want to actually change LCR contents here.
    cpu_atomic_bit_set(&p_uart->lcr_h, __UNSIGNED(0));

    // See datasheet
    return (4 * clock_driver_peripheral_clk_frequency()) / (64 * baud_ibrd + baud_fbrd);
}

// --------------------------------------------------------------------------------

/**
 * @brief Configures the given uart instance depending on the given configuration
 * 
 * @param p_uart pointer to the uart instance to be configured 
 * @param p_cfg configuration to be used to configure the uart instance
 */
static void uart_driver_configure(
    RP2040_UART_REG* p_uart, TRX_DRIVER_CONFIGURATION* p_cfg
) {

    /**
     * @brief Baudrate to use.
     * By default we use 9600 baud.
     */
    u32 baudrate = __UNSIGNED(9600);

    switch (p_cfg->module.usart.baudrate) {

        default:
            // no break;
            // fall through

        case BAUDRATE_9600:
            // already set
            break;

        case BAUDRATE_19200:
            baudrate = __UNSIGNED(19200);
            break;

        case BAUDRATE_38400:
            baudrate = __UNSIGNED(38400);
            break;

        case BAUDRATE_115200:
            baudrate = __UNSIGNED(115200);
            break;

        case BAUDRATE_230400:
            baudrate = __UNSIGNED(230400);
            break;
    }

    /**
     * @brief Setup the following configuration values
     * and set them all together to the lcr_h register
     * of the actual uart-instance
     * - number of stopbits
     * - parity
     * - number of databits
     */
    u32 lcr_h_cfg = __UNSIGNED(0);

    switch (p_cfg->module.usart.parity) {

        default:
            // no break
            // fall through

        case PARITY_NONE : 
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_PARITY_NONE;
            break;

        case PARITY_EVEN:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_PARITY_EVEN;
            break;

        case PARITY_ODD:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_PARITY_ODD;
            break;
    }

    switch (p_cfg->module.usart.databits) {
        default:
            // no break
            // fall through

        case DATABITS_9:
            // not available
            // will use 8 databits here
            // no break
            // fall through

        case DATABITS_8:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_DATA_BITS_8;
            break;

        case DATABITS_5:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_DATA_BITS_5;
            break;

        case DATABITS_6:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_DATA_BITS_6;
            break;

        case DATABITS_7:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_DATA_BITS_7;
            break;
    }

    switch (p_cfg->module.usart.databits) {
        default:
            // no break;
            // fall through

        case STOPBITS_1:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_STOPBITS_1;
            break;

        case STOPBITS_2:
            lcr_h_cfg |= UART_DRIVER_UARTLCR_H_STOPBITS_1;
            break;
    }

    // Any LCR writes need to take place before enabling the UART
    // this function must be called before lcr_h is set
    u32 baud = uart_driver_configure_baudrate(p_uart, baudrate);

    cpu_atomic_bit_set(&p_uart->lcr_h, lcr_h_cfg);

    // set the FIFO threshold levels
    cpu_atomic_bit_set(
        &p_uart->ifls,
        RP2040_UART_DRIVER_FIFO_TX_LEVEL | RP2040_UART_DRIVER_FIFO_RX_LEVEL
    );

    // activate IRQs (TX / RX)

    // Enable the UART, both TX and RX
    p_uart->cr = UART_DRIVER_UARTCR_UART_ENABLE
               | UART_DRIVER_UARTCR_TX_ENABLE
               | UART_DRIVER_UARTCR_RX_ENABLE;
    
    // Enable FIFOs
    cpu_atomic_bit_set(&p_uart->lcr_h, UART_DRIVER_UARTLCR_H_FIFO_ENABLE);

    // Always enable DREQ signals -- no harm in this if DMA is not listening
    p_uart->dmacr = UART_DRIVER_UARTDMACR_TX_DMA_EN
                  | UART_DRIVER_UARTDMACR_RX_DMA_EN;
}

// --------------------------------------------------------------------------------

/**
 * @brief Check if the given uart-inatance is able to accept a new data-byte
 * for transmission.
 * 
 * @param p_uart_inst pointer to the uart-instance to use
 */
static inline u8 usart_driver_is_ready_for_tx(RP2040_UART_REG* p_uart_inst) {

    /**
     * @brief if the transmit FIFO full flag is set
     * the FIFO cannot accept another data-byte.
     * Otherwise one more byte can be stored.
     * 
     */
    return !(p_uart_inst->fr & UART_DRIVER_UARTFR_TXFF_BITS);
}

// --------------------------------------------------------------------------------

/**
 * @brief Copies num_bytes (as maximum) bytes from p_buffer_from into the hw-fifo
 * of p_uart_inst. If not all bytes fit into the hw-fifo the remaining bytes will
 * not be copied.
 * 
 * @param p_uart_inst reference to the uart-instance where to write the data into
 * @param num_bytes number of bytes to write
 * @param p_buffer_from buffer where to read from
 * @return the number of bytes that have been copied into the uart-fifo
 */
static u16 uart_driver_add_bytes_to_fifo(
    RP2040_UART_REG* p_uart_inst, u32 num_bytes, const u8* const p_buffer_from
) {

    u32 i = __UNSIGNED(0);
    for ( ; i < num_bytes; ++i) {

        if (!usart_driver_is_ready_for_tx(p_uart_inst)) {
            break;
        }

        p_uart_inst->data = p_buffer_from[i];
    }

    return i;
}

// --------------------------------------------------------------------------------

/**
 * @see usart0_driver.h#usart0_driver_initialize
 */
void usart0_driver_initialize(void) {

    UART0_STATUS_REG_clear_all();
    USART0_TX_BUFFER_clear_all();
    USART0_RX_BUFFER_clear_all();

    rp2040_reset_uart0();// blocks until uart0 is resetted
}

/**
 * @see usart0_driver.h#usart0_driver_configure
 */
void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {
    uart_driver_configure(uart_driver_get_uart0_reg(), p_cfg);
}

/**
 * @see usart0_driver.h#usart0_driver_power_off
 */
void usart0_driver_power_off(void) {

}

/**
 * @see usart0_driver.h#usart0_driver_bytes_available
 */
u16 usart0_driver_bytes_available (void) {
    return 0;
}

/**
 * @see usart0_driver.h#usart0_driver_get_N_bytes
 */
u16 usart0_driver_get_N_bytes (u16 num_bytes, u8* p_buffer_to) {
    return 0;
}

/**
 * @see usart0_driver.h#usart0_driver_set_N_bytes
 */
u16 usart0_driver_set_N_bytes(u16 num_bytes, const u8* const p_buffer_from) {

    /**
     * @brief try to write as much as possible into the uarts hw-fifo
     */
    u16 bytes_written = uart_driver_add_bytes_to_fifo(
        uart_driver_get_uart0_reg(),
        num_bytes,
        &p_buffer_from[0]
    );

    /**
     * @brief try to write the remaining bytes into the sw-buffer
     */
    USART0_TX_BUFFER_start_write();
    {
        bytes_written += USART0_TX_BUFFER_add_N_bytes(
            num_bytes - bytes_written,
            &p_buffer_from[bytes_written]
        );
    }
    USART0_TX_BUFFER_stop_write();
    
    /**
     * @brief return the number of bytes that have been stored
     * to inform the user if some of them have not.
     */
    return bytes_written;
}

/**
 * @see usart0_driver.h#usart0_driver_is_ready_for_rx
 */
u8 usart0_driver_is_ready_for_rx(void) {
    return 1;
}

/**
 * @see usart0_driver.h#usart0_driver_start_rx
 */
void usart0_driver_start_rx (u16 num_of_rx_bytes) {

}

/**
 * @see usart0_driver.h#usart0_driver_wait_for_rx
 */
void usart0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {
    (void) num_bytes;
    (void) timeout_ms;
}

/**
 * @see usart0_driver.h#usart0_driver_stop_rx
 */
void usart0_driver_stop_rx (void) {

}

/**
 * @see usart0_driver.h#usart0_driver_is_ready_for_tx
 */
u8 usart0_driver_is_ready_for_tx (void) {
    return usart_driver_is_ready_for_tx(uart_driver_get_uart0_reg()) ;
}

/**
 * @see usart0_driver.h#usart0_driver_start_tx
 */
void usart0_driver_start_tx (void) {

}

/**
 * @see usart0_driver.h#usart0_driver_wait_for_tx
 */
void usart0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
    (void) num_bytes;
    (void) timeout_ms;
}

/**
 * @see usart0_driver.h#usart0_driver_stop_tx
 */
void usart0_driver_stop_tx (void) {

}

/**
 * @see usart0_driver.h#usart0_driver_clear_rx_buffer
 */
void usart0_driver_clear_rx_buffer(void) {

}

/**
 * @see usart0_driver.h#usart0_driver_clear_tx_buffer
 */
void usart0_driver_clear_tx_buffer(void) {
    
}

/**
 * @see usart0_driver.h#usart0_driver_set_address
 */
void usart0_driver_set_address (u8 addr) {
    (void) addr;
}

/**
 * @see usart0_driver.h#usart0_driver_mutex_request
 */
u8 usart0_driver_mutex_request(void) {
    return 1;
}

/**
 * @see usart0_driver.h#usart0_driver_mutex_release
 */
void usart0_driver_mutex_release(u8 m_id) {
    (void) m_id;
}

// --------------------------------------------------------------------------------

/**
 * @brief UART0 IRQ Handler
 * 
 */
void IRQ_20_Handler(void) {

    // read the status-reg to find out which IRQ has been fired
}

// --------------------------------------------------------------------------------
