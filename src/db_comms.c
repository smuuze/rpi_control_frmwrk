/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Reader Projekt 2182 Hardware (Hauptprozessor 1284)
 *
 * \author               bk
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"
#include RTOS_H

#include <ctype.h>
#include <string.h>
#include "cpu/avr/iardebus/debusapi.h" // GCC/frmwrk/cpu/avr/iardebus/
#include <fakertos.h>

#ifdef USE_GENERIC_FILE_HANDLER
#include "service/doorloxx/smart_refresh/doorloxx_smart_refresh.h"
#endif

#ifdef USE_FILE_OPERATION_DISPATCHER
#include "debus_file_operation_dispatcher.h"
#endif

#include "rpi_protocol_handler.h"
#include "db_comms.h"

#define noTRACES
#include "traces.h"

static volatile u8 __debus_status = 0;
#define DEBUS_SET_STATUS(status)	__debus_status |= (status)
#define DEBUS_UNSET_STATUS(status)	__debus_status &= ~(status)
#define DEBUS_GET_STATUS(status)	(__debus_status & (status))
#define DEBUS_CLEAR_STATUS()		__debus_status = 0

#define DEBUS_STATUS_ACTIVE		(1 << 0)
#define DEBUS_STATUS_HOST_COM_TIMEDOUT	(1 << 1)

static u16 __host_com_time = 0;
#define DEBUS_HOST_COM_TIMEOUT_MS	7000
#define DEBUS_HOST_COM_UPDATE_TIMEOUT()	__host_com_time = msticker_get_time_ux16();
#define DEBUS_HOST_COM_IS_TIMEOUT()	(mstimer_time_is_up_ux16(__host_com_time, DEBUS_HOST_COM_TIMEOUT_MS) ? 1 : 0)


void debus_task_schedule(void) {

	#if SIO_FOR_HOST != '-'
	{
		if (!debus_msg_ready()) {

			if (mstimer_time_is_up_ux16(__host_com_time, DEBUS_HOST_COM_TIMEOUT_MS)) {
				DEBUS_SET_STATUS(DEBUS_STATUS_HOST_COM_TIMEDOUT);
			}

			return;
		}

		if (!debus_tx_busy()) {

			debus_command_handler();

			DEBUS_HOST_COM_UPDATE_TIMEOUT();
			DEBUS_UNSET_STATUS(DEBUS_STATUS_HOST_COM_TIMEDOUT);
		}
	}
	#endif
}

void debus_host_com_init(void) {
	DEBUS_CLEAR_STATUS();
	DEBUS_HOST_COM_UPDATE_TIMEOUT();
}

u8 debus_host_com_timeout(void) {
	return DEBUS_GET_STATUS(DEBUS_STATUS_HOST_COM_TIMEDOUT) != 0 ? 1 : 0;
}

void debus_polling_handler ( void ) {

	PASS(); // debus_polling_handler() ---------------
	debus_ok_msg();
}


/*******************************************************************************
 *******************************************************************************
 * debus command handler
 *******************************************************************************
 ******************************************************************************/


/// deBus Kommando GET_ACTIVE Steuerung
///
/// deBus Kommando GET_ACTIVE wird auf Broadcast nur dann beantwortet, wenn diese
/// Funktion TRUE liefert (z.B. wenn eine Taste auf dem Geraet gedrueckt wurde);
/// possible implementation:
///   return BUTTON();
unsigned char DEBUS_CALLBACK_CMD_GETACTIVE( void ) {
	return 0;
}

/// Zusatzinformationen fuer das Versionstelegramm
///
/// Es duerfen nur debus_put_XXX - Funktionen aufgerufen werden.
/// debus_stop_msg() wird dann vom Framework aufgerufen.
void DEBUS_CALLBACK_CMD_VERSION_TAIL( void ) {

}

/// Erweiterung fuer die Antwort auf CMD_STATUS
///
/// Diese Funktion ist der richtige Ort, um Statustelegramme mit eigenen Informationen
/// zu erweitern. Es duerfen nur debus_put_XXX - Funktionen aufgerufen werden.
/// debus_stop_msg() wird dann vom Framework aufgerufen.
void DEBUS_CALLBACK_CMD_GETSTATUS_MSG( void ) {

}

void FATAL_ERROR_STOP_LED_AND_BEEPER( void ) {

}

// Operation Code Tabelle fuer den generic Handler.
#ifdef USE_GENERIC_FILE_HANDLER
CODEMEM const u8_fkt_void_pointer_table_entry fileop_generic_handler_table[] =
{
  {TAGUPD_DOORLOXX,doorloxx_smartRefresh_handler},
};
#endif

#if SIO_FOR_HOST != '-'

/// Tabelle um einen eigenen Handler fuer CMD_CONFIG zu aktivieren
///
/// Diese Tabelle wird von debus_command_handler() verwendet um den
/// passenden Handler fuer ein bestimmtes CMD_CONFIG - Register aufzurufen.
//  Am Ende der Tabelle muss immer der Eintrag { 0x00, cfg_invalid_selector } stehen.
CODEMEM const debus_command_handler_entry CMD_CONFIG_HANDLER_TABLE[] =
{
  { 0x00, cfg_invalid_selector  } // muss IMMER der letzte Eintrag sein!
};

/// Tabelle um deBus-Commandhandler an die entsprechende CMD_ID zu binden.
///
/// Diese Tabelle wird von debus_command_handler() verwendet um den
/// passenden Handler fuer eine CMD_ID aufzurufen. Am Ende der Tabelle
/// muss immer der Eintrag { 0x00, debus_default_handler } stehen.
CODEMEM const debus_command_handler_entry debus_command_handler_table[] =
{
  { CMD_POLL, debus_polling_handler },          // Sendet die zuletzt gelesenen Nummer oder nur Status
#ifdef USE_FILE_OPERATION_DISPATCHER
  { CMD_FILEOPERATION, debus_file_operation_dispatcher },  // Fileoperation -> Legic-Debus-Tunnel
#endif
  { 0x66, rpi_protocol_handler_debus_handler},
  { 0x00, debus_default_handler } // muss IMMER der letzte Eintrag sein!
};

#endif


//==============================================================================
