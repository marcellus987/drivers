/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for utilizing RTC feature of STM32F411 MCU.
 *
 * Note: Read the datasheet for more information.
 *
 * Copyright(c).
 *
 */


#ifndef RTC_DRIVER_H__
#define RTC_DRIVER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"


void rtc_init(void);
void setWakeupTimer(uint32_t timerDuration);

void disable_wakeup_timer(void);
void clear_rtc_wutf(void);

#endif /* RTC_DRIVER_H__ */
