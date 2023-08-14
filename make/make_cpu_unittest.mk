MCU_NAME 	= unittest
CPU_FAMILY 	= unittest
INC_PATH 	+= $(FRMWRK_PATH)/src/common/cpu/unittest
INC_PATH 	+= $(FRMWRK_PATH)/module_test
INC_PATH	+= /usr/include
INC_PATH	+= $(FRMWRK_PATH)/unittest
DEFS 		+= -D__unittest__

#CFLAGS += -pthread
