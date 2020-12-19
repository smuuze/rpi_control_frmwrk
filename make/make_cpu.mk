

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
MCU_FLAG ?=
CROSS_COMPILER_PREFIX ?= 
LD_EXTRA_FLAGS ?=

MCU_SIZE_FLAGS ?=

ifeq ($(MCU), ATMEGA1284P)
	MCU_NAME = atmega1284p
	CPU_FAMILY = avr
	DEFS += -D__AVR_ATmega1284P__

	INC_PATH += ../../rpi_control_frmwrk/src/common/cpu/avr
	INC_PATH += $(AVR_INCLUDE_PATH)

	MCU_FLAG = -mmcu=$(MCU_NAME)
	CROSS_COMPILER_PREFIX = avr-

	MCU_SIZE_FLAGS = --mcu=$(MCU_NAME)

	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax
endif

ifeq ($(MCU), RASPBERRY_PI)
	MCU_NAME 	= raspberrypi
	CPU_FAMILY 	= arm
	INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/rpi_arm
	INC_PATH	+= /usr/include
	
	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax
endif

ifeq ($(MCU), UNITTEST)
	MCU_NAME 	= unittest
	CPU_FAMILY 	= unittest
	INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/unittest
	INC_PATH	+= /usr/include
	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax
endif

SECTIONS =
SECTIONS += -j .text
SECTIONS += -j .data
#SECTIONS += -R.blibl
#SECTIONS += -R.signature
#SECTIONS += -R.lock
#SECTIONS += -R.eeprom
#SECTIONS += -R.iblchif
#SECTIONS += -R.fuse     

CSLAGS += -g

LDFLAGS += $(LD_EXTRA_FLAGS)

#MCU_TARGET     = at90s2313
#MCU_TARGET     = at90s2333
#MCU_TARGET     = at90s4414
#MCU_TARGET     = at90s4433
#MCU_TARGET     = at90s4434
#MCU_TARGET     = at90s8515
#MCU_TARGET     = at90s8535
#MCU_TARGET     = atmega128
#MCU_TARGET     = atmega1280
#MCU_TARGET     = atmega1281
#MCU_TARGET     = atmega1284p
#MCU_TARGET     = atmega16
#MCU_TARGET     = atmega163
#MCU_TARGET     = atmega164p
#MCU_TARGET     = atmega165
#MCU_TARGET     = atmega165p
#MCU_TARGET     = atmega168
#MCU_TARGET     = atmega169
#MCU_TARGET     = atmega169p
#MCU_TARGET     = atmega2560
#MCU_TARGET     = atmega2561
#MCU_TARGET     = atmega32
#MCU_TARGET     = atmega324p
#MCU_TARGET     = atmega325
#MCU_TARGET     = atmega3250
#MCU_TARGET     = atmega329
#MCU_TARGET     = atmega3290
#MCU_TARGET     = atmega32u4
#MCU_TARGET     = atmega48
#MCU_TARGET     = atmega64
#MCU_TARGET     = atmega640
#MCU_TARGET     = atmega644
#MCU_TARGET     = atmega644p
#MCU_TARGET     = atmega645
#MCU_TARGET     = atmega6450
#MCU_TARGET     = atmega649
#MCU_TARGET     = atmega6490
#MCU_TARGET     = atmega8
#MCU_TARGET     = atmega8515
#MCU_TARGET     = atmega8535
#MCU_TARGET     = atmega88
#MCU_TARGET     = attiny2313
#MCU_TARGET     = attiny24
#MCU_TARGET     = attiny25
#MCU_TARGET     = attiny26
#MCU_TARGET     = attiny261
#MCU_TARGET     = attiny44
#MCU_TARGET     = attiny45
#MCU_TARGET     = attiny461
#MCU_TARGET     = attiny84
#MCU_TARGET     = attiny85
#MCU_TARGET     = attiny861