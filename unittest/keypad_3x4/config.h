
#ifndef   _config_H_ /* parse include file only once */
#define   _config_H_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#define BOARD_DESCRIPTION_FILE                          "platine/board_UNITTEST.h"
#include "platine/board_UNITTEST.h"

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

INCLUDE_GPIO ( 	LCD_RS )
INCLUDE_GPIO ( 	LCD_EN )
INCLUDE_GPIO ( 	LCD_D4 )
INCLUDE_GPIO ( 	LCD_D5 )
INCLUDE_GPIO ( 	LCD_D6 )
INCLUDE_GPIO ( 	LCD_D7 )

// --------------------------------------------------------------------------------

#endif /* _config_H_ */

// --------------------------------------------------------------------------------
