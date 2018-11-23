//--------------------------------------------------------------------------------------------------------------
//       Hardwarebeschreibung   :  Evaluation Board
//					Microcontroller :	ATMega1284P
//					Versorgungsspannung :	3 Volt 
//					Taktfrequenz :		14,7456 MHz
//					Weitere IC's :		FTDI
//					PCB Anschluesse :	40 Pin GPIO-Heder
//								12 Pol Phönix Kontaktstecker
//								Mini USB
//--------------------------------------------------------------------------------------------------------------
//

#define CPU_XTAL_HZ		14745600    	//Frequency of the attached XTAL
#define CPU_PRESCALER           2
#define XTAL_RTC    		32786

#ifndef XTAL
 #ifdef CPU_PRESCALER
  #define XTAL (CPU_XTAL_HZ/(CPU_PRESCALER))
 #else
  #define XTAL CPU_XTAL_HZ
 #endif
#endif

#include <cpu_incl.h>

//----------------------------------------------------------------------------------------------------------------------------------
//      Konfiguration fuer RTOS Timer
#define RTOS_TIMER                      1
#define RTOS_TCOMP                      B

#define MSTICKER_OCTIMER        RTOS_TIMER
#define MSTICKER_OCCHANNEL      RTOS_TCOMP

//----------------------------------------------------------------------------------------------------------------------------------
//      Konfiguration fuer Beeper Timer
#define BEEPER_TIMER 		1
#define BEEPER_TCOMP 		B

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------  Name		, Port	, Bit	, STARTUP			, SLEEPMODE

 DEF_GPIO_2( EXT_INPUT_1	, A	, 0	, INPUT_PULLUP | INVERTED 	, INPUT_PULLUP | INVERTED	) //
 DEF_GPIO_2( EXT_INPUT_2 	, A	, 1	, INPUT_PULLUP | INVERTED 	, INPUT_PULLUP | INVERTED	) //
 DEF_GPIO_2( EXT_INPUT_3	, A	, 2	, INPUT_PULLUP | INVERTED	, INPUT_PULLUP | INVERTED	) //
 DEF_GPIO_2( EXT_INPUT_4   	, A	, 3	, INPUT_PULLUP | INVERTED	, INPUT_PULLUP | INVERTED	) //
 DEF_GPIO_2( EVENT_IRQ	   	, A	, 4	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( EXT_5V   		, A	, 5	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( SPI1_CE        	, A	, 6	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( UNUSED_A7 		, A	, 7	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------  Name		, Port	, Bit	, BOOTLOADER			, SLEEPMODE

 DEF_GPIO_2( EXT_OUTPUT_4      	, B	, 0	, OUTPUT_OFF			, OUTPUT_OFF			) //
 DEF_GPIO_2( EXT_OUTPUT_3     	, B	, 1	, OUTPUT_OFF			, OUTPUT_OFF			) //
 DEF_GPIO_2( EXT_OUTPUT_2 	, B	, 2	, OUTPUT_OFF			, OUTPUT_OFF			) //
 DEF_GPIO_2( EXT_OUTPUT_1     	, B	, 3	, OUTPUT_OFF			, OUTPUT_OFF			) //
 DEF_GPIO_2( SPI0_CE	        , B	, 4	, INPUT_PULLUP			, INPUT_PULLUP			) // Anpassung fuer Kleverkey
 DEF_GPIO_2( SPI0_MOSI      	, B	, 5	, INPUT_PULLUP			, INPUT_PULLUP			) //
 DEF_GPIO_2( SPI0_MISO     	, B	, 6	, OUTPUT_ON			, OUTPUT_ON			) //
 DEF_GPIO_2( SPI0_SCK      	, B	, 7	, INPUT_HIGH_Z			, INPUT_HIGH_Z			) //

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------  Name        	Port	, Bit	, BOOTLOADER			, SLEEPMODE

 DEF_GPIO_2( I2C0_SCL      	, C	, 0	, OUTPUT_ON			, OUTPUT_ON			) //
 DEF_GPIO_2( I2C0_SDA     	, C	, 1	, OUTPUT_ON 			, OUTPUT_ON			) //
 DEF_GPIO_2( EXT_3V 		, C	, 2	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( EXT_RESET       	, C	, 3	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( UNUSED_C4         	, C	, 4	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( UNUSED_C5      	, C	, 5	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( TOSC1        	, C	, 6 	, OUTPUT_OFF 			, OUTPUT_OFF 			) //
 DEF_GPIO_2( TOSC2        	, C	, 7 	, OUTPUT_OFF 			, OUTPUT_OFF 			) //

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------  Name        	Port	, Bit	, BOOTLOADER			, SLEEPMODE

 DEF_GPIO_2( UART0_RXD      	, D	, 0	, INPUT_PULLUP			, INPUT_PULLUP			) //
 DEF_GPIO_2( UART0_TXD     	, D	, 1	, OUTPUT_ON	 		, OUTPUT_ON			) //
 DEF_GPIO_2( UART1_RXD 		, D	, 2	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( UART1_TXD       	, D	, 3	, OUTPUT_OFF			, OUTPUT_ON			) //
 DEF_GPIO_2( SPI1_SCK         	, D	, 4	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( UNUSED_D5 		, D	, 5	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( GPIO_27     	, D	, 6	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //
 DEF_GPIO_2( IS_BUSY      	, D	, 7	, OUTPUT_OFF | INVERTED		, OUTPUT_OFF | INVERTED		) //


//----------------------------------------------------------------------------------------------------------------------------------
//   GPIO - Aliases
 GPIO_ALIAS( LED_GRN      	, D, 7 , OUTPUT_OFF | INVERTED	)
 GPIO_ALIAS( LED_RED      	, C, 4 , OUTPUT_OFF | INVERTED	)
 GPIO_ALIAS( LED_YELLOW      	, A, 6 , OUTPUT_OFF | INVERTED	)
 
 GPIO_ALIAS( UART1_SEND485      , D, 4 , OUTPUT_OFF | INVERTED	)
 
 GPIO_ALIAS( SPI1_MOSI	        , D, 2 , OUTPUT_OFF | INVERTED	)
 GPIO_ALIAS( SPI1_MISO	        , D, 3 , OUTPUT_OFF | INVERTED	)

//----------------------------------------------------------------------------------------------------------------------------------
#define PCMSK_D        (NF_CL_DAT_PCMSK | PWR_DETECT_PCMASK)

// Bootloader verwendet SET_PORT_INITIAL_STATES(1)
#define INIT_PORTS_FOR_STARTUP()     	SET_PORT_INITIAL_STATES(1)
#define INIT_PORTS_AFTER_SLEEP_MODE()
#define INIT_PORTS_FOR_SLEEP_MODE()	SET_PORT_INITIAL_STATES(2)


// ---- Dummy Functions ------------------------------------------------------------------------------------------------------------
