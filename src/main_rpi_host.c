/*! 
 * --------------------------------------------------------------------------------
 *
 * 	@file		main_rpi_hat.c
 * 	@author		sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "mcu_task_management/mcu_task_controller.h"

// --------------------------------------------------------------------------------

void task_yield(void) {
	mcu_task_controller_background_run();
}

// --------------------------------------------------------------------------------

static void parse_command_line_arguments(int argc, char* argv[]) {

}

static void command_line_usage(void) {
	printf("\nUsage: spiHelper [options]]\n\n");
	printf("Options:\n");
	printf("-dev <device>                        : SPI-device to use for communication\t\n");
	printf("-cmd <command>                       : command to send in hexadecimal form (e.g. 0101)\t\n");
}

// --------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

	ATOMIC_OPERATION
	(
		initialization();
	)

	DEBUG_PASS("main() - Initialization done - starting application");

	for (;;) {
		
		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}

	return 0;
}