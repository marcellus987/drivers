#include "mail_notification_driver.h"
#include "llcc68_driver.h"
#include "power_driver.h"
#include "rtc_driver.h"
#include "ir_beam_driver.h"
#include "systick.h"

#define TRANSMITTER_DEVICE__
#define RECEIVER_DEVICE__


/* Initialize radio and pins to be used. */
void send_notification(const uint8_t state) {
	if(!LLCC68_init_status()) {
		LLCC68_init();
	}
	setTx(&state, sizeof(state));
}

void receive_notification(uint8_t* state) {
	if(!LLCC68_init_status()) {
			LLCC68_init();
	}
	setRx(state);
}

uint8_t verify_mail_presence(void) {
	uint8_t sensor_status;

	ir_beam_init();
	ir_beam_on();

	delay_ms(100); /* Delay to get stable reading from receiver sensor. */

	/* 1 = Unbroken: No mail. 0 = Broken: Mail present. */
	sensor_status = check_sensor_status();
	ir_beam_off();

	/* Must be inverted since a broken beam means mail is present assuming sensor is functioning as normally. */
	return (!sensor_status);
}

/* Goes to sleep for a duration until action is done.
 * This action is either mail being placed inside the mailbox
 * or mail being taken out of the mailbox. */
void wait_for_action_done(uint32_t duration) {
	rtc_init();
	setRTC_timer(duration);
	power_deepsleep();
}

void set_standby_mode(void) {
	power_standby();
}

uint8_t check_standby_flag(void) {
	uint8_t standbyFlag;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	standbyFlag = PWR->CSR & PWR_CSR_SBF ? 1 : 0;
	RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
	return (standbyFlag);
}


void init_debug_mode(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; /* For on-board LED. */

	/* Reset mode configurations for port C. */
	GPIOC->MODER &= ~(GPIO_MODER_MODER13_Msk);

	/* Configure output mode for port C LED pin. */
	GPIOC->MODER |= GPIO_MODER_MODER13_0;
	debug_led_off();
}

void toggle_led(void) {
	GPIOC->ODR ^= GPIO_ODR_OD13;
}


void debug_led_on(void) {
	GPIOC->BSRR |= GPIO_BSRR_BR13;
}

void debug_led_off(void) {
	GPIOC->BSRR |= GPIO_BSRR_BS13;
}

void reset_low_power_mode_flags(void) {
	clear_pwr_flags();
	clear_rtc_wutf();
}

#ifdef RECEIVER_DEVICE__

void received_led_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* For on-board LED. */

	/* Reset mode configurations for port B. */
	GPIOB->MODER &= ~(GPIO_MODER_MODER12_Msk);
	/* Configure output mode for port B LED pin. */
	GPIOB->MODER |= GPIO_MODER_MODER12_0;
}


void received_led_on(void) {
	GPIOB->BSRR |= GPIO_BSRR_BS12;
}

void received_led_off(void) {
	GPIOB->BSRR |= GPIO_BSRR_BR12;
}

#endif /* RECEIVER_DEVICE__. */



