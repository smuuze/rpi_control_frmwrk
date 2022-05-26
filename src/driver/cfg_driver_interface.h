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
 * @file    cfg_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 04 / 27
 * @brief   Short description of this file
 * 
 */

//-----------------------------------------------------------------------------

#ifndef _CFG_DRIVER_INTERFACE_H_
#define _CFG_DRIVER_INTERFACE_H_

//-----------------------------------------------------------------------------

#define COM_DRIVER_IS_MASTER                1
#define COM_DRIVER_IS_SLAVE                 0

#define COM_DRIVER_IRQ_ENABLE               1
#define COM_DRIVER_IRQ_DISABLE              0

//-----------------------------------------------------------------------------

#define DRIVER_SPI_DOUBLE_SPEED             1
#define DRIVER_SPI_NO_DOUBLE_SPEED          0

#define DRIVER_SPI_SPEED_HZ_1000            1000
#define DRIVER_SPI_SPEED_HZ_9600            9600
#define DRIVER_SPI_SPEED_HZ_19200           19200
#define DRIVER_SPI_SPEED_HZ_38400           38400
#define DRIVER_SPI_SPEED_HZ_76800           76800
#define DRIVER_SPI_SPEED_HZ_153600          153600

//----------------------------------------------------------------------------- 

#define DRIVER_CFG_DEVICE_NAME_MAX_LENGTH               96

//----------------------------------------------------------------------------- 

#define TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH        0xFFFF

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef void (*CFG_DRIVER_INTERFACE_HOST_RX_COMPLETE_CALLBACK)    (void);

/**
 * @brief 
 * 
 */
typedef void (*CFG_DRIVER_INTERFACE_HOST_TX_COMPLETE_CALLBACK)    (void);

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef enum {
    BAUDRATE_9600 = 0x00,
    BAUDRATE_19200,
    BAUDRATE_38400,
    BAUDRATE_115200,
    BAUDRATE_230400
} DRIVER_CFG_BAUDRATE;

/**
 * @brief 
 * 
 */
typedef enum {
    DATABITS_8 = 0x00,
    DATABITS_5,
    DATABITS_6,
    DATABITS_7,
    DATABITS_9
} DRIVER_CFG_DATABITS;

/**
 * @brief Enumeration to select the number of stop-bits 
 * to use in a USART driver module
 * 
 */
typedef enum {
    STOPBITS_1 = 0x00,
    STOPBITS_2
} DRIVER_CFG_STOPBITS;

/**
 * @brief Enumeration to select the parity to use
 * in a USART driver module
 * 
 */
typedef enum {
    PARITY_NONE = 0x00,
    PARITY_EVEN,
    PARITY_ODD,
} DRIVER_CFG_PARITY;

//-----------------------------------------------------------------------------

/**
 * @brief Enumeration to select the clock-devider
 * that is used for generating the spi clock-signal
 * 
 */
typedef enum {
    DRIVER_SPI_CLK_DEVIDER_4 = 0,
    DRIVER_SPI_CLK_DEVIDER_8 = 1,
    DRIVER_SPI_CLK_DEVIDER_16 = 2,
    DRIVER_SPI_CLK_DEVIDER_32 = 3,
    DRIVER_SPI_CLK_DEVIDER_64 = 4,
    DRIVER_SPI_CLK_DEVIDER_128 = 5
} DRIVER_CFG_CLK_DEVIDER;

/**
 * @brief Enumeration to select the SPI-Mode to use for operation
 * DRIVER_SPI_MODE_0 :  clock-polarity = 0 / clock-phase = 0
 * DRIVER_SPI_MODE_1 :  clock-polarity = 0 / clock-phase = 1
 * DRIVER_SPI_MODE_2 :  clock-polarity = 1 / clock-phase = 0
 * DRIVER_SPI_MODE_3 :  clock-polarity = 1 / clock-phase = 1
 */
typedef enum {
    DRIVER_SPI_MODE_0 = 0,
    DRIVER_SPI_MODE_1 = 1,
    DRIVER_SPI_MODE_2 = 2,
    DRIVER_SPI_MODE_3 = 3
} DRIVER_CFG_SPI_MODE;

/**
 * @brief Enumeration to select the data order of the SPI driver module
 * 
 */
typedef enum {
    DRIVER_SPI_DATA_ORDER_MSB = 0,
    DRIVER_SPI_DATA_ORDER_LSB = 1
} DRIVER_CFG_SPI_DATA_ORDER;

//-----------------------------------------------------------------------------

/**
 * @brief Enumeration to select the Bitrate to be used
 * in a I2C driver module
 * 
 */
typedef enum {
    DRIVER_I2C_BITRATE_1KHZ = 0,
    DRIVER_I2C_BITRATE_10KHZ = 1,
    DRIVER_I2C_BITRATE_100KHZ = 2,
    DRIVER_I2C_BITRATE_1MHZ = 3
} DRIVER_CFG_I2C_BITRATE;

//-----------------------------------------------------------------------------

/**
 * @brief On linux-system name of device file is stored into here
 * 
 */
typedef struct DRIVER_CFG_DEVICE_NAME {
    char name[DRIVER_CFG_DEVICE_NAME_MAX_LENGTH];
} DRIVER_CFG_DEVICE_NAME_TYPE;

//-----------------------------------------------------------------------------

/**
 * @brief Structure to configure an SPI driver module.
 * 
 */
typedef struct CFG_DRIVER_SPI {

    /**
     * @brief Declare this module as a SPI-MASTER (is_master=1)
     * or a SPI-SLAVE (is_master=0)
     * 
     */
    u8 is_master;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_SPI_MODE
     */
    DRIVER_CFG_SPI_MODE mode;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_CLK_DEVIDER
     */
    DRIVER_CFG_CLK_DEVIDER clk_divider;

    u8 clk_double_speed;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_SPI_DATA_ORDER
     */
    DRIVER_CFG_SPI_DATA_ORDER data_order;

    u8 interrupt_enable;
    u32 speed;
    //char device[DRIVER_CFG_DEVICE_NAME_MAX_LENGTH];

} CFG_DRIVER_SPI;

//-----------------------------------------------------------------------------

/**
 * @brief Structure to configure a I2C driver module
 * 
 */
typedef struct CFG_DRIVER_I2C {

    u8 is_master;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_I2C_BITRATE
     */
    DRIVER_CFG_I2C_BITRATE bit_rate;

    u8 enable_ack;
    u8 interrupt_enable;
    u8 answer_general_call;
    u8 slave_addr;

} CFG_DRIVER_I2C;

//-----------------------------------------------------------------------------

/**
 * @brief Strucutre to configure an USART driver module
 * 
 */
typedef struct CFG_DRIVER_USART {

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_BAUDRATE
     */
    DRIVER_CFG_BAUDRATE baudrate;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_DATABITS
     */
    DRIVER_CFG_DATABITS databits;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_STOPBITS
     */
    DRIVER_CFG_STOPBITS stopbits;

    /**
     * @see cfg_dirver_interface.h#DRIVER_CFG_PARITY
     */
    DRIVER_CFG_PARITY parity;

} CFG_DRIVER_USART;

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct TRX_DRIVER_CONFIGURATION {

    /**
     * @brief union to select the type of configuration
     * for the driver module that will be confgiured
     * Only one type of driver module can be configured at a time
     * 
     */
    union {

        /**
         * @see cfg_dirver_interface.h#CFG_DRIVER_SPI
         */
        CFG_DRIVER_SPI spi;

        /**
         * @see cfg_dirver_interface.h#CFG_DRIVER_I2C
         */
        CFG_DRIVER_I2C i2c;

        /**
         * @see cfg_dirver_interface.h#CFG_DRIVER_USART
         */
        CFG_DRIVER_USART usart;

    } module;

    #if defined __raspberrypi__ || defined __unittest__
    DRIVER_CFG_DEVICE_NAME_TYPE device;
    #endif

} TRX_DRIVER_CONFIGURATION;

//-----------------------------------------------------------------------------

#endif // _CFG_DRIVER_INTERFACE_H_

//-----------------------------------------------------------------------------
