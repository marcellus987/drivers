/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for I2C Communication protocol in STM32F411 MCU.
 *
 * Note: This is polling version and does not utilize DMA or Interrupts
 * 		 when transmitting or receiving.
 *
 * Copyright(c).
 *
 */


#ifndef I2C_DRIVER_H__
#define I2C_DRIVER_H__

#include "stm32f4xx.h"
#include <stdio.h>


typedef struct {
	I2C_TypeDef* i2c;
	uint8_t initialized;
	uint8_t enabled;
} i2c_instance_t;

/*******************************************************************
 * Usage: Initializes I2C instance upon success. Otherwise, instance
 * 		  points to nothing (NULL); therefore, undefined.
 * Param: i2c instance, i2c peripheral address.
 * RetVal: 1 on Success. 0 on failure.
 * *****************************************************************/
uint8_t vI2cInit(i2c_instance_t* i2cInstance, I2C_TypeDef* i2cx);

/*******************************************************************************
 * Usage: I2C must be initialized and enabled. Transmits data from controller
 * 		  to target.
 * Param: i2c peripheral address, target address, data to be sent, size of data.
 * RetVal: 1 on Success. 0 on failure.
 * *****************************************************************************/
uint8_t xI2cControllerTransmit(I2C_TypeDef* i2cx, const uint8_t address, const uint8_t* dataBuff, const int bytesToSend);

/*******************************************************************************
 * Usage: I2C must be initialized and enabled. Receives data from target to
 * 		  controller.
 * Param: i2c peripheral address, target address, data buffer, size of data.
 * RetVal: 1 on Success. 0 on failure.
 * *****************************************************************************/
uint8_t xI2cControllerReceive(I2C_TypeDef* i2cx, const uint8_t address, uint8_t* dataBuff, const int bytesToRead);


/*******************************************************************************
 * Usage: Disables I2C peripheral. Optionally, disables the clock for it,
 * 		  making it uninitialized.
 * Param: i2c instance, clock disable flag.
 * RetVal: None.
 * *****************************************************************************/
void vDisableI2c(i2c_instance_t* i2cInstance,  uint8_t disableClock);


#endif /* I2C_DRIVER_H__ */
