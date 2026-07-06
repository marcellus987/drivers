/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for interfacing STM32F411 MCU with RYLR689 LORA module.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, the module
 * 		 may not  function properly. Read the RYLR689 datasheet
 * 		 for more information.
 *
 * Requirements:
 * Pins used:
 * 		Port A:
 * 		    For SPI:
 * 				PA5 = SCLK
 * 				PA6 = MISO
 * 				PA7 = MOSI
 * 				PA8 = NSS_PIN_POS <-- This is the CS pin. Change this as needed since it is not
 * 									  restricted by SPI definition as stated from "spi.h" header file.
 * 		Port B.
 *			PB0 = RFSW_V1_PIN_POS
 *			PB1 = RFSW_V2_PIN_POS
 *			PB5 = BUSY_PIN_POS
 *			PB6 = IRQ_PIN_POS
 *			PB8 = NRESET_PIN_POS
 *	 	Port C.
 *			PC13 = USER_LED_PIN_POS
 *
 * Copyright(c).
 *
 */


#ifndef LLCC68_DRVIER_H__
#define LLCC68_DRVIER_H__
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "llcc68_defines.h"
#include "systick.h"
#include "spi.h"
#include "uart_driver.h"

/* Port A. */
#define NSS_PIN_POS 		(8U)

/* Port B. */
#define RFSW_V1_PIN_POS 	(0U)
#define RFSW_V2_PIN_POS 	(1U)
#define BUSY_PIN_POS 		(5U)
#define IRQ_PIN_POS 		(6U)
#define NRESET_PIN_POS		(8U)

/* Port C. */
#define USER_LED_PIN_POS 	(13U)

/* Initialize radio and common parameters between transmitter and receiver.*/
void LLCC68_init(void);
uint8_t LLCC68_init_status(void);

/* Status read commands. */
void readRegister(/*uint16_t address,*/ uint8_t* buffer);
uint8_t getRSSI_Inst(void);
void getPacketStatus(uint8_t *result);


/* Send/Receive commands. */
void setRx(uint8_t *buffer);
void setTx(uint8_t *buffer, uint32_t len);


#endif /* LLCC68_DRVIER_H__ */
