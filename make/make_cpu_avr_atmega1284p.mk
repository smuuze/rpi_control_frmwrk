
include $(FRMWRK_PATH)/make/make_cpu_avr.mk

MCU_NAME = atmega1284p
DEFS += -D__AVR_ATmega1284P__

MCU_SIZE_FLAGS = --mcu=$(MCU_NAME)
MCU_FLAG = -mmcu=$(MCU_NAME)
