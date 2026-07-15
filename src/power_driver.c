#include "power_driver.h"
#include "systick.h"

static void clear_exti_lines(void) {
	/* Ensure no EXTI line is pending. */
	EXTI->PR |= 0x67FFFFU;
} /* End of clear_exti_lines(). */

/* Sleep mode. Lowest wake-up latency. */
void power_sleep(void) {
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
	clear_pwr_flags();
	/* Ensure no EXTI line is pending. */
	clear_exti_lines();
	__WFI();
} /* End of power_sleep(). */

/* Deep-sleep mode. Longer wake-up latency than sleep mode. */
void power_deepsleep(void) {
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	PWR->CR |= PWR_CR_LPDS; /* Low-power mode ON. */
	PWR->CR |= PWR_CR_CWUF; /* Clear wake up flag. */
	clear_exti_lines();
	__WFI();
} /* End of power_deepsleep(). */

/* Standby mode. Lowest amount of power consumption while asleep, but highest wake-up latency. */
void power_standby(uint8_t enableWakeupPin) {
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	PWR->CSR &= ~PWR_CSR_EWUP_Msk; /* Disable wake up wake up pin. */
	PWR->CR |= PWR_CR_CSBF;        /* Clear standby flag. */
	PWR->CR |= PWR_CR_CWUF;        /* Clear PWR Wake-up WUF flag. */
	PWR->CR |= PWR_CR_PDDS;        /* Set PDDS bit to enter standby mode. */

	if(enableWakeupPin) {
		/* Enable wake up pin. Forces PA0 to input pull down. */
		PWR->CSR |= PWR_CSR_EWUP;

		/* Wait until PA0 is released. */
		while(GPIOA->IDR & GPIO_IDR_ID0_Msk) { }

		/* Temporary fix for preventing switch debounce.
		 * Hardware implementation would be better alternative.
		 * */
		delay_ms(2000);
	}

	/* Ensure no EXTI line is pending. */
	clear_exti_lines();
	__WFI();
} /* End of power_standby(). */


/* Clear power-related flags */
void clear_pwr_flags(void) {
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_CSBF; /* Clear standby flag. */
	PWR->CR |= PWR_CR_CWUF; /* Clear wake up flag. */
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
} /* End of clear_pwr_flags(). */

