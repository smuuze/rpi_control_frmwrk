MCU_NAME 	= raspberrypi
CPU_FAMILY 	= arm
DEFS 		+= -D__raspberrypi__
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/rpi_arm
INC_PATH	+= /usr/include
	
LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax