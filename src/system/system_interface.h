#ifndef _LOCAL_SYSTEM_INTERFACE_H_
#define _LOCAL_SYSTEM_INTERFACE_H_

/*
 */
typedef enum {

	SYS_EVT_NO_EVENT = 0x00,
	SYS_EVT_INPUT_CHANGED = 0x01

} SYSTEM_EVENT;

// --------------------------------------------------------------------------------------------------------------------

/*!
 *
 * @return
 */
typedef u8  (*SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK)		(void);
typedef u16 (*SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK)		(void);
typedef u32 (*SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK)		(void);

/*!
 *
 */
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK)		(u8 ref_time, u8 time_interval);
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_u16_CALLBACK)		(u16 ref_time, u16 time_interval);
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK)		(u32 ref_time, u32 time_interval);

/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK 		now_u8;
	SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK 		now_u16;
	SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK 		now_u32;

	SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK		isup_u8;
	SYSTEM_INTERFACE_IS_TIME_UP_u16_CALLBACK 	isup_u16;
	SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK 	isup_u32;
} SYSTEM_INTERFACE_TIME;

// --------------------------------------------------------------------------------------------------------------------

/*!
 *
 */
typedef void (*SYSTEM_INTERFACE_ADD_EVENT_CALLBACK)		(SYSTEM_EVENT evt);
typedef SYSTEM_EVENT (*SYSTEM_INTERFACE_GET_EVENT_CALLBACK)	(void);

/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_ADD_EVENT_CALLBACK 	add;
	SYSTEM_INTERFACE_GET_EVENT_CALLBACK 	get_next;
} SYSTEM_INTERFACE_EVENT;

// --------------------------------------------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	GPIO_LEVEL_LOW = 0x00,  //!< PIN_LEVEL_LOW
	GPIO_LEVEL_HIGH = 0x01, //!< PIN_LEVEL_HIGH
	GPIO_LEVEL_HIGH_Z = 0x2,//!< PIN_LEVEL_HIGH_Z
} SYSTEM_INTERFACE_GPIO_LEVEL;

typedefe enum {
	GPIO_INPUT = 0x00,
	GPIO_OUTPUT = 0x01,
} SYSTEM_INTERFACE_GPIO_DIRECTION;

/*!
 *
 */
typedef void (*SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK)							(u8 pin_num, SYSTEM_INTERFACE_GPIO_DIRECTION direction);
typedef void (*SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK)							(u8 pin_num, SYSTEM_INTERFACE_IO_PIN_LEVEL level);
typedef SYSTEM_INTERFACE_IO_PIN_LEVEL (*SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK)	(u8 pin_num);

/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK 	set_direction;
	SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK 	set_level;
	SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK 	get_level;
} SYSTEM_INTERFACE_IO;

// --------------------------------------------------------------------------------------------------------------------

typedef u8   (*SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK)	(u8* addr);
typedef u16  (*SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK)	(u8* addr);
typedef u32  (*SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK)	(u8* addr);
typedef void (*SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK)	(u8* addr, u8* p_buffer_to, u8 num_bytes);


/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK	get_byte;
	SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK	get_word;
	SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK	get_long;
	SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK	get_N_bytes;
} SYSTEM_INTERFACE_MEMORY_PROGMEM;

/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_MEMORY_PROGMEM	progmem;
} SYSTEM_INTERFACE_MEMORY;

// --------------------------------------------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {
	SYSTEM_INTERFACE_TIME 	time;
	SYSTEM_INTERFACE_EVENT	event;
	SYSTEM_INTERFACE_MEMORY	memory;
	SYSTEM_INTERFACE_IO		io;
} SYSTEM_INTERFACE;

/*!
 *
 */
extern const SYSTEM_INTERFACE i_system;

#endif // _LOCAL_SYSTEM_INTERFACE_H_
