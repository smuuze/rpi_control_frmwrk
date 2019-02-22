#include "config.h"

#define noTRACES  // IN DIESEM MODUL BITTE NICHT ABSCHALTEN!
#include "cpu/avr/iardebus/traces.h"

#include "service/osal/osal.h"

#if (RTOSMODE == RTOSMODE_FAKERTOS)

#include "utils/stdmacros.h"

#include "fakertos.h"

////////////////////////////////////////////////////////////////////////////////
// Simple systimer interface
////////////////////////////////////////////////////////////////////////////////
ux8 systimer_time_is_up(const systimer_type reference_time,
			const systimer_type time_interval)
{
	return ((((systimer_type) (_systimer_get_time() - reference_time))
	                > time_interval) ? 1 : 0);
}


////////////////////////////////////////////////////////////////////////////////
// Milliseconds ticker.
////////////////////////////////////////////////////////////////////////////////

__WEAK__ u32 msticker_time;
u8 ocr256;

// msticker_time ist __WEAK__ deklariert, um die Nutzung des Tickers fuer
// traces.avr zu ermoeglichen. 
// Mit folgender Zeile in config.h aktiviert man das Feature:
// #define TRACE_FAKE_RTOS_TIME_STAMP 2
// TRACE_FAKE_RTOS_TIME_STAMP gibt an, wie viele Bytes des Tickers jeder
// TRACE-Ausgabe vorangestellt werden sollen. Wenn das aktiviert ist,
// legt traces.avr eine 4 Byte-Variable msticker_time an.
ASSERT_C(sizeof(msticker_time)==4,passt_nicht_zu_msticker_time_in_traces_avr);


ux32 msticker_get_time_ux32(void)
{
	ux32 t;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		t = (ux32) msticker_time;
	}
	return t;
}

ux16 msticker_get_time_ux16(void)
{
	ux16 t;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		t = (ux16) msticker_time;
	}
	return t;
}

ux8 msticker_get_time_ux8(void)
{
	return (ux8) msticker_time;
}

u8 mstimer_time_since_ux8(const ux8 reference_time)
{
	return (u8)(msticker_get_time_ux8() - reference_time);
}

u16 mstimer_time_since_ux16(const ux16 reference_time)
{
	return (u16)(msticker_get_time_ux16() - reference_time);
}

u32 mstimer_time_since_ux32(const ux32 reference_time)
{
	return (u32)(msticker_get_time_ux32() - reference_time);
}

ux8 mstimer_time_is_up_ux32(const ux32 reference_time, const ux32 time_interval)
{
	return ((((ux32) (msticker_get_time_ux32() - reference_time))
	                > time_interval) ? 1 : 0);
}

ux8 mstimer_time_is_up_ux16(const ux16 reference_time, const ux16 time_interval)
{
	return ((((ux16) (msticker_get_time_ux16() - reference_time))
	                > time_interval) ? 1 : 0);
}

ux8 mstimer_time_is_up_ux8(const ux8 reference_time, const ux8 time_interval)
{
	return ((((ux8) (msticker_get_time_ux8() - reference_time))
	                > time_interval) ? 1 : 0);
}

void fakertos_sleep( uint16_t milli_seconds )
{
        uint16_t start_time = msticker_get_time_ux16();
        while( !mstimer_time_is_up_ux16(start_time, milli_seconds) ) {
                task_yield();
        }
}


#define MSTICK_INCREMENT	US2SYSTIMER(1000)
#define MSTICK_INCREMENT256	US2SYSTIMER(256*1000.0)
#ifndef MSTICK_TOLERANCE_NS
#define MSTICK_TOLERANCE_NS	500
#endif
ASSERT_C((SYSTIMER2US(MSTICK_INCREMENT)*1000.0) > (1000000 - MSTICK_TOLERANCE_NS), systimer_tolerance_too_high);
ASSERT_C((SYSTIMER2US(MSTICK_INCREMENT)*1000.0) < (1000000 + MSTICK_TOLERANCE_NS), systimer_tolerance_too_high);

//ASSERT that the timer is running with a resolution of 0.5 to 4µs
ASSERT_C((SYSTIMER2US(1)*1000.0) >= (500), systimer_is_running_too_fast);
ASSERT_C((SYSTIMER2US(1)*1000.0) <= (4000), systimer_is_running_too_slow);

#define msticker_COMnX0		( combine4(COM,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL,0) )
#define msticker_COMnX1		( combine4(COM,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL,1) )
#define msticker_TCNTn		combine(TCNT,MSTICKER_OCTIMER)
#define msticker_TCCRnA		combine3(TCCR,MSTICKER_OCTIMER,A)
#define msticker_TCCRnB		combine3(TCCR,MSTICKER_OCTIMER,B)
#define msticker_TCCRnC		combine3(TCCR,MSTICKER_OCTIMER,C)
#define msticker_OCRnX		combine3(OCR,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL)
#define msticker_OCRnXL		combine4(OCR,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL,L)
#define msticker_OCRnXH		combine4(OCR,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL,H)

#ifndef TIMSK
#define msticker_TIMSKn		combine(TIMSK,MSTICKER_OCTIMER)
#define msticker_TIFRn		combine2(TIFR, MSTICKER_OCTIMER)
#else  // #ifndef TIMSK
// Sonderbehandlung fuer mega128
// ACHTUNG! Die Position der Steuerbits ist NICHT identisch !!!
#if (MSTICKER_OCTIMER==1)
#define msticker_TIMSKn		TIMSK
#define msticker_TIFRn		TIFR
#elif (MSTICKER_OCTIMER==3)
#define msticker_TIMSKn		ETIMSK
#define msticker_TIFRn		ETIFR
#else  // #if (MSTICKER_OCTIMER==1)
#error MSTICKER_OCTIMER fuer mega128 nur mit Timer1/3 getestet
#endif // #if (MSTICKER_OCTIMER==1)
#endif // #ifndef TIMSK



#define msticker_OCIEnX		( combine3(OCIE,MSTICKER_OCTIMER,MSTICKER_OCCHANNEL) )
#define msticker_OCFnX		combine3(OCF, MSTICKER_OCTIMER, MSTICKER_OCCHANNEL)
#define msticker_TIMERn_COMPX_vect combine5(TIMER,MSTICKER_OCTIMER,_COMP,MSTICKER_OCCHANNEL,_vect)

void msticker_init(void)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		msticker_OCRnX = msticker_TCNTn + MSTICK_INCREMENT;
		__asm__ __volatile__
		(
		//Disable OCR interrupt.
	"	lds	r24, %[TIMSKn]					\n\t"
	"	andi	r24, (~(1<<%[OCIEnX]))				\n\t"
	"	sts	%[TIMSKn], r24					\n\t"
		//OCR mode: normal port operation
	"	lds	r24, %[TCCRnA]					\n\t"
	"	andi	r24, (~((1<<%[COMnX1]) | (1<<%[COMnX0])))	\n\t"
	"	sts	%[TCCRnA], r24					\n\t"
		//Clear interrupt flag
	"	ldi	r24, (1<<%[OCFnX])				\n\t"
	"	sts	%[TIFRn], r24					\n\t"
		//Enable OCR and global interrupts
	"	lds	r24, %[TIMSKn]					\n\t"
	"	ori	r24, (1<<%[OCIEnX])				\n\t"
	"	sts	%[TIMSKn], r24					\n\t"
		:
		:	[TIMSKn] "p" (_SFR_MEM_ADDR(msticker_TIMSKn)),
			[OCIEnX] "i" (msticker_OCIEnX),
			[TIFRn] "p" (_SFR_MEM_ADDR(msticker_TIFRn)),
			[OCFnX] "i" (msticker_OCFnX),
			[TCCRnA] "p" (_SFR_MEM_ADDR(msticker_TCCRnA)),
			[COMnX1] "i" (msticker_COMnX1),
			[COMnX0] "i" (msticker_COMnX0),
			[TCCRnC] "p" (_SFR_MEM_ADDR(msticker_TCCRnC))
		:	"r24"
		);
	}
//	ATOMIC_BLOCK(ATOMIC_FORCEON) {
//		u16 t = TCNT1;
//		u16 o = OCR1B;
//		tkdebug_byte(TCCR1A);
//		tkdebug_byte(TCCR1B);
//		tkdebug_byte(TCCR1C);
//		tkdebug_byte(TIMSK1);
//		tkdebug_byte(TIFR1);
//		tkdebug_byte(u16_to_u8high(t));
//		tkdebug_byte(u16_to_u8low(t));
//		tkdebug_byte(u16_to_u8high(o));
//		tkdebug_byte(u16_to_u8low(o));
//		tkdebug_byte(0x0A);
//	}
}

//AVR - Worst case interrupt response time, if interrupts are enabled!
//	<=128kB >128kB ROM
//	+4	(+5)	multi-cycle instruction is completed first (ret)
//	+4 	(+5)	PC is pushed to stack and jump to vector table
//	+2/+3 	(+2/+3)	rjmp to ISR (use __CLOSE_TO_VECTORS__)
//			extra clock cycle if jmp to ISR (instead of rjmp)
//	=10/11	=12/13
void __CLOSE_TO_VECTORS__ __NAKED__ __SIGNAL__ msticker_TIMERn_COMPX_vect(void);
void msticker_TIMERn_COMPX_vect(void)
{	//interrupts are disabled for up to 13 clocks till now.
	RESET_REGISTERS_USED();
	asm __volatile__
	(
	IGNORE_REGISTER_USED(30)
"	push	r30							\n\t"
"	in	r30, __SREG__						\n\t"
"	push	r30							\n\t"
	:
	:
	:	"r30"
	);				// +5 = 16 clocks
	push_registers("MSTICKER_1","31"); // +2 = 18 clocks to interrupt enable

	//Writing OCR uses temp register and must be protected (reading is okay)
	//Writing 16 bit timer register with temp: Write high byte first!
	//Reading 16 bit timer register with temp: Read low byte first!
	//msticker_OCRnX += MSTICK_INCREMENT;
	asm __volatile__
	(
	REGISTER_USED(30)
	REGISTER_USED(31)
"	lds	r30, %[ocr256]						\n\t"
"	subi	r30, lo8(%[_mstickinc256])				\n\t"
"	sts	%[ocr256], r30						\n\t"
"	lds	r30, %[ocrl]						\n\t"
"	lds	r31, %[ocrh]						\n\t"
"	sbci	r30, hi8(%[_mstickinc256])				\n\t"
"	sbci	r31, hlo8(%[_mstickinc256])				\n\t"
"	sts	%[ocrh], r31						\n\t"
"	sei								\n\t"
"	sts	%[ocrl], r30						\n\t"
	:
	:	[ocrl] "p" (_SFR_MEM_ADDR(msticker_OCRnXL)),
		[ocrh] "p" (_SFR_MEM_ADDR(msticker_OCRnXH)),
	 	[ocr256] "p" (&ocr256),
		[_mstickinc256] "i" (-(MSTICK_INCREMENT256))
	);

	pop_registers("MSTICKER_1");
	//msticker_time++;
	ASSERT_SAME_TYPES(typeof(msticker_time), u32);
	asm __volatile__
	(
	REGISTER_USED(30)
"	cli								\n\t"
"	lds	r30, (%[time_p]+0)					\n\t"
"	subi	r30, lo8(-1 >>  0)					\n\t"
"	sts	(%[time_p]+0), r30					\n\t"
"	lds	r30, (%[time_p]+1)					\n\t"
"	sbci	r30, lo8(-1 >>  8)					\n\t"
"	sts	(%[time_p]+1), r30					\n\t"
"	lds	r30, (%[time_p]+2)					\n\t"
"	sbci    r30, lo8(-1 >> 16)					\n\t"
"	sts	(%[time_p]+2), r30					\n\t"
"	lds	r30, (%[time_p]+3)					\n\t"
"	sbci	r30, lo8(-1 >> 24)					\n\t"
"	sei								\n\t"
"	sts	(%[time_p]+3), r30					\n\t"
	:
	:	[time_p] "p" (&msticker_time)
	);

	CHECK_PUSH_POP_REGISTERS();

	asm __volatile__
	(
"	pop	r30							\n\t"
"	out	__SREG__, r30						\n\t"
"	pop	r30							\n\t"
"	reti								\n\t"
	:
	:
	:	"r30"
	);
}

////////////////////////////////////////////////////////////////////////////////
// Timer setup
////////////////////////////////////////////////////////////////////////////////

#define RTOS_TIMER_FREE_RUNNING	( 0x00 )

void fakertos_timer_init(void)
{
#if 	config_SYSTEMTIMER_PRESCALER == 1
#define CS_bits	1
#elif	config_SYSTEMTIMER_PRESCALER == 8
#define CS_bits	2
#elif	config_SYSTEMTIMER_PRESCALER == 64
#define CS_bits	3
#elif	config_SYSTEMTIMER_PRESCALER == 256
#define CS_bits	4
#elif	config_SYSTEMTIMER_PRESCALER == 1024
#define CS_bits	5
#else
#error Invalid prescaler for SYSTIMER
#endif
	fakertos_TCCRnB = RTOS_TIMER_FREE_RUNNING | CS_bits;
	msticker_init();
}

////////////////////////////////////////////////////////////////////////////////
// fake task functions.
////////////////////////////////////////////////////////////////////////////////
#ifdef config_FAKERTOS_TASKYIELD_FCT_PROTO
config_FAKERTOS_TASKYIELD_FCT_PROTO
#endif
void task_yield(void)
{
#ifdef config_FAKERTOS_TASKYIELD_FUNCTION
	config_FAKERTOS_TASKYIELD_FUNCTION
#endif
}

#endif // #if (RTOSMODE == RTOSMODE_FAKERTOS)
