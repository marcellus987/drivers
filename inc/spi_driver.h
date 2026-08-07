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

/* Structure of SPI instance. */
typedef struct {
	SPI_TypeDef *spi;
	uint8_t initialized;
	uint8_t enabled;
} spi_instance_t;


/***** API declaration END. *****/

/*******************************************************************
 * Usage: Initializes SPI instance upon success. Otherwise, instance
 * 		  points to nothing (NULL); therefore, undefined.
 * Param: spi instance, SPI peripheral address.
 * RetVal: 1 on success. 0 on failure.
 * *****************************************************************/
uint8_t xSpiInit(spi_instance_t *spiInstance, SPI_TypeDef *spix);

/*******************************************************************
 * Usage: SPI must be initialized before invoking this function.
 * 		  Otherwise, it does nothing. If RCC clock disable flag
 * 		  is set then SPI becomes uninitialized. Otherwise, it
 * 		  only becomes disabled but registers contents are preserved.
 * Param: spi instance, SPI RCC clock disable flag.
 * RetVal: None.
 * *****************************************************************/
void vDisableSpi(spi_instance_t *spiInstance, uint8_t disable_clock);

/*******************************************************************
 * Usage: SPI must be initialized. Automatically disables the SPI for
 * 		  baudrate re-configuraton. Enables it depending on the enable
 * 		  flag passed to this function.
 * Param: spi instance, desired baudrate, enable flag.
 * RetVal: None.
 * *****************************************************************/
void vSetBaudrate(spi_instance_t *spiInstance, uint8_t bd, uint8_t keepEnabled);


/*********************************************************************
 * Usage: SPI must be initialized. Transmit data to SPI slave device.
 * Param: SPIx used, data to be sent, size of data to be sent.
 * RetVal: None.
 * *******************************************************************/
void vSpiTransmit(SPI_TypeDef *spix, const uint8_t *data, uint32_t size);

/****************************************************************************
 * Usage: SPI must be initialized. Receive data from SPI slave device.
 * Param: SPIx used, data buffer, size of data to be received, dummy byte
 * 		  since some SPI device requires certain dummy byte during reception.
 * RetVal: None.
 * **************************************************************************/
void vSpiReceive(SPI_TypeDef *spix, uint8_t *buffer, uint32_t size, uint8_t dummyByte);

/*********************************************************************
 * Usage: SPI must be initialized. Selecting slave. Pulls CS line low.
 * Param: GPIO port, CS pin position.
 * *******************************************************************/
void vSlaveSelect(GPIO_TypeDef *gpiox, uint8_t pin_pos);

/**********************************************************************
 * Usage: SPI must be initialized. Selecting slave. Pulls CS line high.
 * Param: GPIO port, CS pin position.
 * RetVal: None.
 * ********************************************************************/
void vSlaveDeselect(GPIO_TypeDef *gpiox, uint8_t pin_pos);
/***** API declaration END. *****/


#endif /* SPI_DRIVER_H__ */
