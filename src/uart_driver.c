#include "uart_driver.h"
#include <math.h>

#define SYSTEM_CLOCK_FREQ (16000000U) /* 16MHz core clock. */

/***** Global variables START. *****/
uint8_t receiveFromUSB;
uint8_t uart_initialized = 0;
/***** Global variables END. *****/

/***** Helper functions definition START. *****/

/* UART TX helper for printf(). */
static void uart_printf(uint8_t data) {
	/* Enable TX. Sends preamble (IDLE word). */
	USART2->CR1 |= USART_CR1_TE;

	/* To clear TC bit if necessary. Read SR then write to DR. */
	uint32_t temp = USART2->SR;

	while(!(USART2->SR & USART_SR_TXE_Msk)) {}
	USART2->DR = data;

	/* Wait for TC bit to ensure transmission completes before disabling TX. Will be cleared on next write to TX. */
	while(!(USART2->SR & USART_SR_TC_Msk)) {}

	/* Disable TX. */
	USART2->CR1 &= ~USART_CR1_TE_Msk;
} /* End of uart_printf(). */

/* Called every time printf is invoked. Declared in syscalls.c file. */
int __io_putchar(int ch) {
	uart_printf(ch & 0xFFU);
	return ch;
} /* End of __io_putchar(). */


/* Sets the Baudrate register. */
static void setBRR(uint32_t baudrate) {
	/* Enable full access to FPU Coprocessor. */
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	uint16_t brr;
	uint8_t over8 = USART2->CR1 & USART_CR1_OVER8_Msk ? 1 : 0;

	/* Calculate USARTDIV. Formula per datasheet. */
	float usartDiv = (float)SYSTEM_CLOCK_FREQ / (8.0f * (2 - over8) * (float)baudrate);

	uint16_t mantissa = (uint16_t)usartDiv;	/* Extract integer portion. */
	float fractional = usartDiv - mantissa; /* Extract fractional portion. */

	if(over8) { /* OVER8 = 1. */
		fractional = roundf(fractional * 8);
		if(fractional > 7) {
			fractional = 0;
			++mantissa; /* Carry-to mantissa the overflow from fractional part. */
		}
	}
	else { /* OVER8 = 0. */
		fractional = roundf(fractional * 16);
		if(fractional > 15) {
			fractional = 0;
			++mantissa; /* Carry-to mantissa the overflow from fractional part. */
		}
	}

	brr = (mantissa << 4) | (uint16_t)fractional;
	USART2->BRR = brr;
	USART1->BRR = brr; /* USART1 and USART2 have same configuration. */

	/* Disable full access to FPU Coprocessor to save power. */
	SCB->CPACR &= ~((3UL << 10*2) | (3UL << 11*2));
} /* End of setBRR(). */


static void uart_pin_config(void) {
	/* Enable clock for Port A. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* Reset mode for PA2 and PA3 and PA10. */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER10_Msk);

	/* Set mode to AF and select AF mode for TX: PA2 pin. */
	GPIOA->MODER |= (GPIO_MODER_MODER2_1);
	GPIOA->AFR[0] |= (0x7U << GPIO_AFRL_AFSEL2_Pos);

	/* Set mode to AF and select AF mode for RX: PA2 or PA10 depending where incoming data originates. */
	if(receiveFromUSB) {
		GPIOA->MODER |= (GPIO_MODER_MODER3_1);
		GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0;
		GPIOA->AFR[0] |= (0x7U << GPIO_AFRL_AFSEL3_Pos);
	}
	else {
		GPIOA->MODER |= (GPIO_MODER_MODER10_1);
		GPIOA->PUPDR |= GPIO_PUPDR_PUPD10_0;
		GPIOA->AFR[1] |= (0x7U << GPIO_AFRH_AFSEL10_Pos);
	}
}/* End of uart_pin_config(). */
/***** Helper functions definition END. *****/

/***** APIs definition START. *****/
uint8_t uart_init_status(void) {
	return uart_initialized;
} /* End of uart_init_status(). */

/* Calls helper function. */
void setBaudrate(uint32_t br) {
	setBRR(br);
} /* End of setBaudrate(). */


void uart_init(uint8_t USB_to_UART) {
	receiveFromUSB = USB_to_UART;

	/* Configure pins to be used by USART peripheral. */
	uart_pin_config();

	/* Enable clock for USART peripheral. */
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	/* Enable USART2. */
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 &= ~USART_CR1_M_Msk;	/* 8-bit data length. */
	USART2->CR2 &= ~USART_CR2_STOP_Msk; /* 1-bit stop bit. */

	/* Enable USART1 if necessary. */
	if(!receiveFromUSB) {
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		USART1->CR1 |= USART_CR1_UE;
		USART1->CR1 &= ~USART_CR1_M_Msk;	/* 8-bit data length. */
		USART1->CR2 &= ~USART_CR2_STOP_Msk; /* 1-bit stop bit. */
	}
	uart_initialized = 1;
} /* End of uart_init(). */

/* Send buffered data. */
void uart_send(uint8_t *data, uint32_t len) {
	uint32_t temp;

	/* Enable TX. Sends preamble (IDLE word). */
	USART2->CR1 |= USART_CR1_TE;

	for(volatile int i = 0; i < len; ++i) {
		/* To clear TC bit if necessary. Read SR then write to DR. */
		temp = USART2->SR;
		/* Wait for TXE bit to be set. */
		while(!(USART2->SR & USART_SR_TXE_Msk)) {}
		USART2->DR = data[i];
	}

	/* Wait for TC bit to ensure transmission completes before disabling TX. Will be cleared on next write to TX. */
	while(!(USART2->SR & USART_SR_TC_Msk)) {}

	/* Disable TX. */
	USART2->CR1 &= ~USART_CR1_TE_Msk;
} /* End of uart_send(). */

void uart_receive(uint8_t *buffer, uint32_t len) {
	USART_TypeDef* targetUSART;

	/* Select RX source. */
	if(receiveFromUSB) {
		targetUSART = USART2;

	}
	else {
		targetUSART = USART1;
	}

	/* Enable RX. Listens to preamble (IDLE word). */
	targetUSART->CR1 |= USART_CR1_RE;


	/* Wait for a byte to arrive. */
	for(uint32_t i = 0; i < len; ++i) {
		while(!(targetUSART->SR & USART_SR_RXNE_Msk)) {}
		uint32_t status = targetUSART->SR;

		buffer[i] = targetUSART->DR;
	}

	/* Disable RX. */
	targetUSART->CR1 &= ~USART_CR1_RE;
} /* End of uart_receive(). */

/***** API definition END. *****/
