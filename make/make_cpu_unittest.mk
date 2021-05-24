MCU_NAME 	= unittest
CPU_FAMILY 	= unittest
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/unittest
INC_PATH	+= /usr/include

ifeq ($(PLATTFORM), MACOS)

	LDFLAGS = -Wl
	LD_EXTRA_FLAGS += -Wl

else

	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax

endif