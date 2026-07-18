#include "spi_driver.h"
#include <stdio.h>

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


/* Unused for now.
static uint8_t isValidSpi(SPI_TypeDef *spix) {
	switch((uint32_t)spix) {
	case((uint32_t)SPI1):
	case((uint32_t)SPI2):
	case((uint32_t)SPI3):
	case((uint32_t)SPI4):
	case((uint32_t)SPI5):
		return  1;
	default:
		return 0;
	}
}
*/
/***** Helper functions definition END. *****/


/***** API definition START. *****/
void vDisableSpi(spi_instance_t *spi_instance, uint8_t disable_clock) {
	if(spi_instance->initialized) {
		SPI_TypeDef *spix = spi_instance->spi;

		/* Poll these bits to ensure proper disabling of SPI.*/
		while(spix->SR & SPI_SR_RXNE) {
			uint8_t temp = spix->DR;
		}
		while(!(spix->SR & SPI_SR_TXE)){}
		while(spix->SR & SPI_SR_BSY) {}
		spix->CR1 &= ~(SPI_CR1_SPE);
		spi_instance->enabled = 0;

		/* Disable clock for SPIx, if necessary. Makes SPI uninitialized. */
		if(disable_clock) {
			if(spix == SPI1) {
				RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
			}
			else if(spix == SPI4) {
				RCC->APB2ENR &= ~RCC_APB2ENR_SPI4EN;
			}
			else if(spix == SPI5) {
				RCC->APB2ENR &= ~RCC_APB2ENR_SPI5EN;
			}
			else if(spix == SPI2) {
				RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
			}
			else {
				RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
			}
			spi_instance->initialized = 0;
		}
	}
} /* End of disable_spi().*/

uint8_t xSpiInit(spi_instance_t *spi_instance, SPI_TypeDef *spix) {
	/* Configure pins to be used.
	 * Hard coded for now.
	 * */
	spi_pin_config();

	/* Enable clock for SPIx. */
	if(spix == SPI1) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	}
	else if(spix == SPI4) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
	}
	else if(spix == SPI5) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}
	else if(spix == SPI2) {
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	}
	else if(spix == SPI3) {
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}
	else { /***** Invalid SPI. *****/
		spi_instance->spi = NULL;
		return 0;
	}

	/***** Valid SPI. *****/

	/* Per datasheet, the following steps must be observed. */

	/* Disable SPI. */
	while(spix->SR & SPI_SR_RXNE) {}
	while(!(spix->SR & SPI_SR_TXE)){}
	while(spix->SR & SPI_SR_BSY) {}
	spix->CR1 &= ~(SPI_CR1_SPE);

	/* Clear previous SPI configuration to prepare for SPI SD card. */
	spix->CR1 = 0;

	/* SPI1 configuration. According to data sheet. */
	/* 1. Select Baud rate, but reset already select the fastest Baud rate.
	 * 2. Select CPOL and CPHA configuration: SD SPI mode requires both to be 0.
	 * 	  Reset already achieved this.
	 * 3. Set DFF to either 8- or 16-bit format. Reset already achieved this.
	 * 4. Configure LSBFIRST bit. Since SD process data MSB first, set this to 0.
	 * 	  Reset already achieved this.
	 * 5. For Software-based Slave select, set SSM bit and SSI bit to 1. */
	spix->CR1 |= SPI_CR1_SSM;
	spix->CR1 |= SPI_CR1_SSI;

	/* 6. FRF bit if necessary to use TI protocol. Skip this for this configuration.
	 * 	  Reset already achieved this.
	 * 7. Set MSTR and SPE to select master mode and enable SPI. */
	spix->CR1 |= SPI_CR1_MSTR;
	spix->CR1 |= SPI_CR1_SPE;

	/* Additional configuration. Disables interrupts for now. Errors are sent as data tokens
	 * and need not error interrupts. */
	spix->CR2 = 0;

	/* Configure instance. */
	spi_instance->spi = spix;
	spi_instance->initialized = 1;
	spi_instance->enabled = 1;
} /* End of spi_init(). */

void vSetBaudrate(spi_instance_t *spi_instance, uint8_t bd, uint8_t keepEnabled) {
	if(spi_instance->initialized) {
		SPI_TypeDef *spix = spi_instance->spi;

		if(spi_instance->enabled) {
			/* Poll these bits to ensure proper disabling of SPI.*/
			while(spix->SR & SPI_SR_RXNE) {}
			while(!(spix->SR & SPI_SR_TXE)){}
			while(spix->SR & SPI_SR_BSY) {}
			spix->CR1 &= ~(SPI_CR1_SPE);
			spi_instance->enabled = 0;
		}

		/* Clear Baud rate control bits configuration. */
		spix->CR1 &= ~(SPI_CR1_BR_Msk);
		/* Set new Baud rate. */
		spix->CR1 |= (bd << SPI_CR1_BR_Pos);

		if(keepEnabled) {
			/* Turn SPI on. */
			spix->CR1 |= SPI_CR1_SPE;
			spi_instance->enabled = 1;
		}
	}
} /* End of set_baudrate(). */

void vSpiReceive(SPI_TypeDef *spix, uint8_t* buffer, uint32_t size, uint8_t dummyByte) {
	while(size) {
		/* Send dummy byte to drive clock and get response. */
		spix->DR = dummyByte;
		while(!(spix->SR & SPI_SR_RXNE)) {}
		*buffer = spix->DR;
		++buffer;
		--size;
	}
} /* End of spi_receive(). */

void vSpiTransmit(SPI_TypeDef *spix, const uint8_t* data, uint32_t size) {
	uint8_t temp;
	uint32_t transmitCount = 0;

	while(transmitCount < size) {
		while(!(spix->SR & SPI_SR_TXE)) {}
		spix->DR = *data;
		++data;
		++transmitCount;
	}

	/* To ensure transmit completed properly. */
	while(!(spix->SR & SPI_SR_TXE)) {}
	while(spix->SR & SPI_SR_BSY) {}

	/* Since we are transmitting only and not receiving, RXNE bit
	 * will be set, and possibly the OVR bit as well. This must be
	 * cleared to ensure healthy state of the SPI communication.
	 * Per datasheet, the following sequence is used to clear OVR bit,
	 * which in turn also clears RXNE bit:
	 * 1. Read SPIx->DR;
	 * 2. Read SPIx->SR;
	 */
	temp = spix->DR;
	temp = spix->SR;
} /* End of spi_transmit(). */

void vSlaveSelect(GPIO_TypeDef *gpiox, uint8_t pin_pos) {
	gpiox->ODR &= ~(1U << pin_pos);
} /* End of slave_select(). */

void vSlaveDeselect(GPIO_TypeDef *gpiox, uint8_t pin_pos) {
	gpiox->ODR |= (1U << pin_pos);
} /* End of slave_deselect(). */

/***** API definition END. *****/
