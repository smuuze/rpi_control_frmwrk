
MCU_NAME ?=

ifeq ($(MCU_NAME), ATMEGA1284P)
	MCU = -mmcu=atmega1284p
	DEFS += -D__AVR_ATmega1284P__
endif

AVR_INCLUDE_PATH = $(BASE_PATH)/bin/arv/winavr/20100110/avr/include
INC_PATH += $(AVR_INCLUDE_PATH)