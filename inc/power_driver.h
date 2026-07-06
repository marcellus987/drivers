/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for managing power modes in STM32F411 MCU.
 *
 * Note: Read the datasheet for more information.
 *
 * Requirements:
 * Pins used: PA0 = Wake-up pin during STANDBY mode.
 *
 * Copyright(c).
 *
 */

#ifndef POWER_DRIVER_H__
#define POWER_DRIVER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

void power_saving_init(void);
void power_deepsleep(void);
void power_sleep(void);
void power_standby(void);
void setRTC_timer(uint32_t timerDuration);
void clear_pwr_flags(void);

#endif /* POWER_DRIVER_H__ */
