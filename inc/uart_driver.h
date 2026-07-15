/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for UART Communication protocol in STM32F411 MCU.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, UART may not
 * 		 function properly. This is polling version and does
 * 		 not utilize DMA or Interrupts when transmitting or
 * 		 receiving.
 *
 * Requirements:
 * Pins used: PA2 = TX.
 * 			  PA3 = RX.
 * 			  PA10 = RX. <--- Depending on the settings.
 *
 * Copyright(c).
 *
 */


#ifndef UART_DRIVER_H__
#define UART_DRIVER_H__

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

void uart_init(uint8_t USB_to_UART);
uint8_t uart_init_status(void);
void uart_send(uint8_t *data, uint32_t len);
void uart_receive(uint8_t *buffer, uint32_t len);
void setBaudrate(uint32_t br);
#endif /* UART_DRIVER_H__ */
