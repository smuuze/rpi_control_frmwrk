
include $(FRMWRK_PATH)/make/make_cpu_arm.mk

MCU_NAME = raspberrypi

DEFS += -D__raspberrypi__

INC_PATH += /usr/include

CFLAGS += -pthread
