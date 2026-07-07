#ifndef HUMIDITY_SENSOR_H__
#define HUMIDITY_SENSOR_H__

#define BIT_COUNT 40

#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "timer.h"
#include "systick.h"

/* Structure of data from sensor. */
typedef struct {
	uint8_t humidityInt;
	uint8_t humidityDec;
	uint8_t tempInt;
	uint8_t tempDec;
	uint8_t checksum;
} SensorData;

extern SensorData data;

void humidity_init(void);
uint8_t sensor_read(SensorData* data);
//void sensor_read(void);
//void sensor_read(SensorData* data);
void process(void);

extern uint32_t debugTime[40];

#endif /* HUMIDITY_SENSOR_H__ */
