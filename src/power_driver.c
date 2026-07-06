#include "power_driver.h"

static void clear_exti_lines(void) {
	/* Ensure no EXTI line is pending. */
	EXTI->PR |= 0x67FFFFU;
}

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
	PWR->CR |= PWR_CR_CSBF;
	PWR->CR |= PWR_CR_CWUF;   /* Clear wake up flag. */
	clear_exti_lines();
	__WFI();
} /* End of power_deepsleep(). */


/* Standby mode. Lowest amount of power consumption while asleep, but highest wake-up latency. */
void power_standby(void) {
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* Set PA0 to input. This is the trigger wake up pin. */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0_Msk);

	/* Wait until PA0 is released. */
	while(GPIOA->IDR & GPIO_IDR_ID0_Msk) { }

	PWR->CSR &= ~PWR_CSR_EWUP_Msk;	/* Disable wake up triggers. */

	PWR->CR |= PWR_CR_CSBF;
	PWR->CR |= PWR_CR_CWUF;   /* Clear wake up flag. */
	PWR->CR |= PWR_CR_PDDS;	  /* Set PDDS bit to enter standby mode. */
	PWR->CSR |= PWR_CSR_EWUP; /* Enable wake up triggers. */

	/* Ensure no EXTI line is pending. */
	clear_exti_lines();
	__WFI();
} /* End of power_standby(). */


/* Clear power-related flags */
void clear_pwr_flags(void) {
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	if(PWR->CSR & PWR_CSR_SBF) {
		PWR->CR |= PWR_CR_CSBF; /* Clear standby flag. */
	}
	if(PWR->CSR & PWR_CSR_WUF) {
		PWR->CR |= PWR_CR_CWUF; /* Clear wake up flag. */
	}

	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
}

