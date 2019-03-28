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
void tracer_pass(const char* file_name, u16 line_id);

/*!
 *
 * @param file_name
 * @param line_id
 * @param byte
 */
void tracer_trace_byte(const char* file_name, u16 line_id, u8 byte);

/*!
 *
 * @param file_name
 * @param line_id
 * @param word
 */
void tracer_trace_word(const char* file_name, u16 line_id, u16 word);

/*!
 *
 * @param file_name
 * @param line_id
 * @param integer
 */
void tracer_trace_long(const char* file_name, u16 line_id, u32 integer);

/*!
 *
 * @param file_name
 * @param line_id
 * @param length
 * @param p_buffer
 */
void tracer_trace_n(const char* file_name, u16 line_id, u8 length, u8* p_buffer);

#endif // _TRACER_H_

#ifdef TRACER_ENABLED
#define TRACER_RESTART()	tracer_init();
#else
#define TRACER_RESTART()	do {} while(0)
#endif

#ifdef PASS
#undef PASS()
#endif

#ifdef PASS
#undef TRACE_byte(byte)
#endif

#ifdef PASS
#undef TRACE_word(word)
#endif

#ifdef PASS
#undef TRACE_long(integer)
#endif

#ifdef PASS
#undef TRACE_N(len, p_buf)
#endif


#if defined TRACER_ON && defined TRACER_ENABLED

#define PASS()				tracer_pass(__FILE__, __LINE__)
#define TRACE_byte(byte)		tracer_trace_byte(__FILE__, __LINE__, byte)
#define TRACE_word(word)		tracer_trace_word(__FILE__, __LINE__, word)
#define TRACE_long(integer)		tracer_trace_long(__FILE__, __LINE__, integer)
#define TRACE_N(len, p_buf)		tracer_trace_n(__FILE__, __LINE__, len, p_buf)

#else

#define TRACES_RESTART()		do {} while(0)
#define PASS()				do {} while(0)
#define TRACE_byte(byte)		do {} while(0)
#define TRACE_word(word)		do {} while(0)
#define TRACE_long(integer)		do {} while(0)
#define TRACE_N(len, buf)		do {} while(0)

#define BUILD_TRACER(name)

#endif // TRACER_ON

#undef TRACER_ON
