/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for TIMER peripheral in STM32F411 MCU.
 *
 * Note: This is polling version and does not utilize interrupts.
 *
 * Copyright(c).
 *
 */


#ifndef TIMER_H__
#define TIMER_H__

#include <stdint.h>
#include "stm32f4xx.h"

void timer_config(void);
void timer_start(void);
void timer_stop(void);
void timer_reset(void);
uint32_t getTime(void);
void timDelay_us(uint32_t delay);

#endif /* TIMER_H__ */
