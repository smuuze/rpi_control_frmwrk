/*! \file *********************************************************************

 *****************************************************************************/

#ifndef RPI_PROTOCOL_HANDLER_H_
#define RPI_PROTOCOL_HANDLER_H_

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "trx_driver_interface.h"
#include "cfg_driver_interface.h"
#include "mcu_task_interface.h"

#ifndef RPI_PROTOCOL_HANDLER_DRIVER_CFG
#define RPI_PROTOCOL_HANDLER_DRIVER_CFG			DRIVER_SPI_SLAVE, 		/*	u8 op_mode; 		*/ \
							DRIVER_SPI_MODE_3, 		/*	u8 clk_phase;		*/ \
							DRIVER_SPI_CLK_DEVIDER_4, 	/*	u8 clk_divider;		*/ \
							DRIVER_SPI_NO_DOUBLE_SPEED, 	/*	u8 clk_double_speed;	*/ \
							DRIVER_SPI_DATA_ORDER_MSB, 	/*	u8 data_order;		*/ \
							DRIVER_SPI_INTERRUPT_DISABLE	/*	u8 interrupt_enable;	*/
#endif

/*!
 *
 */
#ifndef RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS
#define RPI_PROTOCOL_COMMAND_READ_TIMEOUT_MS		1000
#endif

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

/*!
 *
 */
void rpi_protocol_task_init(void);

/*!
 *
 * @return
 */
MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void);

/*!
 *
 */
void rpi_protocol_task_run(void);

/*!
 *
 */
void rpi_protocol_handler_debus_handler(void);

#endif //RPI_PROTOCOL_HANDLER_H_
