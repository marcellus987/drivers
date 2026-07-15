/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for systick feature in STM32F411 MCU.
 *
 * Note: This is polling version and does not utilize interrupts.
 *
 * Copyright(c).
 *
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

void delay_ms(uint32_t delay);

#endif /* _SYSTICK_H_ */
