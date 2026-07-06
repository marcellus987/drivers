/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for interfacing STM32F411 MCU with SD-card module.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, SD-card module
 * 		 may not function properly. The module uses SPI, therefore
 * 		 it follows the pin-out for SPI defined in "spi.h".
 * 		 The CS pin determined here. It is up to the user to select
 * 		 which GPIO port and pin to use as CS pin. This is polling version
 * 		 and does not utilize DMA or Interrupts when reading or writing
 * 		 to SD-card.
 *
 * 		 For more information, read SD-CARDS SPEC.: PHYSICAL LAYER
 * 		 documentation. I couldn't find the whole document as most search
 * 		 result hides the document behind a membership paywall. Only the
 * 		 simplified version is free, which is what I used here.
 *
 * 		 Finally, this low-level API will be called by FATFS Library, which
 * 		 is what the user will use in high-level to interface with SD-cards.
 *
 * Requirements:
 * Pins used: PA5 = SCLK.
 * 			  PA6 = MISO.
 * 			  PA7 = MOSI.
 * 			  PA5 = CS. <-- Change this as needed.
 *
 *
 * Copyright(c).
 *
 */

/* Author: Marcellus Von Sacramento
 *
 * Purpose: API for SPI Communication protocol in STM32F411 MCU.
 *
 * Note: The pins used here are arbitrary, you must follow
 * 		 whatever pin is used here. Otherwise, SPI may not
 * 		 function properly. CS pin is not determined here.
 * 		 It is up to the user which port and pin for CS pin
 * 		 to use. This is polling version and does not utilize
 * 		 DMA or Interrupts when transmitting or receiving.
 *
 * Requirements:
 * Pins used: PA5 = SCLK.
 * 			  PA6 = MISO.
 * 			  PA7 = MOSI.
 *
 * Copyright(c).
 *
 */





#ifndef SD_CARD_DRIVER_H__
#define SD_CARD_DRIVER_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "spi.h"
#include "systick.h"


uint8_t sd_init(void);
uint8_t sd_read(uint32_t sector, uint8_t* buffer);
uint8_t sd_write(uint32_t sector, const uint8_t* buffer);

uint8_t get_SDstatus(void);
uint8_t sd_busy(void);

/* MISC commands. */
uint8_t sendCMD9(uint8_t* buffer, uint32_t bufferSize);
uint8_t sendCMD13(void);


#endif /* SD_CARD_DRIVER_H__ */
