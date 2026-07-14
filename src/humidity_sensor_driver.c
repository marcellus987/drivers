#include <humidity_sensor_driver.h>
#include "timer.h"

#define DATA_PIN 0 /* PA0 */
#define BIT_COUNT 40 /* Per datasheet, sensor transmit 40-bit data back to master MCU. */

/* State of each communication phase.*/
typedef enum {
	WAITING_RESPONSE_LOW,
	RESPONSE_LOW,
	RESPONSE_HIGH,
	DATA_LOW,
	DATA_HIGH,
} State;


/* API definition START. */
void dht22_sensor_init(void) {
	timer_config();
	/* Enable clock for port A. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
} /* End of humidity_init(). */


uint8_t dht22_sensor_read(raw_dht22_data_t* data) {
	uint32_t startTime;
	State state;
	*data = (raw_dht22_data_t){0};
	uint8_t *curByte = (uint8_t*)data;
	int bitCount = 0;

	uint32_t timeout;

	/* Configure PA0 as Output mode. */
	GPIOA->MODER |= (1U << 0);
	GPIOA->MODER &= ~(1U << 1);

	/* Pull-down line to initiate communication with sensor. */
	GPIOA->BSRR |= (1 << (DATA_PIN + 16));
	delay_ms(10);

	/* Configure PA0 as Input mode. */
	GPIOA->MODER &= ~(1U << 0);
	GPIOA->MODER &= ~(1U << 1);

	timDelay_us(40);

	state = WAITING_RESPONSE_LOW;

	while(bitCount < BIT_COUNT) {
		timeout = TIM2->CNT;
		if(GPIOA->IDR & (1U << DATA_PIN)) {
			switch(state) {
			case RESPONSE_LOW:
				state = RESPONSE_HIGH;
				break;
			case DATA_LOW:
				startTime = TIM2->CNT;
				state = DATA_HIGH;
				break;
			default:
				while(GPIOA->IDR & (1U << DATA_PIN)) {
					if(TIM2->CNT - timeout > 100) {
						return 0;
					}
				}
			}
		}
		else {
			switch(state) {
			case WAITING_RESPONSE_LOW:
				state = RESPONSE_LOW;
				break;
			case RESPONSE_HIGH:
				state = DATA_LOW;
				break;
			case DATA_HIGH:
				uint32_t time = TIM2->CNT - startTime;
				uint8_t bit = (time > 40);
				curByte[bitCount >> 3] <<= 1;
				curByte[bitCount >> 3] |= bit;
				++bitCount;
				state = DATA_LOW;
				break;
			default:
				while(!(GPIOA->IDR & (1U << DATA_PIN))) {
					if(TIM2->CNT - timeout > 100) {
						return 0;
					}
				}
			}
		}
	}

	return 1;
} /* End of sensor_read(). */

/* API definition END. */
