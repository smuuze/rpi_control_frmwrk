 /*
  * \@file	protocol_management/rpi_protocl_handler.h
  * \author	sebastian lesse
  */

#ifndef RPI_PROTOCOL_HANDLER_H_
#define RPI_PROTOCOL_HANDLER_H_

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "mcu_task_management/mcu_task_interface.h"

//-----------------------------------------------------------------------------

#define RPI_CONTROL_PREFIX_CFG_STRING			"COM"
#define RPI_PROTOCOL_SPEED_CFG_STRING			"COM_SPEED_HZ"
#define RPI_PROTOCOL_DEVICE_CFG_STRING			"COM_DEVICE"

//-----------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_DEFAULT_SPI_SPEED_HZ
#define RPI_PROTOCOL_HANDLER_DEFAULT_SPI_SPEED_HZ
#endif

//-----------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_DRIVER_CFG
#define RPI_PROTOCOL_HANDLER_DRIVER_CFG			.is_master = COM_DRIVER_IS_SLAVE, 			/*	u8 op_mode; 		*/ \
							.mode = DRIVER_SPI_MODE_3, 				/*	u8 clk_phase;		*/ \
							.clk_divider = DRIVER_SPI_CLK_DEVIDER_4, 		/*	u8 clk_divider;		*/ \
							.clk_double_speed = DRIVER_SPI_NO_DOUBLE_SPEED, 	/*	u8 clk_double_speed;	*/ \
							.data_order = DRIVER_SPI_DATA_ORDER_MSB, 		/*	u8 data_order;		*/ \
							.interrupt_enable = COM_DRIVER_IRQ_ENABLE,		/*	u8 interrupt_enable;	*/ \
							.speed = DRIVER_SPI_SPEED_HZ_19200			/*	u32 speed;		*/
#endif

//-----------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_I2C_HANDLER_DRIVER_CFG
#define RPI_PROTOCOL_I2C_HANDLER_DRIVER_CFG		COM_DRIVER_IS_SLAVE, 		/*	u8 op_mode; 		*/ \
							DRIVER_SPI_MODE_3, 		/*	u8 clk_phase;		*/ \
							DRIVER_SPI_CLK_DEVIDER_4, 	/*	u8 clk_divider;		*/ \
							DRIVER_SPI_NO_DOUBLE_SPEED, 	/*	u8 clk_double_speed;	*/ \
							DRIVER_SPI_DATA_ORDER_MSB, 	/*	u8 data_order;		*/ \
							COM_DRIVER_IRQ_DISABLE		/*	u8 interrupt_enable;	*/
#endif

//-----------------------------------------------------------------------------

/*!
 *
 */
#ifndef RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS
#define RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS		1000
#endif

//-----------------------------------------------------------------------------

/*!
 *
 * @param p_driver
 */
void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver);

/*!
 *
 */
void rpi_protocol_com_driver_rx_complete_callback(void);

/*!
 *
 */
void rpi_protocol_com_driver_tx_complete_callback(void);

//-----------------------------------------------------------------------------

#endif //RPI_PROTOCOL_HANDLER_H_
