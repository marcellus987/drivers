/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for interfacing STM32F411 MCU with IR beam sensor module.
 *
 * Note: Read the datasheet for more information.
 *
 * Requirements:
 * Pins used:
 * 		PB12 = to briefly power IR beam transmitter.
 * 		PB13 = to briefly power IR beam receiver.
 *
 * Copyright(c).
 *
 */


#ifndef IR_BEAM_DRIVER_H__
#define IR_BEAM_DRIVER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

void ir_beam_init(void);
uint8_t ir_beam_init_status(void); /* To be implemented. */
uint8_t check_sensor_status(void);

void ir_beam_on(void);
void ir_beam_off(void) ;

#endif /* IR_BEAM_DRIVER_H__ */
