
DRIVER_INC_PATH = $(APP_PATH)/driver

#-----------------------------------------------------------------------------

INC_PATH += $(DRIVER_INC_PATH)

#-----------------------------------------------------------------------------

ifneq '' '$(findstring RTC,$(DRIVER_MODULE_CFG))'

	DEFS += -D HAS_DRIVER_RTC0=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
	ifneq '' '$(findstring UNITTEST,$(MCU))'
		CSRCS += $(FRMWRK_PATH)/src/driver/rtc/rtc_driver_unittest.c
	else
	ifneq '' '$(findstring UNIVERSAL,$(MCU))'
		CSRCS += $(FRMWRK_PATH)/src/driver/rtc/rtc_driver_universal.c
	else
	ifneq '' '$(findstring RASPBERRY_PI,$(MCU))'
		LIBS += -l rt
		CSRCS += $(FRMWRK_PATH)/src/driver/rtc/rtc_driver_raspberrypi.c
	else
	ifneq '' '$(findstring atmega1284p,$(MCU))'
		CSRCS += $(FRMWRK_PATH)/src/driver/rtc/rtc_driver_atmega1284p.c
	else
	ifneq '' '$(findstring MACOS,$(MCU))'
		CSRCS += $(FRMWRK_PATH)/src/driver/rtc/rtc_driver_macos.c
	endif
	endif
	endif
	endif
	endif
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring CLK,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_CLK=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/clock/clock_driver_$(MCU_NAME).c
	endif
endif

ifneq '' '$(findstring IRQ,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_IRQ=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/irq/irq_driver_$(MCU_NAME).c
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring GPIO,$(DRIVER_MODULE_CFG))'

	ifneq '' '$(findstring GPIO_NO_INIT_ON_START,$(DRIVER_MODULE_CFG))'

		DEFS += -D GPIO_NO_INIT_ON_START=1

	endif

	ifneq '' '$(findstring GPIO_LINUX,$(DRIVER_MODULE_CFG))'

		DEFS += -D HAS_DRIVER_GPIO=1
		CSRCS += $(APP_PATH)/driver/gpio/gpio_driver_linux/gpio_driver_linux.c

	else
	ifneq '' '$(findstring GPIO_WIRINGPI,$(DRIVER_MODULE_CFG))'

		DEFS += -D HAS_DRIVER_GPIO=1
		CSRCS += $(APP_PATH)/driver/gpio/wiringpi/gpio_driver_wiringpi.c
		LIBS += -l wiringPi

	else
	ifneq '' '$(findstring rp2040,$(MCU))'

		#DEFS += -D HAS_DRIVER_GPIO=1
		#CSRCS += $(APP_PATH)/driver/gpio/arm/gpio_driver_rp2040.c

	else
	ifneq '' '$(findstring GPIO_EMPTY,$(DRIVER_MODULE_CFG))'

		DEFS += -D HAS_DRIVER_GPIO=1
		CSRCS += $(APP_PATH)/driver/gpio/empty/gpio_driver_empty.c

	else
	ifneq '' '$(findstring atmega1284p,$(MCU))'

		DEFS += -D HAS_DRIVER_GPIO=1
		CSRCS += $(APP_PATH)/driver/gpio/avr/gpio_driver_atmega1284p.c

	else

		DEFS += -D HAS_DRIVER_GPIO=1
		CSRCS += $(APP_PATH)/driver/gpio/$(CPU_FAMILY)/gpio_driver_$(MCU_NAME).c

		ifneq '' '$(findstring raspberrypi,$(MCU))'
			LIBS += -l wiringPi
		endif

	endif
	endif
	endif
	endif
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring I2C0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_I2C0=1
	CSRCS += $(APP_PATH)/driver/communication/i2c/i2c0_driver_status.c
	CSRCS += $(APP_PATH)/driver/communication/i2c/i2c0_driver_$(MCU_NAME).c
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring SPI0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_SPI0=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/communication/spi/spi0_driver_$(MCU_NAME).c
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring USART0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART0=1

	ifneq '' '$(findstring RASPBERRY_PI,$(MCU))'
		CSRCS += $(APP_PATH)/driver/communication/usart/usart0_driver_raspberry_pi.c
	else
	ifneq '' '$(findstring rp2040,$(MCU))'
		#CSRCS += $(APP_PATH)/driver/communication/usart/usart_driver_$(MCU_NAME).c
	else
	ifneq '' '$(findstring UNITTEST,$(MCU))'
		CSRCS += $(APP_PATH)/driver/communication/usart/usart0_driver_unittest.c
	else
	ifneq '' '$(findstring MACOS,$(MCU))'
		CSRCS += $(APP_PATH)/driver/communication/usart/usart_driver_macos.c
	else
		CSRCS += $(APP_PATH)/driver/communication/usart/usart0_driver_atmega1284p.c
	endif
	endif
	endif
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring USART1,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART1=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/communication/usart/usart1_driver_atmega1284p.c
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring UNITTEST,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_UNITTEST=1
	CSRCS += $(APP_PATH)/driver/communication/test/communication_driver_test.c
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring TIMER0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_TIMER0=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/timer/timer0_driver_atmega1284p.c
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring TIMER1,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_TIMER0=1

	ifneq '' '$(findstring rp2040,$(MCU))'

	else
		CSRCS += $(APP_PATH)/driver/timer/timer1_driver_atmega1284p.c
	endif
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring LCD_16X2,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_LCD=1
	CSRCS += $(APP_PATH)/driver/lcd/lcd_driver_HD44780_16x2.c
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring KEYPAD_3x4,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_KEYPAD_3x4=1
	CSRCS += $(APP_PATH)/driver/keypad/keypad_driver_3x4.c
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring DEVICE_NAME_SUPPORT,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_DEVICE_NAME_SUPPORT=1
endif

#-----------------------------------------------------------------------------

# Platform specific driver implementation is loaded
# via the DRIVER_PATH
ifdef SOC_PATH
include $(SOC_PATH)/make/make_driver.mk
endif

#-----------------------------------------------------------------------------

ifneq '' '$(findstring THE_PI_HUT_PIR,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_THE_PI_HUT_PIR=1
	CSRCS += $(APP_PATH)/driver/movement_detection/the_pi_hut_pir_sensor.c
endif

#-----------------------------------------------------------------------------
