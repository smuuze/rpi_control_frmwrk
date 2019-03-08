#ifndef _TEST_TRACER_MCU_TASK_H_
#define _TEST_TRACER_MCU_TASK_H_

#include "trx_driver_interface.h"
#include "mcu_task_interface.h"

/*!
 *
 * @param p_driver
 */
void test_tracer_module_init (TRX_DRIVER_INTERFACE* p_driver);

/*!
 *
 */
void test_tracer_task_init(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE test_tracer_task_get_state(void);

/*!
 *
 */
void test_tracer_task_run(void);

/*!
 *
 */
void test_tracer_task_background_run(void);

/*!
 *
 */
void test_tracer_task_sleep(void);

/*!
 *
 */
void test_tracer_task_wakeup(void);

/*!
 *
 */
void test_tracer_task_finish(void);

/*!
 *
 */
void test_tracer_task_terminate(void);


#endif // _TEST_TRACER_MCU_TASK_H_
