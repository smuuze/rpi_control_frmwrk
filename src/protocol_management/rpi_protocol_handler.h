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
 * @file    rpi_protocol_handler.h
 * @author  Sebastian Lesse
 * @date    2023 / 09 / 09
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_rpi_protocol_handler_
#define _H_rpi_protocol_handler_

// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "mcu_task_management/mcu_task_interface.h"
#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

#define RPI_CONTROL_PREFIX_CFG_STRING                               "COM"
#define RPI_PROTOCOL_SPEED_CFG_STRING                               "COM_SPEED_HZ"
#define RPI_PROTOCOL_DEVICE_CFG_STRING                              "COM_DEVICE"

// --------------------------------------------------------------------------------

/**
 * @brief Number of bytes used for the header
 * of a RPi-Protocol response telegram.
 * Current length is 3:
 *   - Byte 0: Number of following bytes
 *   - Byte 1: command code
 *   - Byte 2: status of answer
 */
#define RPI_PROTOCOL_HEADER_LENGTH_RESPONSE         3

// --------------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_DEFAULT_SPI_SPEED_HZ
#define RPI_PROTOCOL_HANDLER_DEFAULT_SPI_SPEED_HZ
#endif

// --------------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_DRIVER_CFG
#define RPI_PROTOCOL_HANDLER_DRIVER_CFG                                         \
    .is_master = COM_DRIVER_IS_SLAVE,               /*  u8 op_mode; */          \
    .mode = DRIVER_SPI_MODE_3,                      /*  u8 clk_phase; */        \
    .clk_divider = DRIVER_SPI_CLK_DEVIDER_4,        /*  u8 clk_divider; */      \
    .clk_double_speed = DRIVER_SPI_NO_DOUBLE_SPEED, /*  u8 clk_double_speed; */ \
    .data_order = DRIVER_SPI_DATA_ORDER_MSB,        /*  u8 data_order; */       \
    .interrupt_enable = COM_DRIVER_IRQ_ENABLE,      /*  u8 interrupt_enable; */ \
    .speed = DRIVER_SPI_SPEED_HZ_19200              /*  u32 speed; */
#endif

// --------------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_I2C_HANDLER_DRIVER_CFG
#define RPI_PROTOCOL_I2C_HANDLER_DRIVER_CFG                                     \
    COM_DRIVER_IS_SLAVE,                            /* u8 op_mode; */           \
    DRIVER_SPI_MODE_3,                              /* u8 clk_phase; */         \
    DRIVER_SPI_CLK_DEVIDER_4,                       /* u8 clk_divider; */       \
    DRIVER_SPI_NO_DOUBLE_SPEED,                     /* u8 clk_double_speed; */  \
    DRIVER_SPI_DATA_ORDER_MSB,                      /* u8 data_order; */        \
    COM_DRIVER_IRQ_DISABLE                          /* u8 interrupt_enable; */
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#ifndef RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS
#define RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS        1000
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_driver 
 */
void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver);

/**
 * @brief 
 * 
 */
void rpi_protocol_com_driver_rx_complete_callback(void);

/**
 * @brief 
 * 
 */
void rpi_protocol_com_driver_tx_complete_callback(void);

/**
 * @brief 
 * 
 * @return u8 
 */
u8 rpi_protocol_handler_get_actual_state(void);

// --------------------------------------------------------------------------------

#endif // _H_rpi_protocol_handler_

// --------------------------------------------------------------------------------
