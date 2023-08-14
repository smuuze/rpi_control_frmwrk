
#-----------------------------------------------------------------------------

CROSS_COMPILER_PREFIX ?=
CROSS_COMPILER_SUFFIX ?=

#-----------------------------------------------------------------------------

LDFLAGS ?= 
LD_EXTRA_FLAGS ?=
CFLAGS ?= -Wextra -Wall
LFLAGS ?=

#-----------------------------------------------------------------------------

ifdef MEMORY_LAYOUT_FILE
	LFLAGS += -T $(MEMORY_LAYOUT_FILE)
endif

#-----------------------------------------------------------------------------

ifeq ($(CPU_FAMILY), avr)

	ifeq ($(OS),Windows_NT)

		CROSS_COMPILER_PATH   = ../../../../bin/avr/winavr/20100110/bin
		AVR_INCLUDE_PATH = $(BASE_PATH)/bin/arv/winavr/20100110/avr/include

	else 

		UNAME_S := $(shell uname -s)
		CROSS_COMPILER_PREFIX = avr-

		ifeq ($(UNAME_S),Linux)
			CROSS_COMPILER_PATH = /usr/bin
			INC_PATH += /usr/lib/avr/include
		endif

		ifeq ($(UNAME_S),Darwin)
			CFLAGS += -D OSX
			CROSS_COMPILER_PATH = /usr/local/bin
		endif

	endif

endif

#-----------------------------------------------------------------------------

ifeq ($(CPU_FAMILY), arm)

	ifeq ($(OS),Windows_NT)

	else 

		UNAME_S := $(shell uname -s)
		CROSS_COMPILER_PREFIX = arm-none-eabi-

		ifeq ($(UNAME_S),Linux)
			CROSS_COMPILER_PATH = /usr/bin

			ifeq ($(MCU),RASPBERRY_PI)
				CROSS_COMPILER_PREFIX = arm-linux-gnueabihf-
			endif
		endif

		ifeq ($(UNAME_S),Darwin)
			CFLAGS += -D OSX
			CROSS_COMPILER_PATH = /usr/local/bin
		endif

	endif

endif

#-----------------------------------------------------------------------------

ifeq ($(CPU_FAMILY), unittest)

	ifeq ($(OS),Windows_NT)

	else 

		UNAME_S := $(shell uname -s)
		CROSS_COMPILER_PREFIX =

		ifeq ($(UNAME_S),Linux)
			CROSS_COMPILER_PATH = /usr/bin
		endif

		ifeq ($(UNAME_S),Darwin)
			CFLAGS += -D OSX
			CROSS_COMPILER_PATH = /usr/bin
		endif

	endif

endif

#-----------------------------------------------------------------------------

ifeq ($(CPU_FAMILY), universal)

	ifeq ($(OS),Windows_NT)

	else 

		UNAME_S := $(shell uname -s)
		CROSS_COMPILER_PREFIX =

		ifeq ($(UNAME_S),Linux)
			CROSS_COMPILER_PATH = /usr/bin
		endif

		ifeq ($(UNAME_S),Darwin)
			CFLAGS += -D OSX
			CROSS_COMPILER_PATH = /usr/bin
		endif

	endif

endif

#-----------------------------------------------------------------------------

AS      = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)as$(CROSS_COMPILER_SUFFIX)
CC      = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)gcc$(CROSS_COMPILER_SUFFIX)
CC_SIZE = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)size$(CROSS_COMPILER_SUFFIX)
CC_OBJ  = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)objcopy$(CROSS_COMPILER_SUFFIX)
CC_LD   = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)ld$(CROSS_COMPILER_SUFFIX)

#-----------------------------------------------------------------------------

PLATTFORM ?=

ifeq ($(PLATTFORM), MACOS)

	PLATTFORM_EXTENSION = app

else

	PLATTFORM_EXTENSION = elf

	LDFLAGS = -Wl,-Map,$(OBJECT_DIRECTORY)/$(TARGET).map
	LD_EXTRA_FLAGS += -Wl,--gc-sections,--relax

	CC_DUMP = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)objdump$(CROSS_COMPILER_SUFFIX)
	CC_COPY = $(CROSS_COMPILER_PATH)/$(CROSS_COMPILER_PREFIX)objcopy$(CROSS_COMPILER_SUFFIX)
endif

#-----------------------------------------------------------------------------

RM						:= rm
MK						:= mkdir -p
CP						:= cp
MAKE_EXE				:= chmod ug=+rwx
MAKE_FILE_RIGHTS		:= chmod ug=+rw
MAKE_DIR_RIGHTS			:= chmod ug=rwx
MAKE_SERVICE_RIGHTS		:= chmod 644
ECHO					:= printf '%s\n'
ECHO_NO_N				:= printf '%s'

MAKE_OWNER				:= chown shc:shc

MAKE_SERVVICE_RELOAD	:= systemctl daemon-reload
MAKE_SERVICE_ENABLE		:= systemctl enable
MAKE_SERVICE_START		:= systemctl start
MAKE_SERVICE_STOP		:= systemctl stop
MAKE_SERVICE_DISABLE	:= systemctl disable

RM_FLAGS				:= -rf

#-----------------------------------------------------------------------------

VERBOSE = @

#-----------------------------------------------------------------------------

GREP ?= | grep
AWK ?= | awk

#-----------------------------------------------------------------------------

ELF2UF = elf2uf2
