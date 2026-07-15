/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for I2C Communication protocol in STM32F411 MCU.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, I2C may not
 * 		 function properly.This is polling version and does
 * 		 not utilize DMA or Interrupts when transmitting or
 * 		 receiving.
 *
 * Requirements:
 * Pins used: PB6 = SCL.
 * 			  PB7 = SDA.
 *
 * Copyright(c).
 *
 */


#ifndef I2C_H__
#define I2C_H__

#include "stm32f4xx.h"
#include <stdio.h>

void i2c_init(void);
uint8_t i2c_init_status(void);
uint8_t i2c_controller_transmit(const uint8_t address, const uint8_t* dataBuff, const int bytesToRead);
uint8_t i2c_controller_receive(const uint8_t address, uint8_t* dataBuff, const int bytesToRead);

#endif /* I2C_H__ */


