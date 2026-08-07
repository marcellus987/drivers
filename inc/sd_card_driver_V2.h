#ifndef SD_CARD_DRIVER_V2_H__
#define SD_CARD_DRIVER_V2_H__
#include "spi_driver.h"

typedef struct {
	spi_instance_t *spiInstance; /* SPI peripheral used for communication. */
	GPIO_TypeDef *csPinPort;
	uint8_t csPinPos;       	 /* Chip Select pin position. */
	uint8_t initialized;         /* Init. status. */
} sd_card_instance_t;


uint8_t xSdCardInit(void);
uint8_t xSdRead(uint32_t sector, uint8_t* buffer);
uint8_t xSdWrite(uint32_t sector, const uint8_t* buffer);
uint8_t xSdInitStatus(void);
uint8_t xSdBusy(void);
void vSetTargetSDInstance(sd_card_instance_t *targetSdInstance);

/* MISC commands. */
uint8_t xSendCMD9(uint8_t* buffer, uint32_t bufferSize);
uint8_t xSendCMD13(void);
#endif /* SD_CARD_DRIVER_V2_H__ */
