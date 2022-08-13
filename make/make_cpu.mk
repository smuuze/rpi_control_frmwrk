
#-----------------------------------------------------------------------------

CFLAGS += -g

#-----------------------------------------------------------------------------

ifeq ($(MCU), ATMEGA1284P)
include $(FRMWRK_PATH)/make/make_cpu_avr_atmega1284p.mk
endif

ifeq ($(MCU), RASPBERRY_PI)
include $(FRMWRK_PATH)/make/make_cpu_raspberrypi.mk
endif

ifeq ($(MCU), UNITTEST)
include $(FRMWRK_PATH)/make/make_cpu_unittest.mk
endif

ifeq ($(MCU), RP2040)
include $(FRMWRK_PATH)/make/make_cpu_rp2040.mk
endif

#-----------------------------------------------------------------------------

INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/$(CPU_FAMILY)/$(MCU_NAME)

#-----------------------------------------------------------------------------

SECTIONS =
SECTIONS += -j .text
SECTIONS += -j .data
#SECTIONS += -R.blibl
#SECTIONS += -R.signature
#SECTIONS += -R.lock
#SECTIONS += -R.eeprom
#SECTIONS += -R.iblchif
#SECTIONS += -R.fuse

#-----------------------------------------------------------------------------

LDFLAGS += $(LD_EXTRA_FLAGS)

#-----------------------------------------------------------------------------
