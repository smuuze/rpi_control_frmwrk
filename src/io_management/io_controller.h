#ifndef _IO_CONTROLLER_H_
#define _IO_CONTROLLER_H_

#include "io_management/io_input_controller.h"
#include "io_management/io_output_controller.h"

#include "system/system_interface.h"
#include "driver/gpio/gpio_interface.h"


#define IO_CONTROLLER_BUILD_OUTPUT(name, pin_descriptor)								\
															\
	static u8 _##name##_output_id = IO_OUTPUT_ID_INVALID;								\
															\
	static IO_OUTPUT_DESCRIPTOR _##name##_output_pin = {								\
		0, /* u8 id; */												\
		&pin_descriptor, /* GPIO_DRIVER_PIN_DESCRIPTOR pin_descriptor; */					\
		IO_TYPE_USER, /* IO_TYPE type; */									\
		0,/* u8 actual_pin_state; */										\
		0,/* u8 next_pin_state; */										\
		0,/* u32 reference_time; */										\
		0,/* u32 duration; */											\
		0,/* u32 toggle_period; */										\
		0,/* IO_OUTPUT_SET_PIN set_pin; */									\
		0/* struct IO_OUTPUT_DESCRIPTOR* _next; */								\
	}; 														\
 															\
	void name##_init(void) {											\
		_##name##_output_id = io_output_controller_register_output(&_##name##_output_pin);			\
	}														\
															\
	u8 name##_is_on(void) {												\
		return _##name##_output_pin.actual_pin_state != IO_OUTPUT_STATE_OFF ? 1 : 0;				\
	}														\
															\
	void name##_set_on(void) {											\
		io_output_controller_set_output(_##name##_output_id, IO_OUTPUT_STATE_ON, 0, 0);				\
	}														\
															\
	void name##_set_off(void) {											\
		io_output_controller_set_output(_##name##_output_id, IO_OUTPUT_STATE_OFF, 0, 0);			\
	}														\
															\
	void name##_set_disabled(void) {										\
		io_output_controller_set_output(_##name##_output_id, IO_OUTPUT_STATE_OFF, 0, 0);			\
	}														\
															\
 	void name##_set_control(u8 state, u32 duration, u32 toggle_peridod) {						\
		io_output_controller_set_output(_##name##_output_id, state, duration, toggle_peridod);			\
	}														\
															\
	u32 name##_get_on_time(void) {											\
		if (_##name##_output_pin.actual_pin_state == IO_OUTPUT_STATE_ON) {					\
			return i_system.time.now_u32() - _##name##_output_pin.reference_time;				\
		} else {												\
			return 0;											\
		}													\
	}

#define IO_CONTROLLER_INCLUDE_OUTPUT(name)										\
	u8 name##_is_on(void);												\
	void name##_set_on(void;											\
	void name##_set_off(void);											\
	void name##_set_disabled(void);											\
 	ivoid name##_set_control(u8 state, u32 duration, u32 toggle_peridod);						\
	u32 name##_get_on_time(void);

#define IO_CONTROLLER_BUILD_INPUT(name, pin_descriptor)									\
															\
	static u8 _##name##_input_id = 0;										\
															\
	static IO_INPUT_DESCRIPTOR _##name##input_pin = {								\
		0, /* u8 id; */												\
		&pin_descriptor, /* GPIO_DRIVER_PIN_DESCRIPTOR pin_descriptor; */					\
		GPIO_LEVEL_HIGH_Z, /* SYSTEM_INTERFACE_GPIO_LEVEL pin_state */ 						\
		0, /* u8 down; */											\
		0, /* u8 pressed; */											\
		0, /* u8 released; */											\
		0, /* u32 __down_time; */										\
		0, /* u16 __press_time; */										\
		0, /* u16 __release_time; */										\
		0 /* struct IO_INPUT_DESCRIPTOR_T* __next_button; */							\
	};														\
															\
	void name##_init(void) {											\
		_##name##_input_id  = io_input_controller_register_input(&_##name##input_pin);				\
	}														\
															\
	u8 name##_get_state(void) {											\
		return _##name##input_pin.actual_pin_state;								\
	}														\
															\
	u8 name##_is_pressed(void) {											\
		return _##name##input_pin.pressed;									\
	}														\
															\
	u8 name##_is_down(void) {											\
		return _##name##input_pin.down;										\
	}														\
															\
	u8 name##_is_released(void) {											\
		return _##name##input_pin.released;									\
	}														\
															\
	u32 name##_get_down_time(void) {										\
		return _##name##input_pin.__down_time;									\
	}

#define IO_CONTROLLER_INCLUDE_INPUT(name)										\
	void name##_init(void);												\
	u8 name##_get_state(void);											\
	u8 name##_is_pressed(void);											\
	u8 name##_is_down(void);											\
	u8 name##_is_released(void);											\
	u32 name##_get_down_time(void);

#endif // _IO_CONTROLLER_H_
