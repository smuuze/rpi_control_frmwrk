#ifndef _CFG_DRIVER_INTERFACE_H_
#define _CFG_DRIVER_INTERFACE_H_

#define DRIVER_SPI_MASTER					1
#define DRIVER_SPI_SLAVE					0

#define DRIVER_SPI_MODE_0					0
#define DRIVER_SPI_MODE_1					1
#define DRIVER_SPI_MODE_2					2
#define DRIVER_SPI_MODE_3					3

#define DRIVER_SPI_CLK_DEVIDER_4				0
#define DRIVER_SPI_CLK_DEVIDER_8				1
#define DRIVER_SPI_CLK_DEVIDER_16				2
#define DRIVER_SPI_CLK_DEVIDER_32				3
#define DRIVER_SPI_CLK_DEVIDER_64				4
#define DRIVER_SPI_CLK_DEVIDER_128				5

#define DRIVER_SPI_DOUBLE_SPEED					1
#define DRIVER_SPI_NO_DOUBLE_SPEED				0

#define DRIVER_SPI_DATA_ORDER_MSB				0
#define DRIVER_SPI_DATA_ORDER_LSB				1

#define DRIVER_SPI_INTERRUPT_ENABLE				1
#define DRIVER_SPI_INTERRUPT_DISABLE				0

#define DRIVER_I2C_BITRATE_1KHZ					0
#define DRIVER_I2C_BITRATE_10KHZ				1
#define DRIVER_I2C_BITRATE_100KHZ				2
#define DRIVER_I2C_BITRATE_1MHZ					3

/*!
 *
 */
typedef void (*CFG_DRIVER_INTERFACE_HOST_RX_COMPLETE_CALLBACK)	(void);

/*!
 *
 */
typedef void (*CFG_DRIVER_INTERFACE_HOST_TX_COMPLETE_CALLBACK)	(void);

/*!
 *
 */
typedef struct CFG_DRIVER_SPI {

	u8 is_master;
	u8 mode;
	u8 clk_divider;
	u8 clk_double_speed;
	u8 data_order;
	u8 interrupt_enable;

} CFG_DRIVER_SPI;


/*!
 *
 */
typedef struct CFG_DRIVER_I2C {

	u8 is_master;
	u8 bit_rate;
	u8 enable_ack;
	u8 interrupt_enable;
	u8 answer_general_call;
	u8 slave_addr;

} CFG_DRIVER_I2C;


/*!
 *
 */
typedef struct CFG_DRIVER_USART {

	u8 baud_rate;
	u8 databits;
	u8 stopbits;

} CFG_DRIVER_USART;


/*!
 *
 */
typedef struct TRX_DRIVER_CONFIGURATION {

	union {

		CFG_DRIVER_SPI spi;
		CFG_DRIVER_I2C i2c;
		CFG_DRIVER_USART usart;

	} module;

} TRX_DRIVER_CONFIGURATION;

#endif // _CFG_DRIVER_INTERFACE_H_
