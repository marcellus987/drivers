/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for STM32F411 MCU with DHT22(AM2302) temperature-humidity sensor module.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, sensor module may not
 * 		 function properly. Read datasheet for more information.
 *
 * Requirements:
 * Pins used: PA0 = Data line.
 *
 *
 * Copyright(c).
 *
 */


#ifndef HUMIDITY_SENSOR_H__
#define HUMIDITY_SENSOR_H__



#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "timer.h"
#include "systick.h"

/* Structure of data from sensor. */
typedef struct {
	uint8_t raw_humidityInt;
	uint8_t raw_humidityDec;
	uint8_t raw_tempInt;
	uint8_t raw_tempDec;
	uint8_t raw_checksum;
} raw_dht22_data_t;

/* Structure of data from sensor. */
typedef struct {
	int humidityInt;
	int humidityDec;
	int tempInt;
	int tempDec;
} dht22_data_t;

//extern raw_dht22_data_t data;

void dht22_sensor_init(void);
uint8_t dht22_sensor_read(raw_dht22_data_t* data);
//void sensor_read(void);
//void sensor_read(sensor_data_t* data);
void process(void);

extern uint32_t debugTime[40];

#endif /* HUMIDITY_SENSOR_H__ */
