#ifndef _LOCAL_MSG_BUFFER_H_
#define _LOCAL_MSG_BUFFER_H_

#include "config.h"

/*!
 *
 */
typedef struct LOCAL_MSG_BUFFER_DESCR {

	volatile u8* p_buffer;
	volatile u16 read_pointer;
	volatile u16 write_pointer;
	volatile u16 num_elements;
	u16 size;
	volatile u8 status_flags;

} LOCAL_MSG_BUFFER_DESCR;


#define MSG_BUFFER_STATUS_FLAG_READ_ACTIVE				(1 << 0)
#define MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE				(1 << 1)

/*!
 *
 */
#define INCLUDE_LOCAL_MSG_BUFFER(name)					\
									\
	void name##_init(void);						\
	u16 name##_bytes_available(void);				\
	u16 name##_bytes_free(void); 					\
	void name##_clear_all(void);					\
									\
	u16 name##_size(void);						\
									\
	u8 name##_start_write(void);					\
	void name##_add_byte(u8 byte);					\
	void name##_add_word(u16 word);					\
	void name##_add_long(u32 long_integer);				\
	void name##_add_N_bytes(u16 length, const u8* p_buffer);	\
	void name##_stop_write(void);					\
									\
	u8 name##_start_read(void);					\
	u8 name##_get_byte(void);					\
	u16 name##_get_word(void);					\
	u32 name##_get_long(void);					\
	u16 name##_get_N_bytes(u16 length, u8* p_buffer);		\
	void name##_stop_read(void);					\
									\
	prefix void name##_debug_print(void);

#define BUILD_LOCAL_MSG_BUFFER(prefix, name, size)															\
																					\
	static u8 __##name##_msg_buffer[size];																\
	static LOCAL_MSG_BUFFER_DESCR __##name##_msg_buffer_descr = {													\
		__##name##_msg_buffer, /* u8* p_buffer; */														\
		0, 	/* u16 read_pointer; */																\
		0, 	/* u16 write_pointer; */															\
		0,	/* u16 num_elements; */																\
		size, 	/* u16 size; */																	\
		0	/* u8 status_flags; */																\
	};																				\
																					\
	prefix void name##_init(void) 						{ local_msg_buffer_init(&__##name##_msg_buffer_descr); }				\
	prefix u16 name##_bytes_available(void) 				{ return local_msg_buffer_bytes_available(&__##name##_msg_buffer_descr); }		\
	prefix u16 name##_bytes_free(void) 					{ return local_msg_buffer_bytes_free(&__##name##_msg_buffer_descr); }			\
	prefix void name##_clear_all(void) 					{ local_msg_buffer_clear_all(&__##name##_msg_buffer_descr); }				\
																					\
	prefix u8 name##_start_write(void)					{ return local_msg_buffer_start_write(&__##name##_msg_buffer_descr); }			\
	prefix void name##_add_byte(u8 byte) 					{ local_msg_buffer_add_byte(&__##name##_msg_buffer_descr, byte); }			\
	prefix void name##_add_word(u16 word) 					{ local_msg_buffer_add_word(&__##name##_msg_buffer_descr, word); }			\
	prefix void name##_add_long(u32 long_integer) 				{ local_msg_buffer_add_long(&__##name##_msg_buffer_descr, long_integer); }		\
	prefix void name##_add_N_bytes(u16 length, const u8* p_buffer)		{ local_msg_buffer_add_N_bytes(&__##name##_msg_buffer_descr, length, p_buffer); }	\
	prefix void name##_stop_write(void)					{ local_msg_buffer_stop_write(&__##name##_msg_buffer_descr); } 				\
																					\
	prefix u8 name##_start_read(void)					{ return local_msg_buffer_start_read(&__##name##_msg_buffer_descr); }			\
	prefix u8 name##_get_byte(void) 					{ return local_msg_buffer_get_byte(&__##name##_msg_buffer_descr); }			\
	prefix u16 name##_get_word(void) 					{ return local_msg_buffer_get_word(&__##name##_msg_buffer_descr); }			\
	prefix u32 name##_get_long(void) 					{ return local_msg_buffer_get_long(&__##name##_msg_buffer_descr); }			\
	prefix u16 name##_get_N_bytes(u16 length, u8* p_buffer) 		{ return local_msg_buffer_get_N_bytes(&__##name##_msg_buffer_descr, length, p_buffer); }\
	prefix void name##_stop_read(void)					{ local_msg_buffer_stop_read(&__##name##_msg_buffer_descr); }				\
																					\
	prefix void name##_debug_print(void)					{ local_msg_buffer_debug_print(&__##name##_msg_buffer_descr); }				\
																					\
	prefix u16 name##_size(void)						{ return size; }

#define BUILD_FAST_MSG_BUFFER(prfeix, name, size)															\
																					\
	static u8  __##name##_msg_buffer[size];																\
	static u16 __##name##_msg_buffer_size = size;															\
	static u16 __##name##_read_counter = 0;																\
	static u16 __##name##_write_counter = 0;															\
																					\
	inline void name##_init(void) 						{ __##name##_read_counter = 0; __##name##_write_counter = 0; }				\
	inline u16 name##_bytes_available(void) 				{ return (__##name##_write_counter - __##name##_read_counter); }			\
	inline u16 name##_bytes_free(void) 					{ return (__##name##_msg_buffer_size - __##name##_write_counter); }			\
	inline void name##_clear_all(void) 					{ __##name##_read_counter = 0; __##name##_write_counter = 0; }				\
																					\
	inline u8 name##_start_write(void)					{ return 1;}										\
	inline void name##_add_byte(u8 byte) 					{ __##name##_msg_buffer[__##name##_write_counter++] = byte; }				\
	inline void name##_add_word(u16 word) 					{  }											\
	inline void name##_add_long(u32 long_integer) 				{  }											\
	inline void name##_add_N_bytes(u16 length, const u8* p_buffer)		{ u16 i = 0; 										\
										  for ( ; i < length; i++) {								\
											__##name##_msg_buffer[__##name##_write_counter++] = p_buffer[i];		\
										  }											\
										}											\
	inline void name##_stop_write(void)					{  } 											\
																					\
	inline u8 name##_start_read(void)					{ return 1; }										\
	inline u8 name##_get_byte(void) 					{ return __##name##_msg_buffer[__##name##_read_counter++]; }				\
	inline u16 name##_get_word(void) 					{ return 0; }										\
	inline u32 name##_get_long(void) 					{ return 0; }										\
	inline u16 name##_get_N_bytes(u16 length, u8* p_buffer) 		{ u16 i = 0;										\
										  for ( ; i < length; i++) {								\
											p_buffer[i] = __##name##_msg_buffer[__##name##_read_counter++];			\
										  }											\
										}											\
	inline void name##_stop_read(void)					{  }											\
																					\
	inline u16 name##_size(void)						{ return size; }
	
/*!
 *
 */
#ifndef LOCAL_MSG_BUFFER_INVALID_CHARACTER
#define LOCAL_MSG_BUFFER_INVALID_CHARACTER	0x00
#endif

#define LOCAL_MSG_BUFFER_NO_PREFIX


/*!
 *
 * @param p_buffer_descr
 */
void local_msg_buffer_init(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 */
u8 local_msg_buffer_start_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @param byte
 */
void local_msg_buffer_add_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u8 byte);


/*!
 *
 * @param p_buffer_descr
 * @param word
 */
void local_msg_buffer_add_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 word);


/*!
 *
 * @param p_buffer_descr
 * @param long_integer
 */
void local_msg_buffer_add_long(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u32 long_integer);


/*!
 *
 * @param p_buffer_descr
 * @param num_bytes
 * @param p_ext_buffer
 */
void local_msg_buffer_add_N_bytes(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, const u8* p_buffer_from);


/*!
 *
 * @param p_buffer_descr
 */
void local_msg_buffer_stop_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 */
u8 local_msg_buffer_start_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @return
 */
u16 local_msg_buffer_bytes_available(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @return
 */
u16 local_msg_buffer_bytes_free(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @return
 */
u8 local_msg_buffer_get_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @return
 */
u16 local_msg_buffer_get_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @return
 */
u32 local_msg_buffer_get_long(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 * @param num_bytes
 * @param p_buffer_to
 * @return
 */
u16 local_msg_buffer_get_N_bytes(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, u8* p_buffer_to);

/*!
 *
 * @param p_buffer_descr
 */
void local_msg_buffer_stop_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 */
void local_msg_buffer_clear_all(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


/*!
 *
 * @param p_buffer_descr
 */
void local_msg_buffer_debug_print(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr);


#endif // _LOCAL_MSG_BUFFER_H_
