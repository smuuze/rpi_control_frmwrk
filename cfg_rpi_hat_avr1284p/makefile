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

# Dies ist die Software fuer die PDL2A / PDL5A-Hardware.

FREIGABE_ID       =
include ../src/version_string.mk

# PROJEKT Name des Hexfiles
PROJECT = RPI_Hat_ATMega1284P

# HW_DEVICE_TYPE ( siehe http://ew.deister-gmbh.de/cgi-bin/ibase/gertyp.cgi )
HW_DEVICE_TYPE = 0x22B2

# SW_DEVICE_TYPE wird im deBus Versionstelegramm und fuer Adressumstellung verwendet
SW_DEVICE_TYPE = 0x22B2

#-----------------------------------------------------------------------------
# Precompiler Schalter fuer den mak-File
#-----------------------------------------------------------------------------
MIT_UART = TRUE
APP_USART = TRUE
MIT_BLUETOOTH = TRUE

#MIT_LOCAL_FAKERTOS = TRUE


#-----------------------------------------------------------------------------
APP_PATH = ../src
BASE_PATH    = ../../../..

CSRCS += ../src/platine/pin_map_171006.c
CSRCS += ../src/main_rpi_hat.c

# main config selector
include $(BASE_PATH)/local.mk
include $(DE_FRM)/make/mega1284_debl_v1.mk

#-----------------------------------------------------------------------------
# Projektabhaengige Dateien

#-----------------------------------------------------------------------------
# Fuer alle Projekte gueltige Dateien
include ../src/project_default.mk
