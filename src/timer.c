#include "timer.h"


void timer_config(void) {
	/* Enable clock for TIM2. */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* 16 MHz is the default frequency on RESET.
	 * This give microsecond resolution. */
	TIM2->PSC = 16 - 1;

	/* Counter counts from 0 - 32-bit max. */
	TIM2->ARR = TIM_ARR_ARR;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void timer_start(void) {
	/* Enable timer. */
	TIM2->CR1 |= TIM_CR1_CEN;
}

void timer_stop(void) {
	/* Disable timer. */
	TIM2->CR1 &= ~(TIM_CR1_CEN);
}

void timer_reset(void) {
	/* Clear counter. */
	TIM2->CNT = 0;
}

uint32_t getTime(void) {
	return(TIM2->CNT);
}

void timDelay_us(uint32_t delay) {
	uint32_t startTime = TIM2->CNT;
	while((TIM2->CNT - startTime) < delay) {}
}

