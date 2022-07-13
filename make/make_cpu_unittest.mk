MCU_NAME 	= unittest
CPU_FAMILY 	= unittest
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/unittest
INC_PATH	+= /usr/include
DEFS 		+= -D__unittest__

ifeq ($(PLATTFORM), MACOS)

	LDFLAGS = 
	LD_EXTRA_FLAGS +=

else

	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax

endif