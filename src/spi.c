#include "spi.h"
#include <stdio.h>

/* Note: PA5 = SLCK, PA6 = MISO; PA7 = MOSI. */

/* Global vars. START */
volatile uint8_t spi_initialized = 0;
/* Global vars. END */

/***** Helper functions declaration START. *****/
static void spi_pin_config(void);
/***** Helper functions declaration END. *****/

/***** Helper functions definition START. *****/
static void spi_pin_config(void) {
	/* Enable clock for GPIOA. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* Clear mode bits before selecting modes. */
	GPIOA->MODER &= ~(GPIO_MODER_MODER5_Msk);
	GPIOA->MODER &= ~(GPIO_MODER_MODER6_Msk);
	GPIOA->MODER &= ~(GPIO_MODER_MODER7_Msk);

	/* Set mode to alternate function for PA5 to PA7. */
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	GPIOA->MODER |= GPIO_MODER_MODE6_1;
	GPIOA->MODER |= GPIO_MODER_MODE7_1;


	/* Clear AF selection before selecting new AF. */
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5_Msk);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL7_Msk);


	/* Select alternate functions to SPI1 for each SPI pin. */
	GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL5_Pos);
	GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL6_Pos);
	GPIOA->AFR[0] |= (5U << GPIO_AFRL_AFSEL7_Pos);
} /* End of spi_pin_config(). */
/***** Helper functions definition END. *****/


/***** API definition START. *****/
void disable_spi(uint8_t disable_clock) {
	/* Poll these bits to ensure proper disabling of SPI.*/
	while(SPI1->SR & SPI_SR_RXNE) {}
	while(!(SPI1->SR & SPI_SR_TXE)){}
	while(SPI1->SR & SPI_SR_BSY) {}
	SPI1->CR1 &= ~(SPI_CR1_SPE);
	if(disable_clock) {
		/* Disable clock. */
		RCC->APB2ENR &= ~(RCC_APB2ENR_SPI1EN);
	}

	spi_initialized = 0;
} /* End of disable_spi().*/

void spi_init(void) {
	/* Configure pins to be used. */
	spi_pin_config();

	/* Enable clock for SPI1. */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	disable_spi(0);

	/* Clear previous SPI configuration to prepare for SPI SD card. */
	SPI1->CR1 = 0;

	/* SPI1 configuration. According to data sheet. */
	/* 1. Select Baud rate, but reset already select the fastest Baud rate.
	 * 2. Select CPOL and CPHA configuration: SD SPI mode requires both to be 0.
	 * 	  Reset already achieved this.
	 * 3. Set DFF to either 8- or 16-bit format. Reset already achieved this.
	 * 4. Configure LSBFIRST bit. Since SD process data MSB first, set this to 0.
	 * 	  Reset already achieved this.
	 * 5. For Software-based Slave select, set SSM bit and SSI bit to 1. */
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;

	/* 6. FRF bit if necessary to use TI protocol. Skip this for this configuration.
	 * 	  Reset already achieved this.
	 * 7. Set MSTR and SPE to select master mode and enable SPI. */
	SPI1->CR1 |= SPI_CR1_MSTR;
	SPI1->CR1 |= SPI_CR1_SPE;

	/* Additional configuration. Disables interrupts for now. Errors are sent as data tokens
	 * and need not error interrupts. */
	SPI1->CR2 = 0;

	spi_initialized = 1;
} /* End of spi_init(). */

void set_baudrate(uint8_t bd) {
	/* Disable SPI since Baud rate should not be change while communication is ongoing.*/
	disable_spi(0);

	/* Clear Baud rate control bits configuration. */
	SPI1->CR1 &= ~(SPI_CR1_BR_Msk);
	/* Set new Baud rate. */
	SPI1->CR1 |= (bd << SPI_CR1_BR_Pos);

	/* Turn SPI on. */
	SPI1->CR1 |= SPI_CR1_SPE;

	spi_initialized = 1;
} /* End of set_baudrate(). */

void spi_receive(uint8_t* buffer, uint32_t size, uint8_t dummyByte) {
	while(size) {
		/* Send dummy byte to drive clock and get response. */
		SPI1->DR = dummyByte;
		while(!(SPI1->SR & SPI_SR_RXNE)) {}
		*buffer = SPI1->DR;
		++buffer;
		--size;
	}
} /* End of spi_receive(). */

void spi_transmit(const uint8_t* data, uint32_t size) {
	uint8_t temp;
	uint32_t transmitCount = 0;

	while(transmitCount < size) {
		while(!(SPI1->SR & SPI_SR_TXE)) {}
		SPI1->DR = *data;
		++data;
		++transmitCount;
	}

	/* To ensure transmit completed properly. */
	while(!(SPI1->SR & SPI_SR_TXE)) {}
	while(SPI1->SR & SPI_SR_BSY) {}

	/* Since we are transmitting only and not receiving, RXNE bit
	 * will be set, and possibly the OVR bit as well. This must be
	 * cleared to ensure healthy state of the SPI communication.
	 * Per datasheet, the following sequence is used to clear OVR bit,
	 * which in turn also clears RXNE bit:
	 * 1. Read SPIx->DR;
	 * 2. Read SPIx->SR;
	 */
	temp = SPI1->DR;
	temp = SPI1->SR;
} /* End of spi_transmit(). */

void slave_select(spi_cs_pin_port_t port, uint8_t pin_pos) {
	switch(port) {
	case SPI_GPIO_A:
		GPIOA->ODR &= ~(1U << pin_pos);
		break;
	case SPI_GPIO_B:
		GPIOB->ODR &= ~(1U << pin_pos);
		break;
	case SPI_GPIO_C:
		GPIOC->ODR &= ~(1U << pin_pos);
		break;
	case SPI_GPIO_D:
		GPIOD->ODR &= ~(1U << pin_pos);
		break;
	case SPI_GPIO_E:
		GPIOE->ODR &= ~(1U << pin_pos);
		break;
	case SPI_GPIO_H:
		GPIOH->ODR &= ~(1U << pin_pos);
		break;

	default:
	}
} /* End of slave_select(). */

void slave_deselect(spi_cs_pin_port_t port, uint8_t pin_pos) {
	switch(port) {
		case SPI_GPIO_A:
			GPIOA->ODR |= (1U << pin_pos);
			break;
		case SPI_GPIO_B:
			GPIOB->ODR |= (1U << pin_pos);
			break;
		case SPI_GPIO_C:
			GPIOC->ODR |= (1U << pin_pos);
			break;
		case SPI_GPIO_D:
			GPIOD->ODR |= (1U << pin_pos);
			break;
		case SPI_GPIO_E:
			GPIOE->ODR |= (1U << pin_pos);
			break;
		case SPI_GPIO_H:
			GPIOH->ODR |= (1U << pin_pos);
			break;
		default:
		}
} /* End of slave_deselect(). */

uint8_t spi_init_status(void) {
	return (spi_initialized);
}

/***** API definition END. *****/
