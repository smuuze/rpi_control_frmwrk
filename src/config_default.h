//-------------------------------------------------------------------------
//       ZENTRALE DEFAULT WERTE FUER SWITCHES
//-------------------------------------------------------------------------
//
// Diese Datei wird �ber die config.h von allen Projekten eingebunden.
// ALLE Schalter werden hier auf eine Voreinstellung gesetzt, sofern sie
// noch nicht bekannt sind, wenn dies hier eingelesen wird.
//
// Wenn ein neuer Schalter gebraucht wird, wird dieser zuerst hier eingef�gt
// und auf einen 'neutralen' Wert gesetzt. Das ist normalerweise die Einstellung,
// die den selben Code wie bisher erzeugt. In dem Projekt, das die neue
// Funktion ben�tigt, kann dieser neue Schalter dann auf einen anderen Wert
// gesetzt werden. Das muss aber in config.h passieren, BEVOR diese
// Datei eingebunden wird.

// Strommmesungen mit einem PRD: nur EV1 und Mifare Classic sind aktiv
// 50ms  => ?mA
// 100ms => ?mA
// 150ms => ?mA
// 200ms => ?mA
// 300ms => ?mA
#define READ_INTERVALL_TIME 200  // Wartezeit zwischen 2 Leseversuchen

//-----------------------------------------------------------------------------

#define TEST_HANDLER_NO_READ_TAG_MONITOR

//#ifndef SET_UNUSED_BITS2ZERO
//#define SET_UNUSED_BITS2ZERO
//#endif

//-----------------------------------------------------------------------------
#ifdef SIO_FOR_COPRO1
  #define copro1_init_proto void init_copro1_slave_state_machine(void);
  #define copro1_func_proto void copro1_slave_state_machine(void);
  #define copro1_init_call  init_copro1_slave_state_machine();
  #define copro1_func_call  copro1_slave_state_machine();
#else
  #define copro1_init_proto // do nothing
  #define copro1_func_proto // do nothing
  #define copro1_init_call  // do nothing
  #define copro1_func_call  // do nothing
#endif

// -- I2C Interface

#ifdef I2C_USE_MSGFIFO
  #define i2c_driver_init_proto void init_i2c_driver_polling_function(void);
  #define i2c_driver_func_proto void i2c_driver_polling_function(void);
  #define i2c_driver_init_call  init_i2c_driver_polling_function();
  #define i2c_driver_func_call  i2c_driver_polling_function();
#else
  #define i2c_driver_init_proto // do nothing
  #define i2c_driver_func_proto // do nothing
  #define i2c_driver_init_call  // do nothing
  #define i2c_driver_func_call  // do nothing
#endif

//----------------------------------------------------------------------------
//      Konfiguration fuer Systemtimer
//----------------------------------------------------------------------------
#ifdef CPU_PRESCALER
  #define config_CPU_CLK_PRESCALER (CPU_PRESCALER)
#else
  #define config_CPU_CLK_PRESCALER 1
#endif

#ifdef config_CPU_CLK_HZ
ASSERT_C(0, config_CPU_CLK_HZ_ALREADY_DEFINED);
#endif

#ifndef config_CPU_CLK_HZ
#define  config_CPU_CLK_HZ  ((CPU_XTAL_HZ)/(config_CPU_CLK_PRESCALER))
#endif

// Minimalwert fuer Timerueberlauf : 32768us (d.h. TCLK <= 2MHz)
// Maximalwert fuer Timerstep : 4us (d.h. TCLK >= 250kHz)
#if   ( config_CPU_CLK_HZ <= 2000000 )
  #define config_SYSTEMTIMER_PRESCALER 1
#elif ( config_CPU_CLK_HZ <= 16000000 )
  #define config_SYSTEMTIMER_PRESCALER 8
#else
  #define config_SYSTEMTIMER_PRESCALER 64
#endif

//----------------------------------------------------------------------------

#ifndef config_RF_CLK_HZ
#define config_RF_CLK_HZ    (125000)
#endif

#if SIO_FOR_HOST == '-'
#define RTOS_H "../src/fakertos/fakertos.h"
#else
#define RTOS_H "fakertos.h"
#endif

#ifndef config_FAKERTOS_TASKYIELD_INIT_PROTO
#define config_FAKERTOS_TASKYIELD_INIT_PROTO
#endif

#ifndef config_FAKERTOS_TASKYIELD_FCT_PROTO
#define config_FAKERTOS_TASKYIELD_FCT_PROTO		\
	void mcu_task_controller_background_run(void);
#endif

#define config_FAKERTOS_TASKYIELD_INIT

#ifndef config_FAKERTOS_TASKYIELD_FUNCTION
#define config_FAKERTOS_TASKYIELD_FUNCTION		mcu_task_controller_background_run();
#endif


#ifndef SW_TYPE_FOR_KEYMARKE
#define SW_TYPE_FOR_KEYMARKE 1
#endif

#ifndef MIT_TAMPER
#define MIT_TAMPER 1 // Aktiviert die Tamperabfrage/Ausgabe
#endif

//------------------------------------------------------------------------------
// -- Call Back Funktionen fuer die Tastatur
#ifdef keypadVersion
#define config_KEYPAD_ENTER_KEY_FCT        keypad_enter_event();
#define config_KEYPAD_ENTER_KEY_FCT_PROTO  extern void keypad_enter_event(void);

#define config_KEYPAD_CLEAR_KEY_FCT        keypad_clear_event();
#define config_KEYPAD_CLEAR_KEY_FCT_PROTO  extern void keypad_clear_event(void);

//#define config_KEYPAD_NUMBER_KEY_FCT       keypad_number_event();
//#define config_KEYPAD_NUMBER_KEY_FCT_PROTO extern void keypad_number_event(void);

// #define config_KEYPAD_TIMEIUT_FCT
// #define config_KEYPAD_TIMEOUT_FCT_PROTO
#endif
///-----------------------------------------------------------------------------
#ifndef MINIMUM_TIMESTAMP_FOR_CARD_PROCESS
// the actual Date / Time is 18.12.2015 - 10:37:41 (GMT)
// => 1450435061 seconds since 1.1.1970 - 00:00:00
#define MINIMUM_TIMESTAMP_FOR_CARD_PROCESS	0x5673E1F5
#endif

///-----------------------------------------------------------------------------

#ifndef config_HAS_ONBOARD_BUTTONS
#define config_HAS_ONBOARD_BUTTONS				0
#endif

#ifndef config_HAS_LED_MATRIX
#define config_HAS_LED_MATRIX					0
#endif

///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
/// 0x21BAxxxx => Ist die Bezeichnung f�r die allgemeong�ltige, PRLxA\CRLxA Konfigueration.
#ifndef CONFIG_MEM_SCHEME
  #define KONFIGVERSION 2
  #define CONFIG_MEM_SCHEME    (0x21BA*65536+KONFIGVERSION)
#endif // CONFIG_MEM_SCHEME

#ifndef BOOTLOADER_EXTENSION
#define BOOTLOADER_EXTENSION  0
#endif

#ifndef CFG_DEFAULT_VALUE_SELECTOR
#pragma NO____CFG_DEFAULT_VALUE_SELECTOR____SELECTED
#endif
///-----------------------------------------------------------------------------
//Global memory for reading routines. The memory space may be used by reading
//routines freely. The application does not use gmem_read while reading
//routines are active. Consider using the OVERLAY-Macro, as there is
//intentionally no reference in any header file like this:
//u8 gmem_read[config_SIZEOF_gmem_read];
#ifndef  config_SIZEOF_gmem_read
#define config_SIZEOF_gmem_read    (330)
#endif

//Gobal read_tagstruct. Tag reading routines have to return tag data in this
//struct. The memory space of read_tagstruct may be used by reading routines
//freely. The application does not use read_tagstruct while reading routines
//are active. To use: #include "service/deister/tagstruct.h"
//Data is only valid, if a reading routine returned with RFID_READ_SUCCESS.
//Size of the data array in the struct:
#ifndef  config_SIZEOF_TAGSTRUCT_DATA
#define config_SIZEOF_TAGSTRUCT_DATA    (31)
#endif
//Size of tagstruct.ext.GENERIC_SERNUM.sernum[] (may be 0 if not needed)
#ifndef	config_SIZEOF_TAGSTRUCT_EXT_GENERIC_SERNUM_SERNUM
#define	config_SIZEOF_TAGSTRUCT_EXT_GENERIC_SERNUM_SERNUM (8)
#endif

///-----------------------------------------------------------------------------
/// Die Verschluesselung auf dem Hostinterface soll Standardmaessig moeglich sein
#ifndef DEBUS_DEVICE_AES_ENCRYPTION
#define DEBUS_DEVICE_AES_ENCRYPTION 1
#endif


///-----------------------------------------------------------------------------
/// Driver Interface

#define config_I2C_POWER_DOWN_PROTOTYPE				void specific_i2c_power_down(void);
#define config_I2C_POWER_DOWN_FUNCTION_REF			&specific_spi_power_down

#define config_SPI_POWER_DOWN_PROTOTYPE				void specific_spi_power_down(void);
#define config_SPI_POWER_DOWN_FUNCTION_REF			&specific_spi_power_down

///-----------------------------------------------------------------------------
/// System Interface


#define config_SYSTEM_INTERFACE_ADD_EVENT_PROTOTYPE		void local_event_add(SYSTEM_EVENT event);
#define config_SYSTEM_INTERFACE_GET_EVENT_PROTOTYPE		SYSTEM_EVENT local_event_get_next(void);

#define config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK		local_event_add
#define config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK		local_event_get_next

#define config_SYSTEM_INTERFACE_GET_TIME_U8_PROTOTYPE		u8  local_rtc_timer_gettime_u8(void); /*ux8 msticker_get_time_ux8(void);*/
#define config_SYSTEM_INTERFACE_GET_TIME_U16_PROTOTYPE		u16 local_rtc_timer_gettime_u16(void); /*ux16 msticker_get_time_ux16(void);*/
#define config_SYSTEM_INTERFACE_GET_TIME_U32_PROTOTYPE		u32 local_rtc_timer_gettime_u32(void); /*ux32 msticker_get_time_ux32(void);*/

#define config_SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK		local_rtc_timer_gettime_u8 /*msticker_get_time_ux8*/
#define config_SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK		local_rtc_timer_gettime_u16 /*msticker_get_time_ux16*/
#define config_SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK		local_rtc_timer_gettime_u32 /*msticker_get_time_ux32*/

#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_PROTOTYPE		u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval); /*ux8 mstimer_time_is_up_ux8(const ux8 reference_time, const ux8 time_interval);*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_PROTOTYPE	u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval); /*ux8 mstimer_time_is_up_ux16(const ux16 reference_time, const ux16 time_interval);*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_PROTOTYPE	u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval); /*ux8 mstimer_time_is_up_ux32(const ux32 reference_time, const ux32 time_interval);*/

#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK		local_rtc_timer_istimeup_u8 /*mstimer_time_is_up_ux8*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_CALLBACK		local_rtc_timer_istimeup_u16 /*mstimer_time_is_up_ux16*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK		local_rtc_timer_istimeup_u32 /*mstimer_time_is_up_ux32*/

#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE	static u8 local_progmem_get_byte(u8* addr);
#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE	static u16 local_progmem_get_word(u8* addr);
#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE	static u32 local_progmem_get_long(u8* addr);
#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE	static void local_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes);

#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK	local_progmem_get_byte
#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK	local_progmem_get_word
#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK	local_progmem_get_long
#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK	local_progmem_get_N_bytes

#define config_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE void gpio_driver_set_direction(u8 pin_num, SYSTEM_INTERFACE_GPIO_DIRECTION direction);
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE void gpio_driver_set_level(u8 pin_num, SYSTEM_INTERFACE_IO_PIN_LEVEL level);
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE SYSTEM_INTERFACE_IO_PIN_LEVEL gpio_driver_get_level(u8 pin_num);

#define config_INTERFACE_IO_SET_PIN_DIR_CALLBACK gpio_driver_set_direction
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK gpio_driver_set_level
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK gpio_driver_get_level


///-----------------------------------------------------------------------------

#define config_IS_READY_ENABLE					IS_READY_as_OUTPUT();		\
								IS_READY_OFF()
#define config_IS_READY_IDLE					IS_READY_as_OUTPUT();		\
								IS_READY_ON();
#define config_IS_READY_DISABLE					IS_READY_as_INPUT();		\
								IS_READY_OFF()

///-----------------------------------------------------------------------------

#if !defined (config_OFFLINE_MODE_ENTER_FUNC_PROTO) && !defined (config_OFFLINE_MODE_LEAVE_FUNC_PROTO)
#define config_OFFLINE_MODE_ENTER_FUNC_PROTO
#define config_OFFLINE_MODE_ENTER_FUNC_CALL
#endif

#if !defined (config_OFFLINE_MODE_LEAVE_FUNC_PROTO) && !defined (config_OFFLINE_MODE_LEAVE_FUNC_CALL)
#define config_OFFLINE_MODE_LEAVE_FUNC_PROTO
#define config_OFFLINE_MODE_LEAVE_FUNC_CALL
#endif

#if !defined config_DEBUS_SET_ACTIVE_FUNC_PROTO && !defined config_DEBUS_SET_ACTIVE_FUNC_CALL && !defined config_DEBUS_SET_INACTIVE_FUNC_CALL
#define config_DEBUS_SET_ACTIVE_FUNC_PROTO
#define config_DEBUS_SET_ACTIVE_FUNC_CALL
#define config_DEBUS_SET_INACTIVE_FUNC_CALL
#endif

///-----------------------------------------------------------------------------
// Ram Zuweisung fuer die open collector Protokole. Dieser Platz wird vom
// Protokoll nur temporaer, fuer die Dauer der Ausfuehrung benutzt.
#ifndef OC_DATA
#define OC_DATA gmem_read  // Speicherplatz fuer den OC_DATA
#endif

#define SESAM_SPEZIAL
///-----------------------------------------------------------------------------
//DOORLOXX
#ifdef DOORLOXX_BUSINESS_ACTIVE
#define DOORLOXX_BUSINESS_ACTIVE_OVER_COPRO
#endif

#ifdef DOORLOXX_BUSINESS_PASSIVE
#define EE_DOORGROOPID_OFFSET 1
#define EE_DOORGROOPID_SIZE 0
#endif
///-----------------------------------------------------------------------------
/// -- Projekt abh�ngige EEPROM Belegung. Zur besseren Kontrolle der EEPROM
///    Belegung, sollen alle Platzreservierungen hier aufgelistet werden!
//DoorLoxx
#define EE_MAXIMUM_LAST_REFRESH_TIME	0x610
			//bis 		0x613
#define EE_EUI_64_ADR	0x614
			//bis 0x61B (8 byte) airlink Adresse
//DoorLoxx
#define EE_SYSTEMCODE_OFFSET 		0x61C
			//bis 		0x61F
#define EE_DL_COUNTER			0x620
			//bis 		0x623
			//------------------------
                        //     0x624 frei
                        //     ...
                        //------------------------
#define AESKEYMEMORY           0x6E0
                        //     0x6E0-0x6EF => 16 Byte Marker
                        //     0x6F0-0x6FF => 16 Byte Zufall Key
                        //     0x700-0x83F => 16*ANZAHL_AESKEYS (bei ANZAHL_AESKEYS = 20)
                        //------------------------
                        //     0x840 frei bei ANZAHL_AESKEYS = 16
#define EE_BLACKLIST_OFFSET    	0x840 //128 Eintrage a 4 Byte
#define EE_BLACKLIST_SIZE	0x200

///-----------------------------------------------------------------------------
