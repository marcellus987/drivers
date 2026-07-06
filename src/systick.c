

#include "systick.h"

#define CTRL_ENABLE (1U << 0) /* Enable bit of the Systick control and status register. */
#define CTRL_CLCKSRC (1U << 2) /* 3rd bit. Setting the bit selects the Processor clock. See documentation. */
#define CTRL_COUNTFLAG (1U << 16) /* 16th bit to check if counter reaches value of 0 since last read. */


#define ONE_MSEC_LOAD 16000 /* 16MHz is default speed. So there is
							   Clock cycle = CPU Speed(freq/time) / time:
							   16,000,000 cycle/s / 1000s = 16,000 */
#define ONE_MICROSEC_LOAD 16


/* Four register extracted from Generic User guide document from ARM, includes:
 * SYST_CSR == SysTick->CTRL (CMSIS) -> Control and Status.
 * SYST_RVR == SysTick->LOAD (CMSIS) -> Reload value.
 * SYST_CVR == SysTick->VALUE (CMSIS) -> Current value.
 * SYST_CALIB == SysTick->CALIB (CMSIS) -> Calibration properties.
 * */


void delay_ms(uint32_t delay) {
	/* Load the value to register. */
	SysTick->LOAD = ONE_MSEC_LOAD - 1; // 0 to 15,999 == 16000 Clock cycles.

	/* Reset systick value register . */
	SysTick->VAL = 0;

	/* Select processor clock as clock source. */
	SysTick->CTRL |= CTRL_CLCKSRC;

	/* Enable timer. */
	SysTick->CTRL |= CTRL_ENABLE;

	for(volatile int i = 0; i < delay; ++i) {
		/* Or, while(!(SysTick->CTRL & CTRL_COUNTFLAG)) {} */
		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0) {

		}
	}

	/* Disable systick */
	SysTick->CTRL = 0;

}


