#-----------------------------------------------------------------------------
#       Makefile for Driver-Modules
#-----------------------------------------------------------------------------

DRIVER_INC_PATH = $(APP_PATH)/driver
INC_PATH += $(DRIVER_INC_PATH)
CSRCS += $(APP_PATH)/driver/local_rtc_driver.c
CSRCS += $(APP_PATH)/driver/local_clk_driver.c
CSRCS += $(APP_PATH)/driver/local_gpio_driver.c

DEFS += -D HAS_DRIVER_I2C0=1
CSRCS += $(APP_PATH)/driver/local_i2c_driver.c

ifneq '' '$(findstring SPI0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_SPI0=1
	CSRCS += $(APP_PATH)/driver/local_spi_driver.c
endif

ifneq '' '$(findstring USART0,$(DRIVER_MODULE_CFG))'
	DEFS += -D HAS_DRIVER_USART0=1
	CSRCS += $(APP_PATH)/driver/local_usart_driver.c
endif