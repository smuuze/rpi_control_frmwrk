#ifndef _LOCAL_CONTEXT_H_
#define _LOCAL_CONTEXT_H_

#include "config.h"

typedef struct T_SYSTEM_INPUT {
	u8 onboard_button_01;
	u8 onboard_button_02;
	u8 extern_input_01;
	u8 extern_input_02;
	u8 extern_input_03;
	u8 extern_input_04;
} T_SYSTEM_INPUT;

/*!
 *
 */
typedef struct T_SYSTEM_OUTPUT {
	u8 extern_output_01;
	u8 extern_output_02;
	u8 extern_output_03;
	u8 extern_output_04;
	u8 system_event_output_01;
	u8 system_busy_output_01;
} T_SYSTEM_OUTPUT;


/*!
 *
 */
typedef struct T_SYSTEM_IO {
	T_SYSTEM_INPUT in;
	T_SYSTEM_OUTPUT out;
} T_SYSTEM_IO;

/*!
 *
 */
typedef struct T_SYSTEM_SIGNAL {
	u8 led_01;
	u8 led_02;
	u8 led_03;
	u8 led_04;
	u8 led_05;
	u8 led_06;
	u8 led_07;
	u8 led_08;
	u8 led_09;
} T_SYSTEM_SIGNAL;


/*!
 *
 */
typedef struct {
	u16 channel_0;
	u16 channel_1;
	u16 channel_2;
	u16 channel_3;
	u16 temperature;
	u16 humidity;
} T_SYSTEM_DATA_ADC;

/*!
 *
 */
typedef struct {
	i8 maximal;
	i8 minimal;
	i8 actual;
} T_SYSTEM_DATA_TEMPERATURE;

typedef struct {
	u8 maximal;
	u8 minimal;
	u8 actual;
} T_SYSTEM_DATA_HUMIDTY;

/*!
 *
 */
typedef struct {
	T_SYSTEM_DATA_ADC		adc;
	T_SYSTEM_DATA_TEMPERATURE	temperature;
	T_SYSTEM_DATA_HUMIDTY		humidity;
} T_SYSTEM_DATA;

/*!
 *
 */
typedef struct LOCAL_CONTEXT {
	T_SYSTEM_IO io;
	T_SYSTEM_SIGNAL signal;
	T_SYSTEM_DATA	data;
} SYSTEM_T;


extern SYSTEM_T system_context;

#define GET_SYSTEM(part)	system_context.part
#define GET_SYSTEM_PTR()	&system_context


#define SYS_INPUT		io.in
#define SYS_OUTPUT		io.out
#define SYS_SIGNAL		signal

#endif // _LOCAL_CONTEXT_H_
