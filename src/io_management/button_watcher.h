#ifndef _IO_BUTTON_WATCHER_H_
#define _IO_BUTTON_WATCHER_H_

#include "iterator_interface.h"
#include "mcu_task_interface.h"

/*!
 * Timespan that the button will be in pressed state, until the down state is activated
 */
#define IO_BUTTON_PRESSED_TIMESPAN_MS	30000

/*!
 * Timespan the button will be in released state afte it was down and has been released
 */
#define IO_BUTTON_RELEASED_TIMESPAN_MS	30000


/*!
 * Function-Call-refrence to the logic that will produce the actual hw-state of that particular button
 * @return 	1 if button is pressed
 * 		0 if button is not pressed
 */
typedef u8 (*IO_BUTTON_WATCHER_PIN_STATE_CALLBACK) 	(void);

/*!
 *
 */
typedef struct IO_INPUT_DESCRIPTOR_T {

	u8 id;

	u8 down;
	u8 pressed;
	u8 released;

	u32 __down_time;
	u16 __press_time;
	u16 __release_time;

	IO_BUTTON_WATCHER_PIN_STATE_CALLBACK __pin_state;

	struct IO_INPUT_DESCRIPTOR_T* __next_button;

} IO_INPUT_DESCRIPTOR;

/*!
 *
 */
typedef struct BUTTON_STATE {

	u8 id;
	u8 down;
	u8 pressed;
	u8 released;
	u32 down_time;

} IO_INPUT_STATE;

/*!
 *
 */
void io_input_controller_init(void);

/*!
 *
 * @param p_new_button
 * @return
 */
u8 io_input_controller_register_input(IO_INPUT_DESCRIPTOR* p_new_button);

/*!
 *
 * @param button_id
 * @param p_button_state
 */
void io_input_controller_get_state(u8 button_id, IO_INPUT_STATE* p_button_state);

/*!
 *
 */
void io_input_controller_task_init(void);

/*!
 *
 * @return
 */
MACU_TASK_INTERFACE_TASK_STATE io_input_controller_task_get_state(void);

/*!
 *
 */
void io_input_controller_task_run(void);

/*!
 *
 */
void io_input_controller_task_background_run(void);

/*!
 *
 */
void io_input_controller_task_sleep(void);

/*!
 *
 */
void io_input_controller_task_wakeup(void);

/*!
 *
 */
void io_input_controller_task_finish(void);

/*!
 *
 */
void io_input_controller_task_terminate(void);

/*!
 *
 * @param p_iterator
 */
void io_input_controller_get_iterator(ITERATOR_INTERFACE* p_iterator);

/*!
 *
 * @param p_iterator
 * @param p_button_state
 */
void io_input_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_INPUT_STATE* p_button_state);

/*!
 *
 * @param p_iterator
 */
void io_input_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator);


#endif // _IO_BUTTON_WATCHER_H_
