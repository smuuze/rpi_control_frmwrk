//#include RTOS_H (define RTOS_H in config.h!)
#ifndef __SERVICE_125KHZ_AVR_FAKERTOS_H__
#define __SERVICE_125KHZ_AVR_FAKERTOS_H__

#include "config.h"
#include "utils/stdmacros.h"


#ifndef config_CPU_CLK_PRESCALER
#ifdef CPU_PRESCALER
  #define config_CPU_CLK_PRESCALER (CPU_PRESCALER)
#else
  #define config_CPU_CLK_PRESCALER 1
#endif
#endif // #ifndef config_CPU_CLK_PRESCALER

#ifndef config_CPU_CLK_HZ
#define config_CPU_CLK_HZ ((CPU_XTAL_HZ)/(config_CPU_CLK_PRESCALER))
#endif


// Minimalwert fuer Timerueberlauf : 32768us (d.h. TCLK <= 2MHz)
// Maximalwert fuer Timerstep : 4us (d.h. TCLK >= 250kHz)
#ifndef config_SYSTEMTIMER_PRESCALER
#if   ( config_CPU_CLK_HZ <= 2000000 )
  #define config_SYSTEMTIMER_PRESCALER 1
#elif ( config_CPU_CLK_HZ <= 16000000 )
  #define config_SYSTEMTIMER_PRESCALER 8
#else
  #define config_SYSTEMTIMER_PRESCALER 64
#endif
#endif // #ifndef config_SYSTEMTIMER_PRESCALER


////////////////////////////////////////////////////////////////////////////////
// Simple systimer interface
////////////////////////////////////////////////////////////////////////////////
#define systimer_type u16

void fakertos_timer_init(void);
ux8 msticker_get_time_ux8(void);
u32 msticker_get_time_ux32(void);
u16 msticker_get_time_ux16(void);
ux8 mstimer_time_is_up_ux8(const ux8 reference_time, const ux8 time_interval);
ux8 mstimer_time_is_up_ux16(const ux16 reference_time, const ux16 time_interval);
ux8 mstimer_time_is_up_ux32(const ux32 reference_time, const ux32 time_interval);
ux8 systimer_time_is_up(const systimer_type reference_time,
			const systimer_type time_interval);
u8 mstimer_time_since_ux8(const ux8 reference_time);
u16 mstimer_time_since_ux16(const ux16 reference_time);
u32 mstimer_time_since_ux32(const ux32 reference_time);
void task_yield(void);
void fakertos_sleep( uint16_t milli_seconds );

//Privat:
#define fakertos_TCCRnB		combine3(TCCR,RTOS_TIMER,B)
#define fakertos_TCNTn		combine(TCNT,RTOS_TIMER)
#define fakertos_TCNTnL		combine3(TCNT,RTOS_TIMER,L)
#define fakertos_TCNTnH		combine3(TCNT,RTOS_TIMER,H)

#define _systimer_get_time() __extension__				\
({									\
	systimer_type t;						\
									\
	__asm__ __volatile__						\
	(								\
REGISTER_USED(24)							\
REGISTER_USED(25)							\
"	cli								\n\t"\
"	lds	%A0, %[TCNTnL]						\n\t"\
"	sei								\n\t"\
"	lds	%B0, %[TCNTnH]						\n\t"\
	:		"=r" (t)					\
	:	[TCNTnL] "p" (_SFR_MEM_ADDR(fakertos_TCNTnL)),		\
		[TCNTnH] "p" (_SFR_MEM_ADDR(fakertos_TCNTnH))		\
	);								\
	t;								\
})

//Public
static inline systimer_type systimer_get_time(void)
{
	return _systimer_get_time();
}

////////////////////////////////////////////////////////////////////////////////
// Convert time in ms, µs and ns to ticks
////////////////////////////////////////////////////////////////////////////////
// Example usage:
//	Prozessor / Peripherals are running at 8MHz
//	Timer clock is derived from this by a Divider of 2
//	#define MS2MYTICKS(time_in_ms)	MS2TICKS(time_in_ms, 8000000, 2)
//	#define US2MYTICKS(time_in_us)	US2TICKS(time_in_us, 8000000, 2)
//	#define NS2MYTICKS(time_in_ns)	NS2TICKS(time_in_ns, 8000000, 2)
// Result is the number of ticks closest to the given time as uint64_t.

//Milliseconds to ticks
#define MS2TICKS(time_in_ms, TICKER_CLK_HZ, TICKER_PRESCALER)	\
	TIME2TICKS(time_in_ms, 1000, TICKER_CLK_HZ, TICKER_PRESCALER)
//Microseconds to ticks
#define US2TICKS(time_in_us, TICKER_CLK_HZ, TICKER_PRESCALER)	\
	TIME2TICKS(time_in_us, 1000000, TICKER_CLK_HZ, TICKER_PRESCALER)
//Nanoseconds to ticks
#define NS2TICKS(time_in_ns, TICKER_CLK_HZ, TICKER_PRESCALER)	\
	TIME2TICKS(time_in_ns, 1000000000, TICKER_CLK_HZ, TICKER_PRESCALER)

//Helper macros: Do not use directly:
#define __TIME2TICKS_HELPER(PRESCALER, t_PER_SECOND)				\
	((uint64_t) (PRESCALER) * (t_PER_SECOND))
#define TIME2TICKS(time, time_PER_SECOND, TICKER_CLK_HZ, TICKER_PRESCALER)	\
	((uint64_t)								\
		((uint64_t)							\
			((uint64_t) (TICKER_CLK_HZ)				\
				* (uint64_t) (time)				\
				+ ((uint64_t) __TIME2TICKS_HELPER((TICKER_PRESCALER), (time_PER_SECOND)) / 2)	\
			)							\
			/ (uint64_t) (__TIME2TICKS_HELPER((TICKER_PRESCALER), (time_PER_SECOND)))\
		)								\
	)
////////////////////////////////////////////////////////////////////////////////

//Milliseconds to SYSTIMER ticks.
#define MS2SYSTIMER(time_in_ms)	MS2TICKS(time_in_ms, config_CPU_CLK_HZ, config_SYSTEMTIMER_PRESCALER)
//Microseconds to SYSTIMER ticks.
#define US2SYSTIMER(time_in_us)	US2TICKS(time_in_us, config_CPU_CLK_HZ, config_SYSTEMTIMER_PRESCALER)
//Nanoseconds to SYSTIMER ticks.
#define NS2SYSTIMER(time_in_ns)	NS2TICKS(time_in_ns, config_CPU_CLK_HZ, config_SYSTEMTIMER_PRESCALER)


//SYSTIMER ticks to microseconds.
#define SYSTIMER2US(x)							\
	((u64)								\
		((u64) 1000000						\
		* (u64) ((u64) x) * (u64) (config_SYSTEMTIMER_PRESCALER)\
		+ ((u64) (config_CPU_CLK_HZ)/2))			\
		/ ((u64) (config_CPU_CLK_HZ))				\
	)

//CPU clocks to SYSTIMER ticks.
#define CLOCKS2SYSTIMER(x)						\
	((u64)								\
		((u64) (((u64) (x))					\
			+ ((u64) ((config_SYSTEMTIMER_PRESCALER) / 2))	\
			)						\
			/ ((u64) (config_SYSTEMTIMER_PRESCALER))	\
		)							\
	)								\

#endif /* __SERVICE_125KHZ_AVR_FAKERTOS_H__ */
