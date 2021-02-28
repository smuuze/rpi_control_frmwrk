#ifndef _EXCEPTION_INTERFACE_H_
#define _EXCEPTION_INTERFACE_H_

// --------------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------------

#ifdef EXCEPTION_HANDLING_AVAILABLE

#define EXCEPTION_THROW(name)				\
							\
	extern void exception_throw_##name(void);	\
	exception_throw_##name()		

#define EXCEPTION_HANDLE(name, code_block)		\
							\
	void exception_throw_##name(void) {		\
		code_block				\
	}

#else

#define EXCEPTION_THROW(u32_exception_id)	do{}while(0)	
#define EXCEPTION_HANDLE(name, code_block)

#endif

// --------------------------------------------------------------------------------------

#endif // _EXCEPTION_INTERFACE_H_
