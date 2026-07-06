#include "rtc_driver.h"

#define RTC_WPR_KEY_1 (0xCAU)
#define RTC_WPR_KEY_2 (0x53U)
#define WAKE_UP_TIMER_16_BIT_MAX (0xFFFFU)
#define WAKE_UP_TIMER_17_BIT_MAX (0x1FFFFU)

static void enable_rtc_reg_mod(void) {
	uint32_t temp;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/* To enable modification of RTC registers. This is a must before the forced BD reset. */
	PWR->CR |= PWR_CR_DBP;

	/* Delay to ensure DBP is configured. */
	temp = PWR->CR;

	/* Disable write protection of RTC registers. */
	RTC->WPR = RTC_WPR_KEY_1;
	RTC->WPR = RTC_WPR_KEY_2;
}

static void disable_rtc_reg_mod(void) {
	uint32_t temp;

	/* Enable lock protection. */
	RTC->WPR = RTC_WPR_KEY;

	/* To disable modification of RTC registers. */
	PWR->CR &= ~PWR_CR_DBP;

	/* Delay to ensure DBP is configured. */
	temp = PWR->CR;

	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
}


void rtc_init(void) {
	/* Turn LSI ON, this clock is used by RTC Wake-up timer. */
	RCC->CSR |= RCC_CSR_LSION;

	/* Wait until LSI is stable. */
	while(!(RCC->CSR & RCC_CSR_LSIRDY)) {}

	/* Backup domain reset will modify RTC register, so we must enable access it*/
	enable_rtc_reg_mod();

	/* Force a backup domain reset to modify clock source for RTC. */
	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;

	/* Select LSI as clock source/input for RTC. */
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;

	/* Enable RTC clock. */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	disable_rtc_reg_mod();
} /* End of rtc_config(). */



/* Wake-up timer configuration. Timer resolution is 1 second. From 1s to 36 hours (in seconds). */
void setRTC_timer(uint32_t timerDuration) {
	__disable_irq();
	/* If timer duration greater than 36 hours (in seconds), default to max duration. */
	if(timerDuration > WAKE_UP_TIMER_17_BIT_MAX) {
		timerDuration = WAKE_UP_TIMER_17_BIT_MAX;
	}

	enable_rtc_reg_mod();

	/* Disable wake-up timer. */
	RTC->CR &= ~RTC_CR_WUTE;

	/* Poll until write access to wake-up timer is allowed. */
	while(!(RTC->ISR & RTC_ISR_WUTWF)) {}

	/* Select 1 Hz wake-up clock (i.e. 1 tick per 1 second) and register . */
	if(timerDuration > WAKE_UP_TIMER_16_BIT_MAX) {
		/* 17-Bit timer. Up to 36 hours. */
		RTC->CR |= RTC_CR_WUCKSEL_1 | RTC_CR_WUCKSEL_2;
	}
	else {
		/* 16-Bit timer. Up to 18 hours. */
		RTC->CR |= RTC_CR_WUCKSEL_2;
	}

	/* Set the timer reload value to timerDuration parameter.
	 * This also make sure 0x10000 (i.e. bit 16 is not included.
	 * */
	RTC->WUTR = (timerDuration & WAKE_UP_TIMER_16_BIT_MAX);

	/* Configure wake-up interrupt. */
	EXTI->IMR |= EXTI_IMR_MR22;
	EXTI->EMR |= EXTI_EMR_MR22;
	EXTI->RTSR |= EXTI_RTSR_TR22;   /* Rising-edge. */
	NVIC_EnableIRQ(RTC_WKUP_IRQn);
	RTC->CR |= RTC_CR_WUTIE; 		/* Enable wake-up interrupt. */

	/* Ensure interrupt line for wake-up timer is clear. */
	if(EXTI->PR & EXTI_PR_PR22) {
		EXTI->PR |= EXTI_PR_PR22;
	}

	/* Ensure flag for wake-up timer is clear. */
	if(RTC->ISR & RTC_ISR_WUTF) {
		RTC->ISR &= ~RTC_ISR_WUTF;
	}

	/* Enable interrupts. */
	__enable_irq();

	/* Start wake-up timer. */
	RTC->CR |= RTC_CR_WUTE;

	/* Enable lock protection. */
	RTC->WPR = RTC_WPR_KEY;

	disable_rtc_reg_mod();
} /* End of setRTC_timer(). */

/* Clears wake-up timer flag. To reload the timer and ensure device go to low-power mode. */
void clear_rtc_wutf(void) {
	enable_rtc_reg_mod();
	if(RTC->ISR & RTC_ISR_WUTF) {
		RTC->ISR &= ~RTC_ISR_WUTF;
	}
	disable_rtc_reg_mod();
}

/* Stop the timer entirely. Timer must be reconfigured again after disabling. */
void disable_wakeup_timer(void) {
	enable_rtc_reg_mod();

	/* Disable wake-up timer. */
	RTC->CR &= ~RTC_CR_WUTE;
	RTC->ISR &= ~RTC_ISR_WUTF;

	disable_rtc_reg_mod();
} /* End of disable_wakeupTimer(). */


