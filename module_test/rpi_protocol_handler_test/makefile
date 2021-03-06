#-----------------------------------------------------------------------------
#       Makefile fuer AVR-GCC Projekte
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# PROJECT: Name des Hexfiles usw.
#-----------------------------------------------------------------------------
# VERSION_STRING muss (!) zwei Ziffern nach dem '.' enthalten, weil
# aus dem String noch weitere Varianten von Version erzeugt
# werden. Beispiel:
# VERSION_STRING = v1.07
# VERSION_DIGITS = 107
# VERSION        = 0x107

# PROJEKT Name des Hexfiles
PROJECT = Unittest_RPI_PROTOCOL_HANDLER

# HW_DEVICE_TYPE ( siehe http://ew.deister-gmbh.de/cgi-bin/ibase/gertyp.cgi )
HW_DEVICE_TYPE = 0x22B2

# SW_DEVICE_TYPE wird im deBus Versionstelegramm und fuer Adressumstellung verwendet
SW_DEVICE_TYPE = 0x22B2

BOARD_ID = 180920

#-----------------------------------------------------------------------------

VERSION_MAJOR		:= 2
VERSION_MINOR		:= 1

#-----------------------------------------------------------------------------

BASE_PATH   = ../../../../..
APP_PATH    = ../../src
MAKE_PATH   = ../../make

#-----------------------------------------------------------------------------

## FILE TO TEST
CSRCS += $(APP_PATH)/protocol_management/rpi_protocol_handler.c

## TESTBENCH
CSRCS += testbench_rpi_protocol_handler.c

## TESTCASES
CSRCS += testcase_command_answer_success.c

## MODULES
CSRCS += $(APP_PATH)/platine/board_180920.c
CSRCS += $(APP_PATH)/time_management/rpi/time_management.c
CSRCS += $(APP_PATH)/common/local_msg_buffer.c
CSRCS += $(APP_PATH)/common/signal_slot_interface.c
CSRCS += $(APP_PATH)/common/local_mutex.c
CSRCS += $(APP_PATH)/common/local_module_status.c

## STUBS
CSRCS += $(APP_PATH)/driver/gpio/test/gpio_driver_test_atmega1284p.c
CSRCS += $(APP_PATH)/driver/communication/test/communication_driver_test.c

#-----------------------------------------------------------------------------

TRACER_CFG = CONSOLE
include $(MAKE_PATH)/make_tracer.mk


# --------- Toolchain Path

CPU_FAMILY		:= rpi_arm

CROSS_PREFIX		:= 
TOOLCHAIN_BIN_PATH	:= /usr/bin
TOOLCHAIN_INC_PATH	:= /usr/include
TOOLCHAIN_LIB_PATH	:=

CC			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)gcc'
AS			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)as'
AR			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)ar' -r
LD			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)ld'
NM          		:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)nm'
CC_DUMP			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)objdump'
CC_COPY			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)objcopy'
CC_SIZE			:= '$(TOOLCHAIN_BIN_PATH)/$(CROSS_PREFIX)size'

RM			:= rm
MK			:= mkdir -p
CP			:= cp
MAKE_EXE		:= chmod ug=+rwx
MAKE_FILE_RIGHTS	:= chmod ug=rw
ECHO			:= echo

RM_FLAGS		:= -rf
VERBOSE 		:= @

# --------- Message Output

MSG_COMPILING		:= Compiling
MSG_LINKING		:= Linking to
MSG_PROG_LOCATION	:= Your programm can be found at
MSG_LISTING		:= - Generating Disassembly
MSG_FINISH		:= --------------- Make done ---------------


# --------- Application Properties (Target / Working dir / ...)

VERSION_MAJOR		:= 2
VERSION_MINOR		:= 5
VERSION			:= $(VERSION_MAJOR).$(VERSION_MINOR)

OBJECT_DIRECTORY	:= obj
OUTPUT_DIRECTORY	:= program
CONFIG_DIRECTORY	:= cfg
DEBUG_DIRECTORY		:= $(OBJECT_DIRECTORY)/debug
DEPENDENCY_DIRECTORY	:= $(OBJECT_DIRECTORY)/dependency
RELEASE_DIRECTORY	:= release/v$(VERSION)
FORMAT			:= ihex

TARGET			:= $(PROJECT)
TARGET_DAEMON		:= shcd
TARGET_SERVICE		:= shc_service


# --------- Include Path

INC_PATH =
INC_PATH += .
INC_PATH += $(APP_PATH)
INC_PATH += $(TOOLCHAIN_INC_PATH)
INC_PATH += $(APP_PATH)/tracer
INC_PATH += $(APP_PATH)/common/cpu/$(CPU_FAMILY)

# --------- Defines

DEFS =
DEFS += -DVERSION_MAJOR=$(VERSION_MAJOR)
DEFS += -DVERSION_MINOR=$(VERSION_MINOR)
DEFS += -D VERSION=0x$(VERSION_MAJOR)

DEBUG_ENABLED = -DTRACER_ENABLED

# --------- Compiler Flags

CFLAGS +=

CFLAGS += -Wall
CFLAGS += -Os
#CFLAGS += -funsigned-char
#CFLAGS += -funsigned-bitfields
#CFLAGS += -fpack-struct
#CFLAGS += -fshort-enums
#CFLAGS += -lm

LDFLAGS +=
#LDFLAGS += --section-start init=0
LDFLAGS += -Wl,-Map=$(OBJECT_DIRECTORY)/$(TARGET).map,--cref
LDFLAGS += -L$(TOOLCHAIN_LIB_PATH)

# --------- 

OPTIMIZATION = -Os
MCU_FLAG = 

# --------- 

RELEASE_OBJECTS		:= $(CSRCS:%.c=$(OBJECT_DIRECTORY)/%.o)
DEBUG_OBJECTS		:= $(CSRCS:%.c=$(DEBUG_DIRECTORY)/%.o)
DEPENDENCY_OBJECTS	:= $(CSRCS:%.c=$(DEPENDENCY_DIRECTORY)/%.o)

LOCAL_OBJECTS		:= $(notdir $(RELEASE_OBJECTS))
LOCAL_DEBUG_OBJECTS	:= $(notdir $(DEBUG_OBJECTS))

# --------- Make Targets

all: release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(TARGET).map
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET)
	$(VERBOSE) $(MAKE_EXE) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
debug: debug_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(TARGET).map
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET)
	$(VERBOSE) $(MAKE_EXE) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

release: release_dir release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(RELEASE_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(RELEASE_DIRECTORY)/$(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(RELEASE_DIRECTORY)/$(TARGET)
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(RELEASE_DIRECTORY)/$(TARGET).map
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(RELEASE_DIRECTORY)/$(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

eclipse: dependency_obj

clean:
	$(VERBOSE) $(ECHO) - Removing object directory from filesystem
	$(VERBOSE) $(RM) $(RM_FLAGS) $(OBJECT_DIRECTORY)
	$(VERBOSE) $(ECHO) - Removing generated program-files
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).hex
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).lss
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

release_obj: obj_dir $(RELEASE_OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Relase-Objects - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(LOCAL_OBJECTS:%=$(OBJECT_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

debug_obj: debug_dir $(DEBUG_OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Debug-Objects - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(DEBUG_ENABLED) $(LOCAL_DEBUG_OBJECTS:%=$(DEBUG_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

dependency_obj: dependency_dir $(DEPENDENCY_OBJECTS)

# --------- 
	
hex_file:
	$(VERBOSE) $(ECHO) - Generating $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) $(SECTIONS) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).elf $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) $(SECTIONS) $(HEXFLAGS) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).elf $(OBJECT_DIRECTORY)/$(TARGET).hex
	
lss_file:
	$(VERBOSE) $(ECHO) $(MSG_LISTING)
	$(VERBOSE) $(CC_DUMP) -h -S $(OBJECT_DIRECTORY)/$(TARGET).elf > $(OBJECT_DIRECTORY)/$(TARGET).lss

prog_size:
	$(CC_SIZE) $(OBJECT_DIRECTORY)/$(TARGET).elf

# --------- 

obj_dir:
	$(VERBOSE) $(ECHO) - Creating Object directory: $(OBJECT_DIRECTORY)
	$(VERBOSE) $(MK) $(OBJECT_DIRECTORY)
	
dependency_dir: obj_dir
	$(VERBOSE) $(ECHO) - Creating Dependency directory: $(DEPENDENCY_DIRECTORY)
	$(VERBOSE) $(MK) $(DEPENDENCY_DIRECTORY)

debug_dir: obj_dir
	$(VERBOSE) $(ECHO) - Creating Debug directory: $(DEBUG_DIRECTORY)
	$(VERBOSE) $(MK) $(DEBUG_DIRECTORY)

release_dir:
	$(VERBOSE) $(ECHO) - Creating Release directory: $(RELEASE_DIRECTORY)
	$(VERBOSE) $(MK) $(RELEASE_DIRECTORY)

# --------- 

install:
	$(VERBOSE) $(ECHO) - Copy service to target: /etc/init.d/$(TARGET_SERVICE)
	$(VERBOSE) $(CP) service/shc_service /etc/init.d/$(TARGET_SERVICE)
	$(VERBOSE) $(MAKE_EXE) /etc/init.d/$(TARGET_SERVICE)
	$(VERBOSE) $(ECHO) - Copy daemon to target: /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(CP) $(RELEASE_DIRECTORY)/$(TARGET) /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(MAKE_EXE) /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(ECHO) - Register service with inid.d
	$(VERBOSE) update-rc.d $(TARGET_SERVICE) defaults
	$(VERBOSE) update-rc.d $(TARGET_SERVICE) enable
	$(VERBOSE) $(ECHO) - Starting service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) start
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

uninstall: stop_service
	$(VERBOSE) $(ECHO) - Disabling service
	$(VERBOSE) update-rc.d $(TARGET_SERVICE) disable
	$(VERBOSE) $(ECHO) - Removing service from init.d
	$(VERBOSE) update-rc.d $(TARGET_SERVICE) remove
	$(VERBOSE) $(ECHO) - Removing service from filesystem
	$(VERBOSE) $(RM) /etc/init.d/$(TARGET_SERVICE)
	$(VERBOSE) $(ECHO) - Removing Daemon from filesystem
	$(VERBOSE) $(RM) /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

update: stop_service
	$(VERBOSE) $(ECHO) - Updateing daemon
	$(VERBOSE) $(CP) $(RELEASE_DIRECTORY)/$(TARGET) /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(MAKE_EXE) /usr/sbin/$(TARGET_DAEMON)
	$(VERBOSE) $(ECHO) - Starting service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) start
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

stop_service:
	$(VERBOSE) $(ECHO) - Stopping service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) stop

start_service:
	$(VERBOSE) $(ECHO) - Starting service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) start

# --------- 

fw_update: stop_service
	$(VERBOSE) $(AVR_DUDE) -C $(AVR_DUDE_CFG_FILE) -c $(AVR_DUDE_PROGRAMMER) -p $(AVR_DUDE_MCU_NAME) $(AVR_DUDE_PORT) -b $(AVR_DUDE_BAUDRATE) -U flash:w:"$(AVR_DUDE_UPDATE_PATH)/$(AVR_DUDE_UPDATE_FILE)":$(AVR_DUDE_UPDATE_FORMAT)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_SCK) $(GPIO_MODE_SCK)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MOSI) $(GPIO_MODE_MOSI)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MISO) $(GPIO_MODE_MISO)
	$(VERBOSE) $(ECHO) - Starting service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) start
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
fuses: stop_service
	$(VERBOSE) $(AVR_DUDE) -C $(AVR_DUDE_CFG_FILE) -c $(AVR_DUDE_PROGRAMMER) -p $(AVR_DUDE_MCU_NAME) $(AVR_DUDE_PORT) -b $(AVR_DUDE_BAUDRATE) -U lfuse:w:$(AVR_LFUSE):m -U hfuse:w:$(AVR_HFUSE):m -U efuse:w:$(AVR_EFUSE):m
	$(VERBOSE) $(ECHO) - Starting service
	$(VERBOSE) /etc/init.d/$(TARGET_SERVICE) start
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

create_user:
	$(VERBOSE) $(ECHO) - Creating SHC user
	$(VERBOSE) useradd -M -s /bin/false -G gpio,audio,spi shc

git_update:
	$(VERBOSE) git pull
	$(VERBOSE) $(MAKE_FILE_RIGHTS) $(APP_PATH)/*.c
	$(VERBOSE) $(MAKE_FILE_RIGHTS) $(APP_PATH)/*.h
	$(VERBOSE) $(MAKE_FILE_RIGHTS) Makefile

# --------- 

$(OBJECT_DIRECTORY)/%.o: %.c
	$(VERBOSE) $(ECHO) $(MSG_COMPILING) $(notdir $<)
	$(VERBOSE) $(CC) -c $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(OBJECT_DIRECTORY)/$(notdir $@)

$(DEBUG_DIRECTORY)/%.o: %.c
	$(VERBOSE) $(ECHO) $(MSG_COMPILING) $(notdir $<)
	$(VERBOSE) $(CC) -c $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(DEBUG_ENABLED) $(LIBS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(DEBUG_DIRECTORY)/$(notdir $@)
	
$(DEPENDENCY_DIRECTORY)/%.o: %.c
	$(VERBOSE) $(ECHO) $(MSG_DEPENDENCY) $(notdir $<)
	$(VERBOSE) $(CC) -M -c $(DEFS) $(CFLAGS) $(LIBS) $(MCU_FLAG) $(INC_PATH:%=-I%) $< -o $(DEPENDENCY_DIRECTORY)/$(notdir $@)
