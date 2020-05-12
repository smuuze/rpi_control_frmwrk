#ifndef _TRACER_H_
#define _TRACER_H_

#include "config.h"

/*!
 *
 * @param p_driver
 * @param p_driver_cfg
 */
void tracer_init(void);

/*!
 *
 * @param file_name
 * @param line_id
 */
void tracer_pass(const char* str, const char* file_name, u16 line_id);

/*!
 *
 * @param file_name
 * @param line_id
 * @param byte
 */
void tracer_trace_byte(const char* str, const char* file_name, u16 line_id, u8 byte);

/*!
 *
 * @param file_name
 * @param line_id
 * @param word
 */
void tracer_trace_word(const char* str, const char* file_name, u16 line_id, u16 word);

/*!
 *
 * @param file_name
 * @param line_id
 * @param integer
 */
void tracer_trace_long(const char* str, const char* file_name, u16 line_id, u32 integer);

/*!
 *
 * @param file_name
 * @param line_id
 * @param length
 * @param p_buffer
 */
void tracer_trace_n(const char* str, const char* file_name, u16 line_id, u8 length, u8* p_buffer);


#define ALWAYS_PASS(str)		tracer_pass(str, __FILE__, __LINE__)
#define ALWAYS_TRACE_byte(byte, str)	tracer_trace_byte(str, __FILE__, __LINE__, byte)
#define ALWAYS_TRACE_word(word, str)	tracer_trace_word(str, __FILE__, __LINE__, word)
#define ALWAYS_TRACE_long(integer, str)	tracer_trace_long(str, __FILE__, __LINE__, integer)
#define ALWAYS_TRACE_N(len, p_buf, str)	tracer_trace_n(str, __FILE__, __LINE__, len, p_buf)


#endif // _TRACER_H_

#ifdef TRACER_ENABLED
#define TRACER_RESTART()	tracer_init();
#else
#define TRACER_RESTART()	do {} while(0)
#endif

#ifdef PASS
#undef PASS()
#endif

#ifdef TRACE_byte
#undef TRACE_byte(byte)
#endif

#ifdef TRACE_word
#undef TRACE_word(word)
#endif

#ifdef TRACE_long
#undef TRACE_long(integer)
#endif

#ifdef TRACE_N
#undef TRACE_N(len, p_buf)
#endif


#ifdef DEBUG_PASS
#undef DEBUG_PASS(str)
#endif

#ifdef DEBUG_TRACE_byte
#undef DEBUG_TRACE_byte(byte, str)
#endif

#ifdef DEBUG_TRACE_word
#undef DEBUG_TRACE_word(word, str)
#endif

#ifdef DEBUG_TRACE_long
#undef DEBUG_TRACE_long(integer, str)
#endif

#ifdef DEBUG_TRACE_N
#undef DEBUG_TRACE_N(len, p_buf, str)
#endif



#if defined TRACER_ON && defined TRACER_ENABLED

//#pragma TRACES_ENABLED

#define PASS()				tracer_pass("0", __FILE__, __LINE__)
#define TRACE_byte(byte)		tracer_trace_byte("0", __FILE__, __LINE__, byte)
#define TRACE_word(word)		tracer_trace_word("0", __FILE__, __LINE__, word)
#define TRACE_long(integer)		tracer_trace_long("0", __FILE__, __LINE__, integer)
#define TRACE_N(len, p_buf)		tracer_trace_n("0", __FILE__, __LINE__, len, p_buf)

#define DEBUG_PASS(str)			tracer_pass(str, __FILE__, __LINE__)
#define DEBUG_TRACE_byte(byte, str)	tracer_trace_byte(str, __FILE__, __LINE__, byte)
#define DEBUG_TRACE_word(word, str)	tracer_trace_word(str, __FILE__, __LINE__, word)
#define DEBUG_TRACE_long(integer, str)	tracer_trace_long(str, __FILE__, __LINE__, integer)
#define DEBUG_TRACE_N(len, p_buf, str)	tracer_trace_n(str, __FILE__, __LINE__, len, (u8*)p_buf)

#else

//#pragma TRACES_NOT_ENABLED

#define TRACES_RESTART()		do {} while(0)

#define PASS()				do {} while(0)
#define TRACE_byte(byte)		do {} while(0)
#define TRACE_word(word)		do {} while(0)
#define TRACE_long(integer)		do {} while(0)
#define TRACE_N(len, buf)		do {} while(0)

#define DEBUG_PASS(str)			do {} while(0)
#define DEBUG_TRACE_byte(byte, str)	do {} while(0)
#define DEBUG_TRACE_word(word, str)	do {} while(0)
#define DEBUG_TRACE_long(integer, str)	do {} while(0)
#define DEBUG_TRACE_N(len, bu, strf)	do {} while(0)

#define BUILD_TRACER(name)

#endif // #if defined TRACER_ON && defined TRACER_ENABLED

#undef TRACER_ON
