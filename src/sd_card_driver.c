#include "sd_card_driver.h"

//#define DEBUG_MODE /* Used for printing debug messages. Comment-out for release.*/

/* Port A pin 8. */
#define CS_PIN_Pos (8U)
#define SD_DUMMY_BYTE (0xFFU)

#define CMD_TEMPLATE (0x40U) /* Command template: 0 1 ... 1 */

/* The following are SD Card SPI command indices derived from SD Specification Part 1. */
#define CMD0 	(0x00U)
#define CMD8 	(0x08U)
#define CMD9 	(0x09U)
#define CMD13 	(0x0DU)
#define CMD17 	(0x11U)
#define CMD24 	(0x18U)
#define CMD55 	(0x37U)
#define CMD58 	(0x3AU)
#define ACMD41 	(0x29U)

#define COMMAND_ARGUMENT_COUNT (4U)

/* Arbitrary loop timeout. */
#define RESPONSE_TIMEOUT (1000U)
#define DATA_TIMEOUT (50000U)

/* Command and Data width in Bytes.*/
#define COMMAND_TOKEN_WIDTH (6U)
#define DATA_TOKEN_WIDTH (512U)

/* Response width in Bytes. */
#define R1_RESPONSE_WIDTH (1U)
#define R2_RESPONSE_WIDTH (2U)
#define R3_RESPONSE_WIDTH (5U)
#define R6_RESPONSE_WIDTH (6U)
#define R7_RESPONSE_WIDTH (5U)

/* The following are generator polynomials for generating CRCs.*/
#define CRC7_POLYNOMIAL (0x09U)
#define CRC16_POLYNOMIAL (0x1021U)

typedef struct {
	uint8_t msb; 		 /* Most Significant Byte = 8-bit: 1-bit Start + 1-bit Direction + 4-bit Command Index. */
	uint8_t argument[4]; /* 32-bit: Argument. */
	uint8_t lsb; 		 /* Least Significant Byte = 8-bit: 7-bit CRC7 + 1-bit Stop. */
} CommandToken;

typedef struct {
	uint8_t startToken; 	/* Start of transmission from host to SD, or vice-versa. */
	uint8_t dataBlock[512]; /* The actual data from host to SD, or vice-versa. */
	uint8_t crc16[2]; 		/* 16-bit CRC. */
} DataToken;

/* Response buffer. Per Datasheet, longest response in SPI mode is 5 bytes. */
typedef struct {
	uint8_t r1Byte;
	uint8_t restOfBytes[4];
} ResponseToken;

typedef enum {
	R1_SD_NOT_IDLE = 0x00,
	R1_SD_IDLE = 0x01,
	R1_ERASE_RESET = 0x02,
	R1_ILLEGAL_COMMAND = 0x04,
	R1_CRC_ERROR = 0x08,
	R1_ERASE_SEQUENCE_ERROR = 0x10,
	R1_ADDRESS_ERROR = 0x20,
	R1_PARAMETER_ERROR = 0x30,
	R1_FATAL_ERROR = 0x7F
} R1_RESULT;

typedef enum {
	/* General command result. */
	CMD_ACCEPTED = 0,
	ILLEGAL_CMD,
	CRC_ERROR,
	ADDRESS_ERROR,
	PARAMETER_ERROR,
	FATAL_ERROR, /* To be used only if R1 = 0x3F. */

	/* For timeouts. */
	CARD_UNRESPONSIVE,

	/* For CMD58. */
	CCS_ACCEPTED,
	CCS_REJECTED,

	/* Voltage check. */
	VOLTAGE_REJECTED,
	VOLTAGE_ACCEPTED,
} CMDRESULT;


/***** Helper method declaration START. *****/
uint8_t generateCRC7(const uint8_t* input, uint8_t size);
uint16_t generateCRC16(const uint8_t* input, uint32_t size);
CMDRESULT sendCMD0(void);
CMDRESULT sendCMD8(void);
CMDRESULT sendACMD41(void);
CMDRESULT sendCMD58(const CMDRESULT cmd8Status);
void print_cmdtoken(uint8_t* cmd_token);
void print_response(uint8_t* response, uint8_t size);
void send_cmd(uint8_t cmd, uint8_t* args, uint32_t argCount, uint8_t needCRC);
void parseAddress(uint8_t* dest, uint32_t address);
void getResponse_R1(uint8_t* rBuffer);
/***** Helper method declaration END. *****/

/***** Globals START. *****/

volatile uint8_t sd_initialized = 0;

/***** Globals END. *****/

/***** Helper functions definition START. *****/
uint8_t generateCRC7(const uint8_t* input, uint8_t size) {
	uint8_t crc = 0;

	for(int i = 0; i < size; ++i) {
		uint8_t currentByte = input[i];

		for(int j = 0; j < 8; ++j) {
			uint8_t crc_msb = (crc & 0x40) ? 1 : 0;
			uint8_t bit = (currentByte & 0x80) ? 1 : 0;

			crc <<= 1;

			if (crc_msb ^ bit)  {
				crc ^= CRC7_POLYNOMIAL;
			}
			currentByte <<= 1;   // <-- move this to the end
		}
	}

	/* Only need 7-bits. */
	return (crc & 0x7F);
} /* End of generateCRC7(). */

uint16_t generateCRC16(const uint8_t* input, uint32_t size) {
	uint16_t crc = 0;
	uint32_t crc_msb_pos = 15;

	for(int i = 0; i < size; ++i) {
		uint8_t currentByte = input[i];

		for(int j = 7; j >= 0; --j) {
			/* Extract one data bit. */
			uint8_t crc_msb = (crc >> crc_msb_pos) & 1;
			uint8_t bit = (currentByte >> j) & 1;

			/* Make space for new bit. */
			crc <<= 1;

			/* Check if CRC MSB is 1. */
			if(crc_msb ^ bit) {
				// XOR.
				crc ^= CRC16_POLYNOMIAL;
			}
		}
	}

	return crc;
} /* End of generateCRC16(). */

CMDRESULT sendCMD0(void) {
	uint8_t r1;

	send_cmd(CMD0, (uint8_t[]) {0x00U, 0x00U, 0x00U, 0x00U}, COMMAND_ARGUMENT_COUNT, 1);
	getResponse_R1(&r1);

	return (r1 & R1_SD_IDLE ? CMD_ACCEPTED : FATAL_ERROR);
} /* End of sendCMD0(). */

CMDRESULT sendCMD8(void) {
	uint8_t r7[R7_RESPONSE_WIDTH];
	uint8_t checkPattern = 0xAAU;
	uint8_t vhs = 0x01U; /* Voltage supply. */

	send_cmd(CMD8, (uint8_t[]) {0x00U, 0x00U, vhs, checkPattern}, COMMAND_ARGUMENT_COUNT, 1);
	getResponse_R1(r7); /* MSB of R7 is R1 format. */

	if(r7[0] & R1_ILLEGAL_COMMAND) {
		return ILLEGAL_CMD;
	}

	if(r7[0] == R1_SD_IDLE) { /* Retrieve the rest of the response. */
		spi_receive(r7 + 1, R7_RESPONSE_WIDTH - 1, SD_DUMMY_BYTE);
	}
	else {
		return FATAL_ERROR;
	}

	/* Parse R7 response. */
	if(r7[R7_RESPONSE_WIDTH - 1] == checkPattern) {
		if(r7[R7_RESPONSE_WIDTH - 2] != vhs) { /* Voltage not supported by card. */
			return VOLTAGE_REJECTED;
		}
		return CMD_ACCEPTED;
	}
	else {
		return CRC_ERROR; /* Check pattern failed. Retry sending CMD8. */
	}
} /* End of sendCMD8(). */

CMDRESULT sendACMD41(void) {
	uint8_t r1 = 0x01U;

	while(r1 != 0x00U) {
		send_cmd(CMD55, (uint8_t[]) {0x00U, 0x00U, 0x00U, 0x00U}, COMMAND_ARGUMENT_COUNT, 1);
		getResponse_R1(&r1);

		send_cmd(ACMD41, (uint8_t[]) {0x40U, 0x00U, 0x00U, 0x00U}, COMMAND_ARGUMENT_COUNT, 1);
		getResponse_R1(&r1);

		if(r1 & R1_ILLEGAL_COMMAND) {
			return ILLEGAL_CMD;
		}
	}

	return CMD_ACCEPTED;
} /* End of sendACMD41(). */

CMDRESULT sendCMD58(const CMDRESULT cmd8Status) {
	uint8_t r3[R3_RESPONSE_WIDTH];

	send_cmd(CMD58, (uint8_t[]) {0x00, 0x00, 0x00, 0x00}, COMMAND_ARGUMENT_COUNT, 1);
	getResponse_R1(r3);

	if(r3[0] & R1_ILLEGAL_COMMAND) {
		return ILLEGAL_CMD;
	}

	spi_receive(r3 + 1, R3_RESPONSE_WIDTH - 1, SD_DUMMY_BYTE);

	/* Parse CMD depending on status of previous CMD8. */
	if(cmd8Status == CMD_ACCEPTED) {
		if(r3[1] & 0x40) {
			return CCS_ACCEPTED;
		}
		return CCS_REJECTED;
	}
	else {
		if((r3[2] & 0xFFU) || (r3[3] & 0x80)) { /* Check operating voltage. */
			return VOLTAGE_ACCEPTED;
		}
		return VOLTAGE_REJECTED;
	}
} /* End of sendCMD58(). */

/* STM32 stores data in little-endian so it must be extracted,
 * in correct order before passing to read or write command. */
void parseAddress(uint8_t* dest, uint32_t address) {
	for(int i = 0; i < COMMAND_ARGUMENT_COUNT; ++i) {
		dest[COMMAND_ARGUMENT_COUNT - i - 1] = address & 0xFF;
		address >>= 8;
	}
} /* End of parseAddress(). */

void send_cmd(uint8_t cmd, uint8_t* args, uint32_t argCount, uint8_t needCRC) {
	CommandToken token;

	token.msb = CMD_TEMPLATE | cmd;

	for(int i = 0; i < argCount; ++i) {
		token.argument[i] = args[i];
	}

	/* Some command do not need CRC and stop bit such as ACMD41 in SPI mode. */
	if(needCRC) {
		token.lsb = ((generateCRC7((uint8_t*)&token, COMMAND_TOKEN_WIDTH - 1) << 1) | 0x01);
	}
	else {
		token.lsb = 0x01;
	}
	spi_transmit((uint8_t*)&token, sizeof(CommandToken));

#ifdef DEBUG_MODE
	printf("MSB: %02X\r\n", token.msb);
	printf("Arguments: ");
	for(int i = 0; i < argCount; ++i) {
		printf("%02X ", token.argument[i]);
	}
	printf("\r\nLSB: %02X\r\n", token.lsb);
#endif

} /* End of send_cmd(). */

void getResponse_R1(uint8_t* r1Buffer) {
	for(int i = 0; i < RESPONSE_TIMEOUT; ++i) {
		spi_receive(r1Buffer, 1, SD_DUMMY_BYTE);
		if(*r1Buffer != 0xFF) {
			break;
		}
	}
#ifdef DEBUG_MODE
	printf("R1 response: %02X\r\n", *r1Buffer);
#endif
}/* End of getRespones_R1(). */

uint8_t verifyCRC(uint8_t crc, uint8_t* response, uint32_t size) {
	if(crc == (response[size - 1] >> 1)) {
		return 1;
	}
	return 0;
} /* End of verifyCRC(). */

void print_cmdtoken(uint8_t* cmd_token) {
	for(int i = 0; i < COMMAND_TOKEN_WIDTH; ++i) {
#ifdef DEBUG_MODE
		printf("cmd_token[%d]: %02X\r\n",i,cmd_token[i]);
#endif
	}

} /* End of print_cmdtoken(). */

void print_response(uint8_t* response, uint8_t size) {
	for(int i = 0; i < size; ++i) {
#ifdef DEBUG_MODE
		printf("response[%d]: %02X\r\n", i, response[i]);
#endif
	}
} /* End of print_response(). */
/***** Helper functions definition END. *****/

/***** API definition START *****/
uint8_t sendCMD13(void) {
	uint8_t r2[R2_RESPONSE_WIDTH];
	send_cmd(CMD13, (uint8_t []){0x00U,0x00U,0x00U,0x00U}, COMMAND_ARGUMENT_COUNT, 0);
	getResponse_R1(r2);

	if(r2[0] == R1_SD_NOT_IDLE) {
		spi_receive(r2 + 1, R2_RESPONSE_WIDTH - 1, SD_DUMMY_BYTE);

		if(r2[1] == 0x00U) {

#ifdef DEBUG_MODE
			printf("Write succeeded!\r\n");
#endif

			return 1;
		}
	}

	return 0;
}

uint8_t sendCMD9(uint8_t* buffer, uint32_t bufferSize) {
#ifdef DEBUG_MODE
	printf("Fetching SD card CSD register...\r\n");
#endif
	uint8_t r1;

	send_cmd(CMD9, (uint8_t []){0x00U,0x00U,0x00U,0x00U}, COMMAND_ARGUMENT_COUNT, 0);
	getResponse_R1(&r1);

	if(r1 & R1_SD_IDLE) {
#ifdef DEBUG_MODE
	printf("FATAL ERROR! Aborting SD card CSD register fetch...\r\n");
#endif
		return 0;
	}

	/* CSD register will be received as 16 bytes block. */
	uint8_t startTok;
	uint8_t crcbuff[2];
	for(int i = 0; i < DATA_TIMEOUT; ++i) {
		spi_receive(&startTok, 1, SD_DUMMY_BYTE);
		if(startTok == 0xFE) {
			spi_receive(buffer, bufferSize, SD_DUMMY_BYTE); /* CSD register. */
			spi_receive(crcbuff,2, SD_DUMMY_BYTE);
			break;
		}
	}
#ifdef DEBUG_MODE
	printf("Successfully fetched SD card CSD register...\r\n");
#endif
	return 1;
}


uint8_t sd_write(uint32_t sector, const uint8_t* buffer) {
#ifdef DEBUG_MODE
	printf("SD write START...\r\n");
#endif
	uint8_t r1;
	uint8_t address[4];

	parseAddress(address, sector);
	send_cmd(CMD24, address, COMMAND_ARGUMENT_COUNT, 1);
	getResponse_R1(&r1);

	if(r1 & R1_SD_IDLE) {
#ifdef DEBUG_MODE
	printf("FATAL ERROR! Aborting SD write...\r\n");
#endif
		return 0;
	}

	uint8_t dataStartToken = 0xFEU; 				  /* Data start byte. */
	uint16_t crc16Token = generateCRC16(buffer, 512);
	uint8_t crc16MSB = crc16Token >> 8; 				  /* HIGH CRC16. */
	uint8_t crc16LSB = crc16Token & 0xFFU; 			  /* LOW CRC16. */

#ifdef DEBUG_MODE
	printf("Transmitting data...\r\n");
#endif

	spi_transmit(&dataStartToken, 1); /* Send start token. */
	spi_transmit(buffer, 512);		  /* Send actual data to be written. */

	/* Send 2 byte CRC16 in two 1 byte token. */
	spi_transmit(&crc16MSB, 1);
	spi_transmit(&crc16LSB, 1);

	uint8_t dataResponse; /* SD card response to data block transmission.*/

	for(int i = 0; i < DATA_TIMEOUT; ++i) {
		spi_receive(&dataResponse, 1, SD_DUMMY_BYTE);
		if((dataResponse & 0x1F) == 0x05U) {

#ifdef DEBUG_MODE
			printf("Data accepted\r\n");
#endif

			break;
		}
		else if((dataResponse & 0x1F) == 0x1BU) {
#ifdef DEBUG_MODE
			printf("Data rejected due to a CRC error.\r\n");
#endif
			return 0;
		}
		else if((dataResponse & 0x1FU) == 0x1DU) {
#ifdef DEBUG_MODE
			printf("Data rejected due to a Write Error\r\n");
#endif
			return 0;
		}
	}
#ifdef DEBUG_MODE
	printf("SD write END...\r\n");
#endif

	sd_busy(); /* Wait until it finish programming after Data acceptance. */
	return 1;
} // End of sd_write().

uint8_t sd_read(uint32_t sector, uint8_t* buffer) {
#ifdef DEBUG_MODE
	printf("SD read START...\r\n");
#endif
	uint8_t r1;
	uint8_t address[COMMAND_ARGUMENT_COUNT];

	/* Parse the address argument for the CMD17. */
	parseAddress(address, sector);
	send_cmd(CMD17, address, COMMAND_ARGUMENT_COUNT, 1);
	getResponse_R1(&r1);

	if(r1 & R1_SD_IDLE) {
#ifdef DEBUG_MODE
	printf("FATAL ERROR! Aborting SD read...\r\n");
#endif
		return 0;
	}

	uint8_t dataStartToken;
	uint8_t crcByte[2];

	for(int i = 0; i < DATA_TIMEOUT; ++i) {
		spi_receive(&dataStartToken, 1, SD_DUMMY_BYTE);
		if(dataStartToken == 0xFE) {
			break;
		}
	}

	spi_receive(buffer, 512, SD_DUMMY_BYTE);

	/* For data integrity. CRC might be turned OFF by default in SPI mode.
	 * If so, this can be ignored. */
	spi_receive(crcByte,2, SD_DUMMY_BYTE);

#ifdef DEBUG_MODE
	printf("SD read END...\r\n");
#endif

	return 1;
} // End of sd_read().

uint8_t get_SDstatus(void) {
#ifdef DEBUG_MODE
	printf("Get SD init. status START...\r\n");
#endif
#ifdef DEBUG_MODE
	printf("Get SD init. status END...\r\n");
#endif
	return sd_initialized;

} /* End of get_SDstatus(). */


uint8_t sd_busy(void) {
#ifdef DEBUG_MODE
	printf("Check SD busy START...\r\n");
#endif
	uint8_t response = 0x00U;
	while(response != 0xFFU) {
		spi_receive(&response, 1, SD_DUMMY_BYTE);
	}
#ifdef DEBUG_MODE
	printf("Check SD busy END...\r\n");
#endif
	return 0;
} /* End of sd_not_busy(). */



/* Returns 1 if initialization succeeded. Else, returns 0.*/
uint8_t sd_init(void) {

	/* Enable clock for GPIOA. */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* Clear mode bits before selecting modes. */
	GPIOA->MODER &= ~(GPIO_MODER_MODER8_Msk);
	/* Select output mode. This will be the Chip Select pin. */
	GPIOA->MODER |= (GPIO_MODER_MODER8_0);

#ifdef DEBUG_MODE
	printf("sd_init() START...\r\n");
#endif

	const uint32_t tempSize = 20;
	uint8_t temp[tempSize];

	memset(temp, 0xFFU, tempSize);

	spi_init();

	/* Baud rate for initialization: <= 400KHz. */
	set_baudrate(0x05);

	/* SD Power Up Sequence. */

	/* Per datasheet: 0.1ms to 35ms max ramp up time (time it takes for VDD to be stable)
	 * plus the the 1ms required VDD stable time.*/
	/* 40ms just to be conservative and safe. */
	delay_ms(40);

	/* Keep CS pin HIGH. */
	slave_deselect(SPI_GPIO_A, CS_PIN_Pos);
	spi_transmit(temp, tempSize);

	/* SD card initialization START.*/
	slave_select(SPI_GPIO_A, CS_PIN_Pos);

#ifdef DEBUG_MODE
	printf("Sending CMD0...\r\n");
#endif

	/* NOTE: Please refer to the SD card Physical Spec. for the SD initialization flow chart. */
	/* Send CMD0 (RESET). */
	if(sendCMD0() == FATAL_ERROR) {
#ifdef DEBUG_MODE
		printf("FATAL ERROR: Invalid SD card...\r\n");
#endif
		return 0;
	}

#ifdef DEBUG_MODE
	printf("Sending CMD8...\r\n");
#endif

	uint8_t retry = 1;
	CMDRESULT cmd8Status;

	/* Send CMD8 (Send Interface condition). */
	while(retry) {
		cmd8Status = sendCMD8();
		switch(cmd8Status) {
		case CMD_ACCEPTED:
		case ILLEGAL_CMD: /* CMD8 will be rejected if card is Legacy (SD v1.X). */
			retry = 0;
			break;
		case FATAL_ERROR:
		case VOLTAGE_REJECTED:
#ifdef DEBUG_MODE
			printf("FATAL ERROR: Invalid SD card...");
#endif
			return 0;
		default: /* CRC_ERROR: Check pattern error. Retry. */
		}
	}

#ifdef DEBUG_MODE
	printf("Sending ACMD41 and CMD58...\r\n");
#endif

	/* Send ACMD41 or CMD58 first depending on cmd8Status. */
	if(cmd8Status == CMD_ACCEPTED) { /* SD card v2.X. */

		sendACMD41();

		if(sendCMD58(cmd8Status) == CCS_ACCEPTED) {
#ifdef DEBUG_MODE
			printf("SDHC or SDXC successfully initiated...\r\n");
#endif
		}
		else {
#ifdef DEBUG_MODE
			printf("SDSC memory card successfully initiated...\r\n");
#endif
		}
	}
	else { /* CMD8 Illegal command. SD Card is Legacy (v1.X) and voltage must be checked.*/
		if(sendCMD58(cmd8Status) == VOLTAGE_ACCEPTED) {
			if(sendACMD41() == CMD_ACCEPTED) {
#ifdef DEBUG_MODE
				printf("SDSC memory card successfully initiated...\r\n");
#endif
			}
			else {
#ifdef DEBUG_MODE
				printf("FATAL ERROR: Invalid SD card...\r\n");
#endif
				return 0;
			}
		}
		else {
#ifdef DEBUG_MODE
			printf("FATAL ERROR: Invalid SD card...\r\n");
#endif
			return 0;
		}
	}
	/* SD card initialization END.*/

	sd_initialized = 1; /* Set global status variable. */

	/* Baud rate for other operation. fpclk = 16MHz/2 = 8 MHz. */
	set_baudrate(0x00);

#ifdef DEBUG_MODE
	printf("sd_init() END...\r\n");
#endif

	return 1;
} /* End of sd_init(). */

/***** API definition END *****/
