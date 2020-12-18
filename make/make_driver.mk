
DRIVER_INC_PATH = $(APP_PATH)/driver

INC_PATH += $(DRIVER_INC_PATH)

ifneq '' '$(findstring RTC,$(DRIVER_MODULE_CFG))'

	DEFS += -D HAS_DRIVER_RTC0=1
	CSRCS += $(APP_PATH)/driver/rtc/rtc_driver_$(MCU_NAME).c

	ifneq '' '$(findstring raspberrypi,$(MCU_NAME))'
		LIBS += -l rt
	endif
endif

ifneq '' '$(findstring CLK,$(DRIVER_MODULE_CFG))'
	CSRCS += $(APP_PATH)/driver/clock/clock_driver_$(MCU_NAME).c
endif

ifneq '' '$(findstring GPIO,$(DRIVER_MODULE_CFG))'

	DEFS += -D HAS_DRIVER_GPIO=1
	CSRCS += $(APP_PATH)/driver/gpio/$(CPU_FAMILY)/gpio_driver_$(MCU_NAME).c

	ifneq '' '$(findstring raspberrypi,$(MCU_NAME))'
		LIBS += -l wiringPi
	endif
endif

ifneq '' '$(findstring I2C0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_I2C0=1
	CSRCS += $(APP_PATH)/driver/communication/i2c/i2c0_driver_status.c
	CSRCS += $(APP_PATH)/driver/communication/i2c/i2c0_driver_$(MCU_NAME).c
endif

ifneq '' '$(findstring SPI0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_SPI0=1
	CSRCS += $(APP_PATH)/driver/communication/spi/spi0_driver_$(MCU_NAME).c
endif

ifneq '' '$(findstring USART0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART0=1
	CSRCS += $(APP_PATH)/driver/communication/usart/usart0_driver_atmega1284p.c
endif

ifneq '' '$(findstring USART1,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART1=1
	CSRCS += $(APP_PATH)/driver/communication/usart/usart1_driver_atmega1284p.c
endif

ifneq '' '$(findstring UNITTEST,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_UNITTEST=1
	CSRCS += $(APP_PATH)/driver/communication/test/communication_driver_test.c
endif

ifneq '' '$(findstring TIMER0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_TIMER0=1
	CSRCS += $(APP_PATH)/driver/timer/timer0_driver_atmega1284p.c
endif

ifneq '' '$(findstring TIMER1,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_TIMER0=1
	CSRCS += $(APP_PATH)/driver/timer/timer1_driver_atmega1284p.c
endif