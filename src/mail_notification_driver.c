#include "mail_notification_driver.h"
#include "llcc68_driver.h"
#include "power_driver.h"
#include "rtc_driver.h"
#include "ir_beam_driver.h"
#include "systick.h"

#define TRANSMITTER_DEVICE__
#define RECEIVER_DEVICE__

#define MAIL_PRESENT_LED 12
#define MAIL_ABSENT_LED 13
#define SD_ERROR_LED 14
#define SD_OK_LED 15




/* Initialize radio and pins to be used. */
void send_notification(const uint8_t* state) {
	if(!LLCC68_init_status()) {
		LLCC68_init();
	}
	setTx(state, sizeof(*state));
}

void receive_notification(uint8_t* state) {
	if(!LLCC68_init_status()) {
			LLCC68_init();
	}
	setRx(state);
}

void send_mail_present(void) {
	uint8_t temp = 0x01U;
	send_notification(&temp);
}

void send_mail_abesent(void) {
	uint8_t temp = 0x00U;
	send_notification(&temp);
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
	setWakeupTimer(duration);
	power_deepsleep();
}

void set_standby_mode(void) {
	power_standby(1);
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

void configure_indicator_led(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* For on-board LED. */

	/* Reset mode configurations for port B. */
	GPIOB->MODER &= ~(GPIO_MODER_MODER12_Msk | GPIO_MODER_MODER13_Msk |GPIO_MODER_MODER14_Msk | GPIO_MODER_MODER15_Msk);
	/* Configure output mode for port B LED pin. */
	GPIOB->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
}

//#define MAIL_PRESENT_LED 12
//#define MAIL_ABSENT_LED 13
//#define SD_ERROR_LED 14
//#define SD_OK_LED 15
void mail_present_led_on(void) {
	GPIOB->BSRR |= (1U << MAIL_PRESENT_LED) | (1U << (MAIL_ABSENT_LED + 16));
}
void mail_absent_led_on(void) {
	GPIOB->BSRR |= (1U << (MAIL_PRESENT_LED + 16)) | (1U << MAIL_ABSENT_LED);
}
void sd_error_led_on(void) {
	GPIOB->BSRR |= (1U << SD_ERROR_LED) | (1U << (SD_OK_LED + 16));
}
void sd_ok_led_on(void) {
	GPIOB->BSRR |= (1U << (SD_ERROR_LED + 16)) | (1U << SD_OK_LED);
}

void led_indicator_off(void) {
	GPIOB->BSRR |= (1U << (SD_ERROR_LED + 16)) | (1U << (SD_OK_LED + 16)) | (1U << (MAIL_PRESENT_LED + 16)) | (1U << (MAIL_ABSENT_LED + 16));
}

#endif /* RECEIVER_DEVICE__. */
