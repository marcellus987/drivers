/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for Mail Notification Device Project.
 *
 * Note: See ".h" files included in the "mail_notification_driver.c" file
 * 		 for pin-out.
 *
 * Copyright(c).
 *
 */

#ifndef MAIL_NOTIFICATION_DRIVER_H__
#define MAIL_NOTIFICATION_DRIVER_H__
#include <stdint.h>
#include "stm32f4xx.h"


/* FLOW:
 * if(!standbyFlag) {
 * 		set standbyMode;
 * }
 * else {
 * 		set deepsleep for 1 min.
 * 		turn on ir beam sesnro and pulse three times.
 * 		if(broken) mail is present in mailbox {
 * 			send Notification;
 * 			turn off IR beam sensor,
 * 			set standby mode.
 * 		}
 * 		else {
 * 			Mail is absent from mailbox.
 * 			set to standby mode.
 * 		}
 * }
 *
 *
 *
 *
 * */

typedef enum {
	MAIL_ABSENT = 0,
	MAIL_PRESENT,
	MAIL_TAKEN
} mail_status_t;

#define WAKE_UP_PIN_Pos (0U)
#define USER_LED_Pos (13U)


#define MAIL_PRESENT (0x01U)
#define MAIL_ABSENT (0x00U)


void wait_for_action_done(uint32_t duration);
uint8_t verify_mail_presence(void);
void send_notification(uint8_t state);

void set_standby_mode(void);
uint8_t check_standby_flag(void);
void reset_low_power_mode_flags(void);

void init_debug_mode(void);
void debug_led_on(void);
void debug_led_off(void);
void toggle_led(void);

void received_led_init(void);
void received_led_on(void);
void received_led_off(void);


#endif /* MAIL_NOTIFICATION_DRIVER_H__ */
