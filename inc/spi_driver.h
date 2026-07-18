/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for SPI Communication protocol in STM32F411 MCU.
 *
 * Note: This is polling version and does not utilize
 * 		 DMA or Interrupts when transmitting or receiving.
 *
 * Copyright(c).
 *
 */

#ifndef SPI_DRIVER_H__
#define SPI_DRIVER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"


typedef struct {
	SPI_TypeDef *spi;
	uint8_t initialized;
	uint8_t enabled;
} spi_instance_t;


/* API declarations START. */

/*******************************************************************
 * Usage: Initializes SPI instance upon success. Otherwise, it does
 * 		  instance points to nothing (NULL); therefore, undefined.
 * Param: spi instance, SPI address.
 * *****************************************************************/
uint8_t xSpiInit(spi_instance_t *spi_instance, SPI_TypeDef *spix);


/*******************************************************************
 * Usage: SPI must be initialized before invoking this function.
 * 		  Otherwise, it does nothing. If RCC clock disable flag
 * 		  is set then SPI becomes uninitialized. Otherwise, it
 * 		  only becomes disabled but registers contents are preserved.
 * Param: spi instance, SPI RCC clock disable flag.
 * *****************************************************************/
void vDisableSpi(spi_instance_t *spi_instance, uint8_t disable_clock);

					/*******************************************************************
					 * Usage: SPI must be initialized. Automatically disables the SPI for
					 * 		  baudrate re-configuraton. Enables it depending on the enable
					 * 		  flag passed to this function.
					 * Param: spi instance, desired baudrate, enable flag.
					 * *****************************************************************/
void vSetBaudrate(spi_instance_t *spi_instance, uint8_t bd, uint8_t keepEnabled);


/*******************************************************************
 * Usage: SPI must be initialized. Automatically disables the SPI for
 * 		  baudrate re-configuraton. Enables it depending on the enable
 * 		  flag passed to this function.
 * Param: spi instance, desired baudrate, enable flag.
 * *****************************************************************/
void vSpiTransmit(SPI_TypeDef *spix, const uint8_t *data, uint32_t size);
void vSpiReceive(SPI_TypeDef *spix, uint8_t *buffer, uint32_t size, uint8_t dummyByte);
void vSlaveSelect(GPIO_TypeDef *gpiox, uint8_t pin_pos);
void vSlaveDeselect(GPIO_TypeDef *gpiox, uint8_t pin_pos);
/* API declarations END. */


#endif /* SPI_DRIVER_H__ */
