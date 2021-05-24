

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
include $(FRMWRK_PATH)/make/make_cpu_avr.mk
endif

ifeq ($(MCU), RASPBERRY_PI)
include $(FRMWRK_PATH)/make/make_cpu_raspberrypi.mk
endif

ifeq ($(MCU), UNITTEST)
include $(FRMWRK_PATH)/make/make_cpu_unittest.mk
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
