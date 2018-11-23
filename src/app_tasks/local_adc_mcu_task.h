#ifndef _LOCAL_ADC_MCU_TASK_H_
#define _LOCAL_ADC_MCU_TASK_H_

#include "trx_driver_interface.h"

/*!
 *
 * @param p_driver
 */
void local_adc_module_init (TRX_DRIVER_INTERFACE* p_driver);

/*!
 *
 */
void local_adc_mcu_task_init(void);

/*!
 *
 */
u8 local_adc_mcu_task_is_runable(void);

/*!
 *
 */
void local_adc_mcu_task_run(void);

/*!
 *
 */
void local_adc_mcu_task_background_run(void);

/*!
 *
 */
void local_adc_mcu_task_sleep(void);

/*!
 *
 */
void local_adc_mcu_task_wakeup(void);

/*!
 *
 */
void local_adc_mcu_task_finish(void);

/*!
 *
 */
void local_adc_mcu_task_terminate(void);


#endif // _LOCAL_ADC_MCU_TASK_H_
