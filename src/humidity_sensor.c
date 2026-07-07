#include "humidity_sensor.h"
#include "timer.h"

#define DATA_PIN 0

typedef enum {
	WAITING_RESPONSE_LOW,
	RESPONSE_LOW,
	RESPONSE_HIGH,
	DATA_LOW,
	DATA_HIGH,
} State;

void humidity_init(void) {
	timer_config();
	/* Enable clock for port A. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;
	RCC->APB2ENR |= (1U << 14);

	/* Select source of interrupt: Port A Pin 0. */
	SYSCFG->EXTICR[0] &= ~(0x000FU);
}


uint8_t sensor_read(SensorData* data) {
	uint32_t startTime;
	State state;
	*data = (SensorData){0};
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
						return 1;
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
						return 1;
					}
				}
			}
		}
	}

	return 0;
}
