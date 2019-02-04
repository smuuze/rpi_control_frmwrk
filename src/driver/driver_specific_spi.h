#ifndef _DRIVER_SPECIFIC_SPI
#define _DRIVER_SPECIFIC_SPI

#define DRIVER_SPI_PADDING_BYTE				(0x00)

#define DRIVER_SPI_CFGMASK_MODULE_EN			(1 << SPE)
#define DRIVER_SPI_CFGMASK_IRQ_EN			(1 << SPIE)
#define DRIVER_SPI_CFGMASK_DORDER_LSB_EN		(1 << DORD)
#define DRIVER_SPI_CFG_MASK_MASTER_EN			(1 << MSTR)

#define DRIVER_SPI_CFGMASK_CPOL_EN			(1 << CPOL)
#define DRIVER_SPI_CFGMKASK_CPHA_EN			(1 << CPHA)

#define DRIVER_SPI_CLK_DEVIDE_IGNORED			0
#define DRIVER_SPI_CLK_DEVIDE_BY_4			( (0 << SPR1) | (0 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_16			( (0 << SPR1) | (1 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_64			( (1 << SPR1) | (0 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_128			( (1 << SPR1) | (1 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_2			( (0 << SPR1) | (0 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_8			( (0 << SPR1) | (1 << SPR0) )
#define DRIVER_SPI_CLK_DEVIDE_BY_32			( (1 << SPR1) | (0 << SPR0) )

#define DRIVER_SPI_CFG_MASK_DOUBLE_SPEED_EN		(1 << SPI2X)

#define SPI0_BUILD_CFG()				static u8 spi0_config;

#define SPI0_POWER_UP()					PRR0 &= ~(1 << PRSPI)
#define SPI0_POWER_DOWN()				PRR0 |= (1 << PRSPI)

#define SPI0_CLEAR_CONFIG()				SPCR = 0;							\
							spi0_config = 0
#define SPI0_RESET_REGISTERS()				do { volatile u8 temp = SPSR; temp = SPDR; } while(0)

#define SPI0_ENABLE_MODULE()				SPCR = (DRIVER_SPI_CFGMASK_MODULE_EN | (spi0_config))
#define SPI0_DISABLE_MODULE()				SPCR = 0

#define SPI0_ENABLE_IRQ()				spi0_config |= DRIVER_SPI_CFGMASK_IRQ_EN
#define SPI0_DISABLE_IRQ()				spi0_config &= ~DRIVER_SPI_CFGMASK_IRQ_EN

#define SPI0_ENABLE_DATA_ORDER_LSB()			spi0_config |= DRIVER_SPI_CFGMASK_DORDER_LSB_EN
#define SPI0_DISABLE_DATA_ORDER_LSB()			spi0_config &= ~DRIVER_SPI_CFGMASK_DORDER_LSB_EN

#define SPI0_ENABLE_MASTER_MODE()			spi0_config |= DRIVER_SPI_CFG_MASK_MASTER_EN
#define SPI0_DISABLE_MASTER_MODE()			spi0_config &= ~DRIVER_SPI_CFG_MASK_MASTER_EN

#define SPI0_ENABLE_DOUBLE_SPEED()			spi0_config |= DRIVER_SPI_CFG_MASK_DOUBLE_SPEED_EN
#define SPI0_DISABLE_DOUBLE_SPEED()			spi0_config &= ~DRIVER_SPI_CFG_MASK_DOUBLE_SPEED_EN

#define SPI0_ENABLE_SPI_MODE_0()			spi0_config &= ~(DRIVER_SPI_CFGMASK_CPOL_EN | DRIVER_SPI_CFGMKASK_CPHA_EN)
#define SPI0_ENABLE_SPI_MODE_1()			spi0_config &= ~(DRIVER_SPI_CFGMASK_CPOL_EN); spi0_config |= (DRIVER_SPI_CFGMKASK_CPHA_EN)
#define SPI0_ENABLE_SPI_MODE_2()			spi0_config &= ~(DRIVER_SPI_CFGMKASK_CPHA_EN); spi0_config |= (DRIVER_SPI_CFGMASK_CPOL_EN)
#define SPI0_ENABLE_SPI_MODE_3()			spi0_config |= (DRIVER_SPI_CFGMASK_CPOL_EN | DRIVER_SPI_CFGMKASK_CPHA_EN)

#define SPI0_ENABLE_CLOCK_DEVIDER(devider)		spi0_config |= ((devider) & ((1 << SPR1) | (1 << SPR0)))

#define SPI0_GET_CONFIG()				SPCR
#define SPI0_GET_STATUS()				SPSR
#define SPI0_GET_BYTE()					SPDR
#define SPI0_SET_BYTE(u8_byte)				SPDR = u8_byte

#define SPI0_IS_TRX_COMPLETE()				((SPSR & (1<<SPIF)) != 0 ? 1 : 0)
#define SPI0_IS_IRQ_ENABLED()				((SPCR & DRIVER_SPI_CFGMASK_IRQ_EN) != 0 ? 1 : 0)

#define SPI0_WAIT_FOR_TRX()				while (SPI0_IS_TRX_COMPLETE() == 0)
#define SPI0_WAIT_FOR_HW()				while (SPI0_IS_IRQ_ENABLED() == 1)

#define SPI0_INIT_INTERFACE()				IO_CONTROLLER_BUILD_INOUT(name, pin_descr)

#define SPI0_ENABLE_INTERFACE()				SPI0_CE_as_INPUT(); SPI0_CE_ON();		\
							SPI0_MOSI_as_INPUT(); SPI0_MOSI_ON();		\
							SPI0_MISO_as_OUTPUT(); SPI0_MISO_ON();		\
							SPI0_SCK_as_INPUT(); SPI0_SCK_ON();

#define SPI0_DISABLE_INTERFACE()			SPI0_CE_as_INPUT(); SPI0_CE_OFF();		\
							SPI0_MOSI_as_INPUT(); SPI0_MOSI_OFF();		\
							SPI0_MISO_as_INPUT(); SPI0_MISO_OFF();		\
							SPI0_SCK_as_INPUT(); SPI0_SCK_OFF();

#endif // _DRIVER_SPECIFIC_SPI
