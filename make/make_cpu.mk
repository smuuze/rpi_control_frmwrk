
#-----------------------------------------------------------------------------

CFLAGS += -g

#-----------------------------------------------------------------------------

ifeq ($(MCU), atmega1284p)
include $(FRMWRK_PATH)/make/make_cpu_avr_atmega1284p.mk
endif

ifeq ($(MCU), RASPBERRY_PI)
include $(FRMWRK_PATH)/make/make_cpu_raspberrypi.mk
endif

ifeq ($(MCU), UNITTEST)
include $(FRMWRK_PATH)/make/make_cpu_unittest.mk
endif

ifeq ($(MCU), UNIVERSAL)
include $(FRMWRK_PATH)/make/make_cpu_universal.mk
endif

#-----------------------------------------------------------------------------

# Platform specific driver implementation is loaded
# via the SOC_PATH

#-----------------------------------------------------------------------------

ifdef SOC_PATH
include $(SOC_PATH)/make/make_cpu.mk
else
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/$(CPU_FAMILY)/$(MCU_NAME)
endif

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
