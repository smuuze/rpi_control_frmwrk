
#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_UNITTEST.h"
#include "platine/board_UNITTEST.h"

//-------------------------------------------------------------------------

// THe initialization routine needs this define to execute gpio init
#define HAS_DRIVER_GPIO 1

// --------------------------------------------------------------------------------

#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS	0

// --------------------------------------------------------------------------------

#ifndef __UNUSED__
#define __UNUSED__ __attribute__((unused))
#endif

#ifndef __USED__
#define __USED__ __attribute__((__used__))
#endif

#ifndef __PACKED__
#define __PACKED__ __attribute__((packed))
#endif

// --------------------------------------------------------------------------------

#ifndef __CONCAT
#define __CONCAT(a, b) a ## b
#endif

#ifndef __STRING
#define __STRING(x) #x
#endif

#ifndef __UNSIGNED
#define __UNSIGNED(x)   x##u
#endif

#ifndef __ALWAYS_INLINE
#define __ALWAYS_INLINE inline
#endif

// --------------------------------------------------------------------------------

extern u8 DDRA;
extern u8 DDRB;
extern u8 DDRC;
extern u8 DDRD;

extern u8 PORTA;
extern u8 PORTB;
extern u8 PORTC;
extern u8 PORTD;

extern u8 PINA;
extern u8 PINB;
extern u8 PINC;
extern u8 PIND;

// --------------------------------------------------------------------------------

#endif /* _config_H_ */

// --------------------------------------------------------------------------------
