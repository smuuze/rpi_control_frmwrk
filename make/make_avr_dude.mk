
AVR_DUDE		:= '/usr/bin/avrdude'
AVR_DUDE_MCU_NAME	:= m1284p
#AVR_DUDE_PORT		:= /dev/spidev0.0
AVR_DUDE_PORT		:=
AVR_DUDE_BAUDRATE	:= 4800
AVR_DUDE_PROGRAMMER	:= linuxgpio

#AVR_DUDE_UPDATE_PATH	:= ../rpi_control_fw/cfg_rpi_hat_ir_board
#AVR_DUDE_UPDATE_FILE	:= RPI_Hat_IR_Board.hex

AVR_DUDE_UPDATE_PATH	:= ../rpi_control_fw/cfg_rpi_hat_control_board_v2
AVR_DUDE_UPDATE_FILE	:= RPI_Hat_ControlBoard_V2.hex

AVR_DUDE_CFG_FILE	:= ../avrdude/avrdude.conf
AVR_DUDE_UPDATE_FORMAT	:= i

GPIO_MODE		:= gpio mode
GPIO_PIN_SCK		:= 14
GPIO_PIN_MOSI		:= 12
GPIO_PIN_MISO		:= 13
GPIO_MODE_MOSI		:= alt0
GPIO_MODE_MISO		:= alt0
GPIO_MODE_SCK		:= alt0

# See: http://www.engbedded.com/fusecalc/

AVR_LFUSE		?= 0xFF
AVR_HFUSE		?= 0xD9
AVR_EFUSE		?= 0xFE