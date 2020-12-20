
# --------- Message Output

MSG_COMPILING		:= Compiling
MSG_DEPENDENCY		:= Generating dependency for 
MSG_LINKING		:= Linking to
MSG_RESET_DEVICE	:= RESETTING DEVICE !
MSG_PROG_LOCATION	:= Your programm can be found at
MSG_FLASH_LOCATION	:= Using this file for Flashing: 
MSG_LISTING		:= - Generating Disassembly
MAP_LISTING		:= - Generating memory map
MSG_TRACER		:= Starting Tracer in
MSG_FINISH		:= --------------- Make done ---------------

# --------- 

TARGET			:= $(PROJECT)
VERSION			:= $(VERSION_MAJOR).$(VERSION_MINOR)
OBJECT_DIRECTORY	:= obj
DEBUG_DIRECTORY		:= $(OBJECT_DIRECTORY)/debug
DEPENDENCY_DIRECTORY	:= $(OBJECT_DIRECTORY)/dependency
RELEASE_DIRECTORY	:= release/$(VERSION)
FORMAT			:= ihex
DEBUG_ENABLED		:= -DTRACER_ENABLED

RELEASE_OBJECTS		:= $(CSRCS:%.c=$(OBJECT_DIRECTORY)/%.o)
DEBUG_OBJECTS		:= $(CSRCS:%.c=$(DEBUG_DIRECTORY)/%.o)
DEPENDENCY_OBJECTS	:= $(CSRCS:%.c=$(DEPENDENCY_DIRECTORY)/%.o)

LOCAL_OBJECTS		:= $(notdir $(RELEASE_OBJECTS))
LOCAL_DEBUG_OBJECTS	:= $(notdir $(DEBUG_OBJECTS))

TRACER_PATH		:= $(BASE_PATH)/rpi_control_sw/cfg_TRACER/shcTracer

CFLAGS 			+= -pedantic -Wall

# --------- 

all: release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET).$(PLATTFORM_EXTENSION)
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET).$(PLATTFORM_EXTENSION)
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
debug: debug_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(TARGET).$(PLATTFORM_EXTENSION)
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(TARGET).map
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(TARGET).elf
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

release: release_dir release_obj hex_file lss_file prog_size
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(RELEASE_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).lss $(RELEASE_DIRECTORY)/$(TARGET).lss
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).elf $(RELEASE_DIRECTORY)/$(TARGET).$(PLATTFORM_EXTENSION)
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(RELEASE_DIRECTORY)/$(TARGET).map
	$(VERBOSE) $(ECHO) $(MSG_PROG_LOCATION) $(RELEASE_DIRECTORY)/$(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

eclipse: dependency_obj 

# --------- 

clean:
	$(VERBOSE) $(ECHO) - Removing object directory from filesystem
	$(VERBOSE) $(RM) $(RM_FLAGS) $(OBJECT_DIRECTORY)
	$(VERBOSE) $(ECHO) - Removing generated program-files
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).hex
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).lss
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).$(PLATTFORM_EXTENSION)
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET).map
	$(VERBOSE) $(RM) $(RM_FLAGS) $(TARGET)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

run:
	./$(TARGET).$(PLATTFORM_EXTENSION)

# --------- 

release_obj: obj_dir $(RELEASE_OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Relase-Objects - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(LOCAL_OBJECTS:%=$(OBJECT_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

debug_obj: debug_dir $(DEBUG_OBJECTS)
	$(VERBOSE) $(ECHO) - Generating Debug-Objects - Version: $(VERSION)
	$(VERBOSE) $(CC) $(OPTIMIZATION) $(DEFS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(MCU_FLAG) $(INC_PATH:%=-I%) $(DEBUG_ENABLED) $(LOCAL_DEBUG_OBJECTS:%=$(DEBUG_DIRECTORY)/%) -o $(OBJECT_DIRECTORY)/$(TARGET).elf

dependency_obj: dependency_dir $(DEPENDENCY_OBJECTS)

# --------- 

ifneq ($(PLATTFORM), MACOS)

hex_file:
	$(VERBOSE) $(ECHO) - Generating $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) $(SECTIONS) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).elf $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CC_COPY) $(SECTIONS) $(HEXFLAGS) -O $(FORMAT) $(OBJECT_DIRECTORY)/$(TARGET).elf $(OBJECT_DIRECTORY)/$(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).hex $(TARGET).hex
	$(VERBOSE) $(CP) $(OBJECT_DIRECTORY)/$(TARGET).map $(TARGET).map
	
lss_file:
	$(VERBOSE) $(ECHO) $(MSG_LISTING)
	$(VERBOSE) $(CC_DUMP) -h -S $(OBJECT_DIRECTORY)/$(TARGET).elf > $(OBJECT_DIRECTORY)/$(TARGET).lss

else

hex_file:
	
lss_file:

endif
	
obj_dir:
	$(VERBOSE) $(ECHO) - Creating Object directory: $(OBJECT_DIRECTORY)
	$(VERBOSE) $(MK) $(OBJECT_DIRECTORY)
	
prog_size:
	$(CC_SIZE) $(MCU_SIZE_FLAGS) $(OBJECT_DIRECTORY)/$(TARGET).elf

# --------- 
	
debug_dir: obj_dir
	$(VERBOSE) $(ECHO) - Creating Debug directory: $(DEBUG_DIRECTORY)
	$(VERBOSE) $(MK) $(DEBUG_DIRECTORY)
	
dependency_dir: obj_dir
	$(VERBOSE) $(ECHO) - Creating Dependency directory: $(DEPENDENCY_DIRECTORY)
	$(VERBOSE) $(MK) $(DEPENDENCY_DIRECTORY)

release_dir:
	$(VERBOSE) $(ECHO) - Creating Release directory: $(RELEASE_DIRECTORY)
	$(VERBOSE) $(MK) $(RELEASE_DIRECTORY)

# ---------

tracer:
	$(VERBOSE) $(ECHO) $(MSG_TRACER) $(TRACER_PATH) -console -file $(PROJECT)_v$(VERSION)_trace_output.txt -path .
	$(TRACER_PATH) -console -file $(PROJECT)_v$(VERSION)_trace_output.txt

# --------- 

flash: 
	$(VERBOSE) $(ECHO) $(MSG_FLASH_LOCATION) $(TARGET).hex
	$(VERBOSE) $(AVR_DUDE) -C $(AVR_DUDE_CFG_FILE) -c $(AVR_DUDE_PROGRAMMER) -p $(AVR_DUDE_MCU_NAME) $(AVR_DUDE_PORT) -b $(AVR_DUDE_BAUDRATE) -U flash:w:"$(TARGET).hex":$(AVR_DUDE_UPDATE_FORMAT)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_SCK) $(GPIO_MODE_SCK)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MOSI) $(GPIO_MODE_MOSI)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MISO) $(GPIO_MODE_MISO)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)
	
fuses:
	$(VERBOSE) $(AVR_DUDE) -C $(AVR_DUDE_CFG_FILE) -c $(AVR_DUDE_PROGRAMMER) -p $(AVR_DUDE_MCU_NAME) $(AVR_DUDE_PORT) -b $(AVR_DUDE_BAUDRATE) -U lfuse:w:$(AVR_LFUSE):m -U hfuse:w:$(AVR_HFUSE):m -U efuse:w:$(AVR_EFUSE):m
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

reset:
	$(VERBOSE) $(ECHO) $(MSG_RESET_DEVICE)
	$(VERBOSE) gpio mode 5 out
	$(VERBOSE) gpio write 5 0
	$(VERBOSE) gpio write 5 1
	$(VERBOSE) gpio mode 5 in
	$(VERBOSE) gpio mode 5 up
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

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

fw_update_only: stop_service
	$(VERBOSE) $(AVR_DUDE) -C $(AVR_DUDE_CFG_FILE) -c $(AVR_DUDE_PROGRAMMER) -p $(AVR_DUDE_MCU_NAME) $(AVR_DUDE_PORT) -b $(AVR_DUDE_BAUDRATE) -U flash:w:"$(AVR_DUDE_UPDATE_PATH)/$(AVR_DUDE_UPDATE_FILE)":$(AVR_DUDE_UPDATE_FORMAT)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_SCK) $(GPIO_MODE_SCK)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MOSI) $(GPIO_MODE_MOSI)
	$(VERBOSE) $(GPIO_MODE) $(GPIO_PIN_MISO) $(GPIO_MODE_MISO)
	$(VERBOSE) $(ECHO) $(MSG_FINISH)

# --------- 

create_user:
	$(VERBOSE) $(ECHO) - Creating SHC user
	$(VERBOSE) useradd -M -s /bin/false -G gpio,audio,spi shc

git_update:
	$(VERBOSE) git pull
	$(VERBOSE) $(MAKE_FOLDER_RIGHTS)
	$(VERBOSE) $(MAKE_FILE_RIGHTS)

# --------- 