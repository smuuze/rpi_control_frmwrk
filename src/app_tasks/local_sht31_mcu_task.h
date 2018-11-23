#ifndef _LOCAL_SHT31_MCU_TASK_H_
#define _LOCAL_SHT31_MCU_TASK_H_

#include "trx_driver_interface.h"

/*!
 *
 * @param p_driver
 */
void local_sht31_module_init (TRX_DRIVER_INTERFACE* p_driver);

/*!
 *
 */
void local_sht31_mcu_task_init(void);

/*!
 *
 */
MACU_TASK_INTERFACE_TASK_STATE local_sht31_mcu_task_get_state(void);

/*!
 *
 */
void local_sht31_mcu_task_run(void);

/*!
 *
 */
void local_sht31_mcu_task_background_run(void);

/*!
 *
 */
void local_sht31_mcu_task_sleep(void);

/*!
 *
 */
void local_sht31_mcu_task_wakeup(void);

/*!
 *
 */
void local_sht31_mcu_task_finish(void);

/*!
 *
 */
void local_sht31_mcu_task_terminate(void);


#endif // _LOCAL_SHT31_MCU_TASK_H_
