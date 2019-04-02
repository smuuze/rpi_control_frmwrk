

ifeq ($(OS),Windows_NT)

	AVR_INCLUDE_PATH = $(BASE_PATH)/bin/arv/winavr/20100110/avr/include
    
else

	UNAME_S := $(shell uname -s)
    
	ifeq ($(UNAME_S),Linux)

		CROSS_COMPILER_PATH   = /usr/bin
		AVR_INCLUDE_PATH = /usr/lib/avr/include

	else

		ifeq ($(UNAME_S),Darwin)
			CCFLAGS += -D OSX
		endif
	endif
    
endif


MCU ?=

ifeq ($(MCU), ATMEGA1284P)
	MCU_NAME = atmega1284p
	DEFS += -D__AVR_ATmega1284P__
endif

MCU_FLAG = -mmcu=$(MCU_NAME)

CROSS_COMPILER_PREFIX = avr-
OPTIMIZATION = -Os

SECTIONS =
SECTIONS += -j .text
SECTIONS += -j .data

INC_PATH += $(AVR_INCLUDE_PATH)