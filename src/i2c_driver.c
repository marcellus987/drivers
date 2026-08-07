#include "i2c_driver.h"

#define I2C_SPEED_100KHZ  	(80U)
#define I2C_STD_MODE_TRISE	(17U)


/***** Helper functions declaration START. *****/
static void pin_config_i2c(void);
/***** Helper functions declaration END. *****/

/***** Helper functions definition START. *****/
static void pin_config_i2c(void) {

	/* Enable clock for port B. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* Set clear mode for PB6 and PB7. */
	GPIOB->MODER &= ~(GPIO_MODER_MODER6_Msk);
	GPIOB->MODER &= ~(GPIO_MODER_MODER7_Msk);

	/* Set AF mode for PB6 and PB7. */
	GPIOB->MODER |= GPIO_MODER_MODER6_1;
	GPIOB->MODER |= GPIO_MODER_MODER7_1;

	/* Select output type to Open-drain,
	 * so pins can only be actively driven LOW. */
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;

	/* Clear Pull-up selection. */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6_Msk);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7_Msk);

	/* Enable Pull-up resistor. */
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD7_0;

	/* Clear AF selection for PB6 and PB7. */
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6_Msk);
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL7_Msk);

	/* Select AF to AF4: I2C1_X. */
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2; /* PB6 = SCL. */
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2; /* PB7 = SDA. */
}
/***** Helper Function definition END. *****/

/***** API definition START. *****/

uint8_t vI2cInit(i2c_instance_t* i2cInstance, I2C_TypeDef* i2cx) {
	/* Configure PB6 and PB7. */
	pin_config_i2c();

	/***** I2C Peripheral configuration. *****/
	/* Enable clock for i2cx. */
	if(i2cx == I2C1) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(i2cx == I2C2) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if(i2cx == I2C3) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}
	else {
		i2cInstance = NULL;
		return 0;
	}

	/***** Valid I2C *****/

	/* Per datasheet, the following steps must be observed. */

	/* Force Reset I2C for clean internal logic state. */
	i2cx->CR1 |= I2C_CR1_SWRST;  /* Enter reset state. */
	i2cx->CR1 &= ~(I2C_CR1_SWRST); /* Exit reset state. */

	/* Clear CR2 FREQ bits before setting it. */
	i2cx->CR2 &= ~(I2C_CR2_FREQ_Msk);

	/* Set CR2 FREQ bits equal to APB clock frequency value (i.e. 16MHz). */
	i2cx->CR2 |= I2C_CR2_FREQ_4;

	/* Set I2C speed to 100KHz. Formula: CCR = Fpclk / (2 * I2C clk)
	 * Fpclk = Peripheral clock frequency; I2C clk = Desired speed of I2C. */
	i2cx->CCR = I2C_SPEED_100KHZ;

	/* Set TRISE. T = period. The period of APB1 clock set in CR2.
	 * So, T = 1/f (f = cycles/second). Then 1 / 16,000,000 = 62.5ns.
	 * To calculate, MAX_SCL_RISE / T = TRISE value + 1.
	 * Hence, 1000ns/62.5ns = 16 + 1 = 17*/
	i2cx->TRISE = I2C_STD_MODE_TRISE;

	/* Enable I2C.*/
	i2cx->CR1 |= I2C_CR1_PE;

	/* Configure instance. */
	i2cInstance->i2c = i2cx;
	i2cInstance->initialized = 1;
	i2cInstance->enabled = 1;

	return 1;
} /* End of i2c_init(). */

/* bytesToSend must be <= sizeof(dataBuff). */
uint8_t xI2cControllerTransmit(I2C_TypeDef* i2cx, const uint8_t address, const uint8_t* dataBuff, const int bytesToSend) {
	if((dataBuff == NULL) | (bytesToSend < 1)) {
		return 0;
	}

	uint8_t temp;

	/* Generate START condition. Pulls SDA line LOW while SCL is HIGH. */
	i2cx->CR1 |= I2C_CR1_START;

	/* Poll until SB bit is set, then clear by reading SR1 and writing to DR. */
	while(!(i2cx->SR1 & I2C_SR1_SB)) { }
	temp = i2cx->SR1;

	/* Write target address to DR. Shifted by 1 so LSB = 0; for transmitter mode. */
	i2cx->DR = (address << 1);

	/* Poll until address transmission completes. Then clear ADDR bit by reading
	 * SR1 and SR2. */
	while(!(i2cx->SR1 & I2C_SR1_ADDR)) { }
	temp = i2cx->SR1;
	temp = i2cx->SR2;

	for(int i = 0; i < bytesToSend; ++i) {
		/* Poll until TX buffer empty, then write data to TX buffer to send. */
		while(!(i2cx->SR1 & I2C_SR1_TXE)) { }
		i2cx->DR = dataBuff[i];
	}

	/* Ensure all bits have left shift register before generating STOP condition. */
	while(!(i2cx->SR1 & I2C_SR1_BTF)) { }
	/* Generate STOP condition. */
	i2cx->CR1 |= I2C_CR1_STOP;

	return 1;
} /* End of i2c_transmit(). */

/* bytesToRead must be <= sizeof(dataBuff) */
uint8_t xI2cControllerReceive(I2C_TypeDef* i2cx, const uint8_t address, uint8_t* dataBuff, const int bytesToRead) {
	if((dataBuff == NULL) | (bytesToRead < 1)) {
		return 0;
	}

	uint8_t temp;

	/* Enable ACK to send ACK each byte reception. */
	i2cx->CR1 |= I2C_CR1_ACK;

	/* Generate START condition. Pulls SDA line LOW while SCL is HIGH. */
	i2cx->CR1 |= I2C_CR1_START;

	/* Poll until SB bit is set, then clear by reading SR1 and writing to DR. */
	while(!(i2cx->SR1 & I2C_SR1_SB)) { }
	temp = i2cx->SR1;

	/* Write target address to DR. Shifted by 1 then set LSB = 1; for receiver mode. */
	i2cx->DR = (address << 1) | 0x01U;

	/* Poll until address transmission completes. */
	while(!(i2cx->SR1 & I2C_SR1_ADDR)) { }

	/* Disable ACK if only receiving 1 byte, so NACK will be sent after the first byte.
	 * This must happen before clearing ADDR bit. */
	if(bytesToRead == 1) {
		i2cx->CR1 &= ~(I2C_CR1_ACK);
	}

	/* Clear ADDR bit by reading SR1 and SR2. */
	temp = i2cx->SR1;
	temp = i2cx->SR2;

	if(bytesToRead > 1) {
		/* Received until 3rd to last byte. */
		for(int i = 0; i < bytesToRead - 2; ++i) {
			/* Poll until TX buffer empty, then write data to TX buffer to send. */
			while(!(i2cx->SR1 & I2C_SR1_RXNE)) { }
			dataBuff[i] = i2cx->DR;
		}
		/* Wait for 2nd to last byte: (N-1)th. */
		while(!(i2cx->SR1 & I2C_SR1_RXNE)) { }
		/* Clear ACK bit then Generate STOP condition since ACK has been sent at this point. */
		i2cx->CR1 &= ~(I2C_CR1_ACK);
		i2cx->CR1 |= I2C_CR1_STOP;
		dataBuff[bytesToRead - 2] = i2cx->DR;

		/* Read last byte: (N)th. Sending NACK */
		while(!(i2cx->SR1 & I2C_SR1_RXNE)) { }
		dataBuff[bytesToRead - 1] = i2cx->DR;
	}
	else { /* bytesToRead == 1. */
		/* Generate STOP condition. */
		i2cx->CR1 |= I2C_CR1_STOP;

		/* Read 1 byte. Sends NACK. */
		while(!(i2cx->SR1 & I2C_SR1_RXNE)) { }
		dataBuff[bytesToRead - 1] = i2cx->DR;
	}

	return 1;
} /* End of i2c_receive(). */

void vDisableI2c(i2c_instance_t* i2cInstance, uint8_t disableClock) {
	if(i2cInstance->initialized) {
		I2C_TypeDef* i2cx = i2cInstance->i2c;

		/* Ensure no bus us not busy. */
		while(i2cx->SR2 & I2C_SR2_BUSY) { }
		/* Disable I2C.*/
		i2cx->CR1 &= ~I2C_CR1_PE;
		i2cInstance->enabled = 0;

		/* Disable clock. Makes i2c uninitialized. */
		if(disableClock) {
			/* Enable clock for i2cx. */
			if(i2cx == I2C1) {
				RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			}
			else if(i2cx == I2C2) {
				RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
			}
			else { /* I2C3. */
				RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
			}
			i2cInstance->initialized = 0;
		}
	}
} /* End of vDisableI2c(). */

/***** API definition END. *****/
