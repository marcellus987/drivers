#ifndef SD_CARD_DRIVER_V2_H__
#define SD_CARD_DRIVER_V2_H__
#include "spi_driver.h"
#include "stm32f4xx.h"

typedef struct {
	spi_instance_t *spiInstance; /* SPI peripheral used for communication. */
	uint8_t chipSelectPos;       /* CS pin position. */
	uint8_t initialized;         /* Init. status. */
} sd_card_instance_t;


uint8_t xSdCardInit(sd_card_instance_t *sdInstance);
uint8_t xSdRead(SPI_TypeDef *spix, uint32_t sector, uint8_t* buffer);
uint8_t xSdWrite(SPI_TypeDef *spix, uint32_t sector, const uint8_t* buffer);
uint8_t xSdStatus(void);
uint8_t xSdBusy(void);

/* MISC commands. */
uint8_t xSendCMD9(uint8_t* buffer, uint32_t bufferSize);
uint8_t xSendCMD13(void);
#endif /* SD_CARD_DRIVER_V2_H__ */
