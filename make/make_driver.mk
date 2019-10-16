
DRIVER_INC_PATH = $(APP_PATH)/driver
INC_PATH += $(DRIVER_INC_PATH)

ifdef DRIVER_MODULE_CFG

CSRCS += $(APP_PATH)/driver/local_rtc_driver.c
CSRCS += $(APP_PATH)/driver/local_clk_driver.c
CSRCS += $(APP_PATH)/driver/gpio/$(CPU_FAMILY)/gpio_driver_$(MCU_NAME).c

DEFS += -D HAS_DRIVER_I2C0=1
CSRCS += $(APP_PATH)/driver/local_i2c_driver.c

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

endif