/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for STM32F411 MCU with LCD1602 module.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, LCD may not
 * 		 function properly. Read datasheet for more information.
 * 		 Datasheet link: "https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/5773/CN0295D%20other%20related%20document.pdf"
 *
 * Requirements:
 * Pins used: PC0 through PC7 = Data line.
 * 			  PC8 = RS: for selecting which register to write.
 * 			  PC9 = E: enable signal for write/read.
 *
 * Copyright(c).
 *
 */


#ifndef LCD_H__
#define LCD_H__
#include "stm32f4xx.h"
#include "timer.h"
#include "systick.h"
#include <stdint.h>

void send_command(uint8_t command);
void send_data(uint8_t data);
void lcd_init(void);
void move_cursor(uint8_t location);
void cls(void);

#endif /* LCD_H__ */
