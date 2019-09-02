/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//---------- Type Definitions -------------------------------------------------

#define TEST_POWER_UP_TIME_MS		15

#define TEST_TIMEOUT_POWER_UP_MS	50

//---------- Test-Case Prototype ----------------------------------------------

void module_test_signal_slot_test_callback_1(void);
void module_test_signal_slot_test_callback_2(void);
void module_test_signal_slot_test_callback_3(void);
void module_test_signal_slot_test_callback_4(void);
void module_test_signal_slot_test_callback_5(void);
void module_test_signal_slot_test_callback_6(void);

void module_test_signal_slot_chain_1(void);
void module_test_signal_slot_chain_2(void);
void module_test_signal_slot_chain_3(void);
void module_test_signal_slot_chain_4(void);
void module_test_signal_slot_chain_5(void);
void module_test_signal_slot_chain_6(void);

//---------- Static Data ------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

SIGNAL_CREATE(MAIN_TEST_SIGNAL)

SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_1, module_test_signal_slot_test_callback_1)
SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_2, module_test_signal_slot_test_callback_2)
SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_3, module_test_signal_slot_test_callback_3)
SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_4, module_test_signal_slot_test_callback_4)
SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_5, module_test_signal_slot_test_callback_5)
SLOT_CREATE(MAIN_TEST_SIGNAL, MAIN_TEST_SLOT_6, module_test_signal_slot_test_callback_6)

SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_1);
SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_2);
SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_3);
SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_4);
SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_5);
SIGNAL_CREATE(MAIN_TEST_SIGNAL_CHAIN_6);

SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_1, MAIN_TEST_SLOT_CHAIN_1, module_test_signal_slot_chain_1)
SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_2, MAIN_TEST_SLOT_CHAIN_2, module_test_signal_slot_chain_2)
SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_3, MAIN_TEST_SLOT_CHAIN_3, module_test_signal_slot_chain_3)
SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_4, MAIN_TEST_SLOT_CHAIN_4, module_test_signal_slot_chain_4)
SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_5, MAIN_TEST_SLOT_CHAIN_5, module_test_signal_slot_chain_5)
SLOT_CREATE(MAIN_TEST_SIGNAL_CHAIN_6, MAIN_TEST_SLOT_CHAIN_6, module_test_signal_slot_chain_6)

//---------- Function ---------------------------------------------------------

static u8 signal_rx_counter = 0;

int main( void ) {

	DEBUG_PASS("--------------------------------------------------");
	DEBUG_PASS("main() - Hello Module Test - Signal-Slot-Interface");
	DEBUG_PASS("--------------------------------------------------\n");

	MAIN_TEST_SIGNAL_init();
	
	MAIN_TEST_SLOT_6_connect();
	MAIN_TEST_SLOT_5_connect();
	MAIN_TEST_SLOT_3_connect();
	MAIN_TEST_SLOT_4_connect();
	MAIN_TEST_SLOT_1_connect();
	MAIN_TEST_SLOT_2_connect();

	MAIN_TEST_SIGNAL_CHAIN_1_init();
	MAIN_TEST_SIGNAL_CHAIN_2_init();
	MAIN_TEST_SIGNAL_CHAIN_3_init();
	MAIN_TEST_SIGNAL_CHAIN_4_init();
	MAIN_TEST_SIGNAL_CHAIN_5_init();
	MAIN_TEST_SIGNAL_CHAIN_6_init();

	MAIN_TEST_SLOT_CHAIN_1_connect();
	MAIN_TEST_SLOT_CHAIN_2_connect();
	MAIN_TEST_SLOT_CHAIN_3_connect();
	MAIN_TEST_SLOT_CHAIN_4_connect();
	MAIN_TEST_SLOT_CHAIN_5_connect();
	MAIN_TEST_SLOT_CHAIN_6_connect();

	TIMEOUT_TIMER_start();
	
	while (TIMEOUT_TIMER_is_up(3000) == 0) {

	}
	
	signal_rx_counter = 0;

	MAIN_TEST_SIGNAL_send();

	if (signal_rx_counter != 6) {
		DEBUG_PASS("main() Incorrect value of Signal-RX-Counter");
	}
	
	signal_rx_counter = 0;

	MAIN_TEST_SIGNAL_CHAIN_1_send();

	if (signal_rx_counter != 6) {
		DEBUG_PASS("main() Incorrect value of Signal-RX-Counter");
	}
	
	return 0;
	
}

//---------- Test-Case Implementation ---------------------------------------------

void module_test_signal_slot_test_callback_1(void) { DEBUG_PASS("module_test_signal_slot_test_callback_1() - Signal Received"); signal_rx_counter += 1; }
void module_test_signal_slot_test_callback_2(void) { DEBUG_PASS("module_test_signal_slot_test_callback_2() - Signal Received"); signal_rx_counter += 1; }
void module_test_signal_slot_test_callback_3(void) { DEBUG_PASS("module_test_signal_slot_test_callback_3() - Signal Received"); signal_rx_counter += 1; }
void module_test_signal_slot_test_callback_4(void) { DEBUG_PASS("module_test_signal_slot_test_callback_4() - Signal Received"); signal_rx_counter += 1; }
void module_test_signal_slot_test_callback_5(void) { DEBUG_PASS("module_test_signal_slot_test_callback_5() - Signal Received"); signal_rx_counter += 1; }
void module_test_signal_slot_test_callback_6(void) { DEBUG_PASS("module_test_signal_slot_test_callback_6() - Signal Received"); signal_rx_counter += 1; }

void module_test_signal_slot_chain_1(void) { DEBUG_PASS("module_test_signal_slot_chain_1() - Signal Received"); signal_rx_counter += 1; MAIN_TEST_SIGNAL_CHAIN_2_send(); }
void module_test_signal_slot_chain_2(void) { DEBUG_PASS("module_test_signal_slot_chain_2() - Signal Received"); signal_rx_counter += 1; MAIN_TEST_SIGNAL_CHAIN_3_send(); }
void module_test_signal_slot_chain_3(void) { DEBUG_PASS("module_test_signal_slot_chain_3() - Signal Received"); signal_rx_counter += 1; MAIN_TEST_SIGNAL_CHAIN_4_send(); }
void module_test_signal_slot_chain_4(void) { DEBUG_PASS("module_test_signal_slot_chain_4() - Signal Received"); signal_rx_counter += 1; MAIN_TEST_SIGNAL_CHAIN_5_send(); }
void module_test_signal_slot_chain_5(void) { DEBUG_PASS("module_test_signal_slot_chain_5() - Signal Received"); signal_rx_counter += 1; MAIN_TEST_SIGNAL_CHAIN_6_send(); }
void module_test_signal_slot_chain_6(void) { DEBUG_PASS("module_test_signal_slot_chain_6() - Signal Received"); signal_rx_counter += 1; }

