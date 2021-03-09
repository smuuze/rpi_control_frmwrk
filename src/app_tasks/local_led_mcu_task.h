/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	local_led_mcu_task.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _LOCAL_LED_MCU_TASK_H_
#define _LOCAL_LED_MCU_TASK_H_

// --------------------------------------------------------------------------------

/*!
 *
 */
void local_led_mcu_task_init(void);

/*!
 *
 */
u8 local_led_mcu_task_is_runable(void);

/*!
 *
 */
void local_led_mcu_task_run(void);

/*!
 *
 */
void local_led_mcu_task_background_run(void);

/*!
 *
 */
void local_led_mcu_task_sleep(void);

/*!
 *
 */
void local_led_mcu_task_wakeup(void);

/*!
 *
 */
void local_led_mcu_task_finish(void);

/*!
 *
 */
void local_led_mcu_task_terminate(void);


#endif // _LOCAL_LED_MCU_TASK_H_
