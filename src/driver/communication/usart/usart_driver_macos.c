
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
 * @file    usart_driver_macos.c
 * @author  Sebastian Lesse
 * @date    2023 / 04 / 09
 * @brief   Short description of this file
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

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "driver/communication/usart/usart0_driver.h"
#include "local_module_status.h"
#include "time_management/time_management.h"
#include "mcu_task_management/thread_interface.h"

// --------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>        //Used for UART
#include <fcntl.h>        //Used for UART
#include <termios.h>        //Used for UART

// --------------------------------------------------------------------------------

#define USART_DRIVER_RX_UNLIMITED    0xFFFF

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER(static inline, USART0_TX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, USART0_RX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(USART0_TIMEOUT_TIMER)

// --------------------------------------------------------------------------------

#define USART0_STATUS_RX_ACTIVE        0
#define USART0_STATUS_TX_ACTIVE        1
#define USART0_STATUS_INITIALIZED    2
#define USART0_STATUS_TERMINATED    3

BUILD_MODULE_STATUS_FAST_VOLATILE(USART0_STATUS, 4)

// --------------------------------------------------------------------------------

static u16 remote_usart_rx_bytes = 0;

/**
 * @brief 
 * 
 */
static int device_handle_id = -1;

// --------------------------------------------------------------------------------

/**
 * @brief Must be called to build and initialize the thread-object befor it can be used.
 * 
 */
static void usart0_driver_macos_init(void);

/**
 * @brief runs in background in a seperate thread, reading incomming data from 
 * the usart interface
 * 
 */
static THREAD_INTERFACE_EXIT_STATUS usart0_driver_macos_background_read(void);

/**
 * @brief Termiantes the thread of this module
 * 
 */
static void usart0_driver_macos_thread_terminate(void);

// ------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(
    USART0_DRIVER_RPI_RX_THREAD,
    THREAD_PRIORITY_MIDDLE,
    usart0_driver_macos_init,
    usart0_driver_macos_background_read,
    usart0_driver_macos_thread_terminate
)

// --------------------------------------------------------------------------------

void usart0_driver_initialize(void) {
    DEBUG_PASS("usart0_driver_initialize()");
    USART0_DRIVER_RPI_RX_THREAD_init();
}

void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

    if (USART0_STATUS_is_set(USART0_STATUS_INITIALIZED)) {
        DEBUG_PASS("usart0_driver_configure() - is already initialized");
        return;
    }

    DEBUG_TRACE_STR(p_cfg->device.name, "usart0_driver_configure() - Device:");

    USART0_RX_BUFFER_init();
    USART0_TX_BUFFER_init();

    //Open in non blocking read/write mode
    device_handle_id = open(p_cfg->device.name, O_RDONLY | O_NOCTTY | O_NDELAY);
    if (device_handle_id < 0) {
        DEBUG_TRACE_byte(
            device_handle_id,
            "usart0_driver_configure() - *** OPEN DEVICE FAILED - ERROR: ***"
        );
        return;
    }
    
    //  CONFIGURE THE UART
    //  The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
    //  Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    //  CSIZE:- CS5, CS6, CS7, CS8
    //  CLOCAL - Ignore modem status lines
    //  CREAD - Enable receiver
    //  IGNPAR = Ignore characters with parity errors
    //  ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
    //  PARENB - Parity enable
    //  PARODD - Odd parity (else even)
    struct termios options;
    
    if (tcgetattr(device_handle_id, &options) != 0) {
        DEBUG_PASS("usart0_driver_configure() - FAILED TO GET CURRENT CONFIGURATION");
        return;
    }

    options.c_cflag = CLOCAL | CREAD; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    switch (p_cfg->module.usart.baudrate) {
        default:
        case BAUDRATE_9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            DEBUG_PASS("usart0_driver_configure() - BAUDRATE_9600");
            break;

        case BAUDRATE_19200:
            cfsetispeed(&options, B19200);
            cfsetospeed(&options, B19200);
            DEBUG_PASS("usart0_driver_configure() - BAUDRATE_19200");
            break;

        case BAUDRATE_38400:
            cfsetispeed(&options, B38400);
            cfsetospeed(&options, B38400);
            DEBUG_PASS("usart0_driver_configure() - BAUDRATE_38400"); 
            break;

        case BAUDRATE_115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            DEBUG_PASS("usart0_driver_configure() - BAUDRATE_115200");
            break;

        case BAUDRATE_230400:
            cfsetispeed(&options, B230400);
            cfsetospeed(&options, B230400);
            DEBUG_PASS("usart0_driver_configure() - BAUDRATE_230400");
            break;
    }

    switch (p_cfg->module.usart.databits) {
        default:
        case DATABITS_8:    options.c_cflag |= CS8; DEBUG_PASS("usart0_driver_configure() - DATABITS_8"); break;
        case DATABITS_5:    options.c_cflag |= CS5; DEBUG_PASS("usart0_driver_configure() - DATABITS_5"); break;
        case DATABITS_6:    options.c_cflag |= CS6; DEBUG_PASS("usart0_driver_configure() - DATABITS_6"); break;
        case DATABITS_7:    options.c_cflag |= CS7; DEBUG_PASS("usart0_driver_configure() - DATABITS_7"); break;
        //case DATABITS_9:    options.c_cflag |= CS8; DEBUG_PASS("usart0_driver_configure()"); break;
    }

    switch (p_cfg->module.usart.stopbits) {
        default:
        case STOPBITS_1:    options.c_cflag &= ~CSTOPB; DEBUG_PASS("usart0_driver_configure() - STOPBITS_1"); break;
        case STOPBITS_2:    options.c_cflag |= CSTOPB; DEBUG_PASS("usart0_driver_configure() - STOPBITS_2"); break;
    }

    switch (p_cfg->module.usart.parity) {
        default:
        case PARITY_NONE:   options.c_cflag &= ~PARENB; DEBUG_PASS("usart0_driver_configure() - PARITY: NONE"); break;
        case PARITY_EVEN:   options.c_cflag |= PARENB; DEBUG_PASS("usart0_driver_configure() - PARITY: EVEN"); break;
        case PARITY_ODD:    options.c_cflag |= (PARENB | PARODD); DEBUG_PASS("usart0_driver_configure() - PARITY: ODD"); break;
    }

    tcflush(device_handle_id, TCIFLUSH);
    
    if (tcsetattr(device_handle_id, TCSANOW, &options) != 0) {
        DEBUG_PASS("usart0_driver_configure() - FAILED TO SET NEW CONFIGURATION");
        return;
    }

    USART0_STATUS_set(USART0_STATUS_INITIALIZED);

    USART0_DRIVER_RPI_RX_THREAD_start();
}

void usart0_driver_power_off(void) {
    DEBUG_PASS("usart0_driver_power_off()");
    USART0_RX_BUFFER_clear_all();
    USART0_TX_BUFFER_clear_all();

    USART0_DRIVER_RPI_RX_THREAD_terminate();
}

u16 usart0_driver_bytes_available (void) {

    DEBUG_CODE_BLOCK (
        u16 bytes_available = USART0_RX_BUFFER_bytes_available();
        if (bytes_available) {
            DEBUG_TRACE_word(bytes_available, "usart0_driver_bytes_available()");
        }
    )

    return USART0_RX_BUFFER_bytes_available();
}

u16 usart0_driver_get_N_bytes (u16 num_bytes, u8* p_buffer_to) {

    u16 num_bytes_available = USART0_RX_BUFFER_bytes_available();

    if (num_bytes < num_bytes_available) {
        num_bytes_available = num_bytes;
    }

    USART0_RX_BUFFER_start_read();
    USART0_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
    USART0_RX_BUFFER_stop_read();

    //DEBUG_TRACE_N(num_bytes_available, p_buffer_to, "usart0_driver_get_N_bytes()");

    return num_bytes_available;
}

u16 usart0_driver_set_N_bytes (u16 num_bytes, const u8* p_buffer_from) {

    if (num_bytes > USART0_TX_BUFFER_size()) {
        num_bytes = USART0_TX_BUFFER_size();
    }

    DEBUG_TRACE_N(num_bytes, p_buffer_from, "usart0_driver_set_N_bytes()");

    USART0_TX_BUFFER_start_write(); // this will delete all data added so far
    USART0_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
    USART0_TX_BUFFER_stop_write();

    return num_bytes;
}

u8 usart0_driver_is_ready_for_rx(void) {
    if (USART0_STATUS_is_set(USART0_STATUS_INITIALIZED)) {
        return 1;
    }

    return 0;
}

void usart0_driver_start_rx(u16 num_of_rx_bytes) {

    DEBUG_TRACE_word(num_of_rx_bytes, "usart0_driver_start_rx()");

    remote_usart_rx_bytes = num_of_rx_bytes;
    USART0_STATUS_set(USART0_STATUS_RX_ACTIVE);
}

void usart0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {

    DEBUG_TRACE_word(timeout_ms, "usart0_driver_wait_for_rx() - Timeout:");

    USART0_TIMEOUT_TIMER_start();

    while (USART0_RX_BUFFER_bytes_available() < num_bytes) {

        if (USART0_TIMEOUT_TIMER_is_up(timeout_ms)) {
            DEBUG_TRACE_word(USART0_TIMEOUT_TIMER_elapsed(), "usart0_driver_wait_for_rx() - TIMEOUT !!! ---");
            break;
        }

        usleep(50000); // reduce cpu-load
    }
}

void usart0_driver_stop_rx (void) {

    DEBUG_PASS("usart0_driver_stop_rx()");
    USART0_STATUS_unset(USART0_STATUS_RX_ACTIVE);
}

u8 usart0_driver_is_ready_for_tx (void) {

    return USART0_STATUS_is_set(USART0_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart0_driver_start_tx (void) {

    DEBUG_PASS("usart0_driver_start_tx()");

    USART0_STATUS_set(USART0_STATUS_TX_ACTIVE);
    USART0_TX_BUFFER_start_read();

    /*
    while (USART0_TX_BUFFER_bytes_available() > 0) {

        USART0_DRIVER_WAIT_UNTIL_TX_RDY();

        u8 byte = USART0_TX_BUFFER_get_byte();
        DEBUG_TRACE_byte(byte, "usart0_driver_start_tx");
        USART0_DRIVER_SET_BYTE(byte);
    }
    */

    USART0_TX_BUFFER_stop_read();
}

void usart0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
    (void) num_bytes;
    (void) timeout_ms;
}

void usart0_driver_stop_tx(void) {

    DEBUG_PASS("usart0_driver_stop_tx()");

    USART0_TX_BUFFER_stop_read();
    USART0_STATUS_unset(USART0_STATUS_TX_ACTIVE);
}

void usart0_driver_clear_rx_buffer(void) {
    USART0_RX_BUFFER_clear_all();
}

void usart0_driver_clear_tx_buffer(void) {
    USART0_TX_BUFFER_clear_all();
}

void usart0_driver_set_address(u8 addr) {
    (void) addr;
}

u8 usart0_driver_mutex_request(void) {
    return 1;
}

void usart0_driver_mutex_release(u8 m_id) {
    (void) m_id;
}

// --------------------------------------------------------------------------------

static void usart0_driver_macos_init(void) {
    DEBUG_PASS("usart0_driver_macos_init()");
}

static THREAD_INTERFACE_EXIT_STATUS usart0_driver_macos_background_read(void) {

    DEBUG_PASS("usart0_driver_macos_background_read() - Thread started");

    while (1) {

        usleep(10000);

        if (USART0_STATUS_is_set(USART0_STATUS_TERMINATED)) {
            break;
        }

        if (USART0_STATUS_is_set(USART0_STATUS_RX_ACTIVE) == 0) {
            continue;
        }

        unsigned char byte_array[1024];
        int length = read(device_handle_id, &byte_array[0], sizeof(byte_array));

        while (length > 0) {

            DEBUG_TRACE_N(length, byte_array, "usart0_driver_macos_background_read()");

            USART0_RX_BUFFER_start_write();
            USART0_RX_BUFFER_add_N_bytes(length, &byte_array[0]);
            USART0_RX_BUFFER_stop_write();

            length = read(device_handle_id, &byte_array[0], sizeof(byte_array));
        }
    }

    close(device_handle_id);
    USART0_STATUS_unset(USART0_STATUS_INITIALIZED);

    DEBUG_PASS("usart0_driver_macos_background_read() - FINISHED");
    return 0;
}

static void usart0_driver_macos_thread_terminate(void) {

    DEBUG_PASS("usart0_driver_macos_thread_terminate()");
    USART0_STATUS_set(USART0_STATUS_TERMINATED);
}
