#ifndef _LOCAL_ADS1115_MCU_TASK_H_
#define _LOCAL_ADS1115_MCU_TASK_H_

#include "trx_driver_interface.h"
#include "mcu_task_interface.h"

/*!
 *
 * @param p_driver
 */
void ads1115_driver_init (TRX_DRIVER_INTERFACE* p_driver);

/*!
 *
 */
void local_ads1115_mcu_task_init(void);

/*!
 *
 */
u16 local_ads1115_mcu_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE local_ads1115_mcu_task_get_state(void);

/*!
 *
 */
void local_ads1115_mcu_task_run(void);

/*!
 *
 */
void local_ads1115_mcu_task_background_run(void);

/*!
 *
 */
void local_ads1115_mcu_task_sleep(void);

/*!
 *
 */
void local_ads1115_mcu_task_wakeup(void);

/*!
 *
 */
void local_ads1115_mcu_task_finish(void);

/*!
 *
 */
void local_ads1115_mcu_task_terminate(void);


#endif // _LOCAL_ADS1115_MCU_TASK_H_
