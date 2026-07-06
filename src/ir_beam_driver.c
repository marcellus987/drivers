#include "ir_beam_driver.h"

#define IR_RECEIVER_POWER_Pos (5U)
#define IR_OUTPUT_Pos (7U)
#define IR_TRANSMITTER_POWER_Pos (6U)

static void ir_pin_config(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* Set output mode. This will power the IR beam. */
	GPIOB->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0);

	/* Set input mode. */
	GPIOB->MODER &= ~GPIO_MODER_MODER10_Msk;

	/* Enable pull-up for receiver output. */
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD10_0;
}

void ir_beam_init(void) {
	ir_pin_config();
//	exti_config();
}

void ir_beam_on(void) {
	GPIOB->BSRR |= GPIO_BSRR_BS12 | GPIO_BSRR_BS13;
}

void ir_beam_off(void) {
	GPIOB->BSRR |= GPIO_BSRR_BR12 | GPIO_BSRR_BR13;
}

uint8_t check_sensor_status(void) {
	return (GPIOB->IDR & GPIO_IDR_ID10 ? 1 : 0);
}

//static void exti_config(void) {
//	__disable_irq();
//	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//
//	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
//
////	NVIC_EnableIRQ(EXTI9_5_IRQn);
//	NVIC_EnableIRQ(EXTI15_10_IRQn);
//
//	__enable_irq();
//}
