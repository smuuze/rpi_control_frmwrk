#ifndef _IO_OUTPUT_CONTROLLER_H_
#define _IO_OUTPUT_CONTROLLER_H_

#include "iterator_interface.h"
#include "mcu_task_interface.h"

/*!
 *
 * @param port_num
 */
typedef void (*IO_OUTPUT_SET_PIN) 	(u8 is_on);

#define IO_OUTPUT_STATE_DISABLED	0xFF
#define IO_OUTPUT_STATE_ON			0x01
#define IO_OUTPUT_STATE_OFF			0x00

#define IO_OUTPUT_ID_INVALID		0xFF

typedef enum {
	IO_TYPE_UNDEFINED,
	IO_TYPE_SYSTEM,
	IO_TYPE_USER
} IO_TYPE;

/*!
 *
 */
typedef struct IO_OUTPUT_DESCRIPTOR {

	u8 id;
	u8 pin_id;

	IO_TYPE type;

	u8 actual_pin_state;
	u8 next_pin_state;

	u32 reference_time;
	u32 duration;
	u32 toggle_period;

	IO_OUTPUT_SET_PIN set_pin;

	struct IO_OUTPUT_DESCRIPTOR* _next;

} IO_OUTPUT_DESCRIPTOR;

/*!
 *
 */
typedef struct IO_OUTPUT_STATE {

	u8 id;
	u8 pin_state;
	u32 toggle_period;
	u32 duration;
	u32 on_time;

} IO_OUTPUT_STATE;

/*!
 *
 */
void io_output_controller_init(void);

/*!
 *
 * @param p_new_output
 * @return
 */
u8 io_output_controller_register_output(IO_OUTPUT_DESCRIPTOR* p_new_output);

/*!
 *
 * @param id
 * @param state
 * @param duration
 * @param toggle_peridod
 * @return
 */
u8 io_output_controller_set_output(u8 id, u8 state, u32 duration, u32 toggle_peridod);

/*!
 *
 * @param id
 * @param p_state
 */
void io_output_controller_get_output(u8 id, IO_OUTPUT_STATE* p_state);

/*!
 *
 */
void io_output_controller_task_init(void);

/*!
 *
 * @return
 */
MCU_TASK_INTERFACE_TASK_STATE io_output_controller_task_get_state(void);

/*!
 *
 */
void io_output_controller_task_run(void);


/*!
 *
 */
void io_output_controller_task_background_run(void);


/*!
 *
 * @param p_iterator
 */
void io_output_controller_get_iterator(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter);

/*!
 *
 * @param p_iterator
 * @param p_output_state
 */
void io_output_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_OUTPUT_STATE* p_output_state);

/*!
 *
 * @param p_iterator
 */
void io_output_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter);

#endif // _IO_OUTPUT_CONTROLLER_H_
