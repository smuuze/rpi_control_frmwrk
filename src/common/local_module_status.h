#ifndef _LOCAL_MODULE_STATUS_H_
#define _LOCAL_MODULE_STATUS_H_

#include "config.h"
#include <string.h>

//#include <atomic.h>

/*!
 *
 */
typedef struct MODULE_STATUS_U8 {
	u8 data;
} MODULE_STATUS_U8_TYPE;

/*!
 *
 */
typedef struct MODULE_STATUS_U16 {
	u16 data;
} MODULE_STATUS_U16_TYPE;

/*!
 *
 */
typedef struct MODULE_STATUS_U32 {
	u32 data;
} MODULE_STATUS_U32_TYPE;

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_set_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 * @return
 */
u8 module_status_isset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_unset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/*!
 *
 * @param p_status
 */
void module_status_clear_all_u8(MODULE_STATUS_U8_TYPE* p_status);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_set_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 * @return
 */
u16 module_status_isset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_unset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/*!
 *
 * @param p_status
 */
void module_status_clear_all_u16(MODULE_STATUS_U16_TYPE* p_status);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_set_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 * @return
 */
u32 module_status_isset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
void module_status_unset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/*!
 *
 * @param p_status
 */
void module_status_clear_all_u32(MODULE_STATUS_U32_TYPE* p_status);


#define BUILD_MODULE_STATUS_U8(name)													\
																	\
	static MODULE_STATUS_U8_TYPE __##name##_module_status;										\
																	\
	inline static void name##_set(u8 bit_flag) 	{ module_status_set_u8(&(__##name##_module_status), bit_flag); }		\
	inline static u8 name##_is_set(u8 bit_flag) 	{ return module_status_isset_u8(&(__##name##_module_status), bit_flag); }	\
	inline static void name##_unset(u8 bit_flag) 	{ module_status_unset_u8(&(__##name##_module_status), bit_flag); }		\
	inline static void name##_clear_all(void) 	{ module_status_clear_all_u8(&(__##name##_module_status)); }

#define BUILD_MODULE_STATUS_U16(name)													\
																	\
	static MODULE_STATUS_U16_TYPE __##name##_module_status;										\
																	\
	inline static void name##_set(u16 bit_flag) 	{ module_status_set_u16(&(__##name##_module_status), bit_flag); }		\
	inline static u16 name##_is_set(u16 bit_flag) 	{ return module_status_isset_u16(&(__##name##_module_status), bit_flag); }	\
	inline static void name##_unset(u16 bit_flag) 	{ module_status_unset_u16(&(__##name##_module_status), bit_flag); }		\
	inline static void name##_clear_all(void) 	{ module_status_clear_all_u16(&(__##name##_module_status)); }

#define BUILD_MODULE_STATUS_U32(name)													\
																	\
	static MODULE_STATUS_U32_TYPE __##name##_module_status;										\
																	\
	inline static void name##_set(u32 bit_flag) 	{ module_status_set_u32(&(__##name##_module_status), bit_flag); }		\
	inline static u32 name##_is_set(u32 bit_flag) 	{ return module_status_isset_u32(&(__##name##_module_status), bit_flag); }	\
	inline static void name##_unset(u32 bit_flag) 	{ module_status_unset_u32(&(__##name##_module_status), bit_flag); }		\
	inline static void name##_clear_all(void) 	{ module_status_clear_all_u32(&(__##name##_module_status)); }


#define BUILD_MODULE_STATUS_FAST(name, size)												\
																	\
	static u8 __##name##_module_status[size];											\
																	\
	inline static void name##_set(u8 index)		{ __##name##_module_status[index] = 1; }						\
	inline static u8 name##_is_set(u8 index)	{ return __##name##_module_status[index] != 0 ? 1 : 0; }				\
	inline static void name##_unset(u8 index)	{ __##name##_module_status[index] = 0; }						\
	inline static void name##_clear_all(void)	{ memset(__##name##_module_status, 0x00, size); }					\


#define BUILD_MODULE_STATUS_FAST_VOLATILE(name, size)											\
																	\
	static volatile u8 __##name##_module_status[size];										\
																	\
	inline static void name##_set(u8 index)		{ __##name##_module_status[index] = 1; }					\
	inline static u8 name##_is_set(u8 index)	{ return __##name##_module_status[index] != 0 ? 1 : 0; }			\
	inline static void name##_unset(u8 index)	{ __##name##_module_status[index] = 0; }					\
	inline static void name##_clear_all(void)	{ memset((u8*)&__##name##_module_status[0], 0x00, size); }				\


#endif // _LOCAL_MODULE_STATUS_H_
