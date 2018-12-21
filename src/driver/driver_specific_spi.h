#ifndef _DRIVER_SPECIFIC_SPI
#define _DRIVER_SPECIFIC_SPI

#define DRIVER_SPI_PADDING_BYTE				(0x00)

#define DRIVER_SPI_ENABLED				(1 << 6)
#define DRIVER_SPI_DISABLED				(0)

#define DRIVER_SPI_INTERRUPT_ENABLED			(1 << 7)
#define DRIVER_SPI_INTERRUPT_DISABLED			(0)

#define DRIVER_SPI_OP_MODE_MASTER			(1 << 4)
#define DRIVER_SPI_OP_MODE_SLAVE			(0)

#define DRIVER_SPI_CLK_POLARITY_IDLE_LOW		(0)
#define DRIVER_SPI_CLK_POLARITY_IDLE_HIGH		(1 << 3)

#define DRIVER_SPI_CLK_PHASE_LEADING			(0)
#define DRIVER_SPI_CLK_PHASE_TRAILING			(1 << 2)

#define DRIVER_SPI_CLK_DEVIDE_IGNORED			0
#define DRIVER_SPI_CLK_DEVIDE_BY_4			( (0 << 1) | (0 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_16			( (0 << 1) | (1 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_64			( (1 << 1) | (0 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_128			( (1 << 1) | (1 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_2			( (0 << 1) | (0 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_8			( (0 << 1) | (1 << 0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_32			( (1 << 1) | (0 << 0) )

#define DRIVER_SPI_CLK_DOUBLE_SPEED			(1 << 0)
#define DRIVER_SPI_CLK_NO_DOUBLE_SPEED			(0)

#define DRIVER_SPI_DATA_ORDER_LSB			( 1 << 5 )
#define DRIVER_SPI_DATA_ORDER_MSB			( 0 )

#define SPI0_CLEAR_CONFIG()				SPCR = 0;

#define SPI0_SET_INTERRUPT(value)			SPCR |= value
#define SPI0_SET_ENABLED(value)				SPCR |= value
#define SPI0_SET_DATA_ORDER(value)			SPCR |= value
#define SPI0_SET_MODE(value)				if (value == DRIVER_SPI_OP_MODE_SLAVE) {	\
								SPI0_CE_as_INPUT();			\
								SPI0_MOSI_as_INPUT();			\
								SPI0_MISO_as_OUTPUT();			\
								SPI0_SCK_as_INPUT();			\
							} else {					\
								SPI0_CE_as_OUTPUT();			\
								SPI0_MOSI_as_OUTPUT();			\
								SPI0_MISO_as_INPUT();			\
								SPI0_SCK_as_OUTPUT();			\
								SPCR |= value;				\
							}
#define SPI0_SET_CLOCK_POLARITY(value)			SPCR |= value
#define SPI0_SET_CLOCK_PHASE(value)			SPCR |= value
#define SPI0_SET_CLOCK_RATE(value)			SPCR |= value

#define SPI0_GET_BYTE()					SPDR
#define SPI0_SET_BYTE(u8_byte)				SPDR = u8_byte

#define SPI0_IS_TRX_COMPLETE()				(SPSR & (1<<SPIF)) != 0 ? 1 : 0

#define SPI0_ENABLE_INTERFACE()				SPI0_CE_as_INPUT(); SPI0_CE_ON();		\
							SPI0_MOSI_as_INPUT(); SPI0_MOSI_ON();		\
							SPI0_MISO_as_OUTPUT(); SPI0_MISO_ON();		\
							SPI0_SCK_as_INPUT(); SPI0_SCK_ON();

#define SPI0_DISABLE_INTERFACE()			SPI0_CE_as_INPUT(); SPI0_CE_OFF();		\
							SPI0_MOSI_as_INPUT(); SPI0_MOSI_OFF();		\
							SPI0_MISO_as_INPUT(); SPI0_MISO_OFF();		\
							SPI0_SCK_as_INPUT(); SPI0_SCK_OFF();

#endif // _DRIVER_SPECIFIC_SPI
