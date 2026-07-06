/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for SPI Communication protocol in STM32F411 MCU.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, SPI may not
 * 		 function properly. CS pin is not determined here.
 * 		 It is up to the user which port and pin for CS pin
 * 		 to use. This is polling version and does not utilize
 * 		 DMA or Interrupts when transmitting or receiving.
 *
 * Requirements:
 * Pins used: PA5 = SCLK.
 * 			  PA6 = MISO.
 * 			  PA7 = MOSI.
 *
 * Copyright(c).
 *
 */


#ifndef SPI_H__
#define SPI_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

typedef enum {
	/* Mapping is based on RCC->AHB1ENR from Datasheet. */
	SPI_GPIO_A = 0,
	SPI_GPIO_B,
	SPI_GPIO_C,
	SPI_GPIO_D,
	SPI_GPIO_E,
	SPI_GPIO_H = 7
} spi_cs_pin_port_t;


/* API declarations START. */
void disable_spi(uint8_t disable_clock);
void spi_init(void);
uint8_t spi_init_status(void);
void set_baudrate(uint8_t bd);
void spi_transmit(const uint8_t* data, uint32_t size);
void spi_receive(uint8_t* buffer, uint32_t size, uint8_t dummyByte);
void slave_select(spi_cs_pin_port_t port, uint8_t pin_pos);
void slave_deselect(spi_cs_pin_port_t port, uint8_t pin_pos);
/* API declarations END. */


#endif /* SPI_H__ */
