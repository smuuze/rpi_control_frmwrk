#ifndef _CFG_DRIVER_INTERFACE_H_
#define _CFG_DRIVER_INTERFACE_H_

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

	u8 op_mode;
	u8 clk_polarity;
	u8 clk_phase;
	u8 clk_divider;
	u8 clk_double_speed;
	u8 data_order;
	u8 interrupt_enable;

} CFG_DRIVER_SPI;


/*!
 *
 */
typedef struct CFG_DRIVER_I2C {

	u8 op_mode;

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
