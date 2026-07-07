/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for interfacing STM32F411 MCU with DS3231/DS1307 RTC module.
 *
 * Note: The pins used here are arbitrary but follows the I2C pin-out as stated
 * 		 in the "i2c.h" header file, so you must follow whatever pin is used here.
 * 		 Otherwise, RTC module may not function properly.
 *
 * Requirements:
 * Pins used: PB6 = SCL.
 * 			  PB7 = SDA.
 *
 * Copyright(c).
 *
 */


#ifndef RTC_I2C_DRIVER__
#define RTC_I2C_DRIVER__
#include "stm32f4xx.h"
#include "i2c.h"

typedef enum {
	MONDAY = 1,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY /* SUNDAY = 7*/
} DayOfTheWeek;

/* Member variables are in the order listed in data sheet: Register Map.*/
typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} raw_time_t;

/* Width of attributes of time_t is consistent with raw time_t.
 * I.e. each max value of attribute is less than 255, which is
 * the max value that fits in uint8_t. So, this can be 'typedef'. */
typedef raw_time_t current_time_t;

/* Member variables are in the order listed in data sheet: Register Map.*/
typedef struct {
	uint8_t dayOfWeek;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} raw_date_t;

typedef struct {
	uint8_t dayOfWeek;
	uint8_t date;
	uint8_t month;
	uint32_t year; /* year could be > 255, which is max value that fits in uint8_t. */
} current_date_t;

void rtc_module_init(void);
uint8_t rtc_module_init_status(void);
void getCurrentTime(current_time_t* curTime);
void getCurrentDate(current_date_t* curDate);
void setCurrentTime(raw_time_t curTime, uint8_t twenty_four_hr_format, uint8_t isPM);
void setCurrentDate(raw_date_t curDate);
void setAMPM(uint8_t isPM);
void setTimeFormat(uint8_t twenty_four_hr_format);
void getTimeStamp(char* dest);
void printFormattedDateTime(current_time_t time, const current_date_t date, uint8_t selection);
#endif /* RTC_I2C_DRIVER__ */
