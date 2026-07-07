#include <rtc_module_driver.h>

//#define DEBUG_MODE__

//#define DS3231_MODULE__
//#define DS1307_MODULE__

/* Addresses from datasheet. */
#define RTC_MODULE_SLAVE_ADDRESS 			(0x68U)
#define RTC_MODULE_SECONDS_REGISTER_ADDRESS (0x00U)
#define RTC_MODULE_MINUTES_REGISTER_ADDRESS (0x01U)
#define RTC_MODULE_HOURS_REGISTER_ADDRESS 	(0x02U)
#define RTC_MODULE_DAY_REGISTER_ADDRESS 	(0x03U)
#define RTC_MODULE_DATE_REGISTER_ADDRESS 	(0x04U)
#define RTC_MODULE_MONTH_REGISTER_ADDRESS 	(0x05U)
#define RTC_MODULE_YEAR_REGISTER_ADDRESS 	(0x06U)

/* AM/PM mask. */
#define AM_PM_BIT_Pos (5U)
#define AM_PM_BIT_Msk (1U << AM_PM_BIT_Pos)

/* 12/24-hour format mask. */
#define HOUR_FORMAT_BIT_Pos (6U)
#define HOUR_FORTMAT_BIT_Msk (1U << HOUR_FORMAT_BIT_Pos)

#define CENTURY_BIT_Pos (7U)
#define CENTURY_BIT_Msk (1U << CENTURY_BIT_Pos)

#define READ_ACCESS_BIT  (1U)
#define WRITE_ACCESS_BIT (0U)


/***** Global variables START. *****/
const uint8_t slaveAddress = RTC_MODULE_SLAVE_ADDRESS;
uint32_t baseCentury = 2000;
volatile uint8_t rtc_module_initialized = 0;
uint8_t isTwentyFourFormat = 0;
uint8_t isPM = 0;
/***** Global variables END. *****/


/***** Helper functions definition START. *****/

/* Helper for merging target register address and data.
 * Result will be in this format:
 *
 *      dest[0]        dest[1]   ...  dest[n-1]
 * |-----------------|---------|-----|---------|
 * | registerAddress | data[1] | ... | data[n] |
 * |-----------------|---------|-----|---------|
 *
 * */
void modifyRegister(const uint8_t registerAddress, const uint8_t* data, const uint8_t dataLen) {
	uint8_t temp[dataLen + 1];
	temp[0] = registerAddress;

	for(int i = 0; i < dataLen; ++i) {
		temp[i + 1] = data[i];
	}

	i2c_controller_transmit(slaveAddress, temp, sizeof(temp));
} /* End of modifyRegister(). */


/* RTC-module-specific I2C implementation with repeated start. See datasheet for more information.
 * dataLen must be <= sizeof(dataBuff).
 */
uint8_t access_register(const uint8_t slaveAddress, const uint8_t registerAddress, uint8_t* dataBuff, const int dataLen) {
	if((dataBuff == NULL) | (dataLen < 1)) {
		return 0;
	}

	uint8_t temp;

	/* Generate START condition. Pulls SDA line LOW while SCL is HIGH. */
	I2C1->CR1 |= I2C_CR1_START;

	/* Poll until SB bit is set, then clear by reading SR1 and writing to DR. */
	while(!(I2C1->SR1 & I2C_SR1_SB)) { }
	temp = I2C1->SR1;

	/* Write target address to DR. Shifted by 1 so LSB = 0; for transmitter mode to
	 * set register address to prepare for access. */
	I2C1->DR = (slaveAddress << 1);

	/* Poll until address transmission completes. Then clear ADDR bit by reading
	 * SR1 and SR2. */
	while(!(I2C1->SR1 & I2C_SR1_ADDR)) { }
	temp = I2C1->SR1;
	temp = I2C1->SR2;

	/* Poll until TX buffer empty, then send target register address. */
	while(!(I2C1->SR1 & I2C_SR1_TXE)) { }
	I2C1->DR = registerAddress;

	return(i2c_controller_receive(slaveAddress, dataBuff, dataLen));
} /* End of access_register(). */



static void convertToBCD(uint8_t* fields, uint8_t fieldCount) {
	uint8_t low;  /* Lower 4-bits. */
	uint8_t high; /* Higher 4-bits. */

	for(int i = 0; i < fieldCount; ++i) {
		low = fields[i] % 10;  /* Extract Ones-place. */
		high = fields[i] / 10; /* Extract Tens-place. */
		fields[i] = (high << 4) | low;
	}

#ifdef DEBUG_MODE__
	for(int i = 0; i < fieldCount; ++i) {
		printf("Fields[%d]: %02X\r\n", i, fields[i]);
	}
#endif
} /* End of convertToBCD(). */

static void convertToDecimal(uint8_t* fields, uint8_t fieldCount) {
	uint8_t low;  /* Lower 4-bits. */
	uint8_t high; /* Higher 4-bits. */

	for(int i = 0; i < fieldCount; ++i) {
		low = fields[i] & 0x0FU;
		high = fields[i] & 0xF0U;
		high >>= 4;
		fields[i] = (high * 10) + low;
	}

#ifdef DEBUG_MODE__
	for(int i = 0; i < fieldCount; ++i) {
		printf("Fields[%d]: %02X\r\n", i, fields[i]);
	}
#endif
} /* End of convertToDecimal(). */

raw_time_t getRawTime(void) {
	raw_time_t r_time;

	/* Start address of time read. */
	uint8_t regAddress = RTC_MODULE_SECONDS_REGISTER_ADDRESS;

	access_register(slaveAddress, regAddress, (uint8_t*)&r_time, sizeof(raw_time_t));

#ifdef DEBUG_MODE__
	printf("r_time: 0x%02X raw_hours; 0x%02X raw_minutes; 0x%02X raw_seconds\r\n", r_time.hours, r_time.minutes, r_time.seconds);
#endif

	return r_time;
} /* End of getRawTime(). */


raw_date_t getRawDate(void) {
	raw_date_t r_date;
	uint8_t regAddress = RTC_MODULE_DAY_REGISTER_ADDRESS;

	access_register(slaveAddress, regAddress, (uint8_t*)&r_date, sizeof(raw_date_t));

#ifdef DEBUG_MODE__
	printf("r_date: 0x%02X raw_Month; 0x%02Xd raw_Date; 0x%02X raw_Year\r\n", r_date.month, r_date.date, r_date.year);
#endif

	return r_date;
} /* End of getRawDate(). */
/***** Helper functions definition END. *****/


/***** API definition START. *****/
uint8_t rtc_module_init_status(void) {
	return rtc_module_initialized;
} /* End of rtc_module_init_status(). */

void rtc_module_init(void) {
	i2c_init();

	raw_time_t rTime = getRawTime();

	/* If bit 6 is set then time is in 12-hour format. */
	if(rTime.hours & HOUR_FORTMAT_BIT_Msk) {
		isTwentyFourFormat = 0;

		/* If bit 5 is set then time is PM. */
		isPM = (rTime.hours & AM_PM_BIT_Msk) ? 1 : 0;
	}
	else {
		isTwentyFourFormat = 1;
	}

	rtc_module_initialized = 1;
} /* End of rtc_module_init(). */


void setAMPM(uint8_t toPM) {
	/* If not in 24-hour format. */
	if(!isTwentyFourFormat) {
		uint8_t regAddress = RTC_MODULE_HOURS_REGISTER_ADDRESS;
		raw_time_t rTime = getRawTime();

		if(toPM) {
			if(!isPM) {
				/* Set PM bit to set to PM. */
				rTime.hours |= AM_PM_BIT_Msk;

				/* Transmit data. */
				modifyRegister(regAddress, (uint8_t *) &rTime, sizeof(raw_time_t));
			}
		}
		else {
			if(isPM) {
				/* Set PM bit to set to PM. */
				rTime.hours &= ~AM_PM_BIT_Msk;

				/* Transmit data. */
				modifyRegister(regAddress, (uint8_t *) &rTime, sizeof(raw_time_t));
			}
		}
	}
} /* End of setAMPM(). */

void setTimeFormat(uint8_t twenty_four_hr_format) {
	uint8_t regAddress = RTC_MODULE_HOURS_REGISTER_ADDRESS;

	raw_time_t rTime = getRawTime();

	if(twenty_four_hr_format) {
		/* If not currently in 24-hour format. */
		if(!isTwentyFourFormat) {
			rTime.hours &= ~HOUR_FORTMAT_BIT_Msk;

			/* Transmit data. */
			modifyRegister(regAddress, (uint8_t *) &rTime, sizeof(raw_time_t));
		}
	}
	else {
		/* If not currently in 12-hour format. */
		if(isTwentyFourFormat) {
			rTime.hours |= HOUR_FORTMAT_BIT_Msk;

			/* Transmit data. */
			modifyRegister(regAddress, (uint8_t *) &rTime, sizeof(raw_time_t));
		}
	}
} /* End of setTimeFormat(). */



void setCurrentTime(raw_time_t curTime, uint8_t twenty_four_hr_format, uint8_t isPM) {
	uint8_t regAddress = RTC_MODULE_SECONDS_REGISTER_ADDRESS;

#ifdef DS1307_MODULE__
	/* To prevent halting the oscillator on accident. */
	if(time.seconds & (0x80U)) {
		time.seconds &= ~(0x80U);
	}
#endif

	convertToBCD((uint8_t*)&curTime, sizeof(raw_time_t));

	if(twenty_four_hr_format) {
		/* Set 24-hour format bit. */
		curTime.hours &= ~HOUR_FORTMAT_BIT_Msk;
	}
	else {
		/* Set 12-hour format bit. */
		curTime.hours |= HOUR_FORTMAT_BIT_Msk;
		if(isPM) {
			/* Set to PM. */
			curTime.hours |= AM_PM_BIT_Msk;
		}
	}

	/* Transmit data. */
	modifyRegister(regAddress, (uint8_t *) &curTime, sizeof(raw_time_t));
} /* End of setCurrentTime(). */

void setCurrentDate(raw_date_t curDate) {
	uint8_t regAddress = RTC_MODULE_DAY_REGISTER_ADDRESS;

	convertToBCD((uint8_t*)&curDate, sizeof(raw_date_t));

	/* Transmit data. */
	modifyRegister(regAddress, (uint8_t *) &curDate, sizeof(raw_date_t));
} /* End of setCurrentDate(). */


/* Fetch the time in decimal. Always returns to time in 24-hour format. */
void getCurrentTime(current_time_t* curTime) {
	raw_time_t r_time = getRawTime();

	/* Check hour format. */
	if(r_time.hours & HOUR_FORTMAT_BIT_Msk) { /* 12-Hour format. */
		uint8_t isPM = (r_time.hours & AM_PM_BIT_Msk) ? 1 : 0;

		/* Extract the bits for 12-hour format. */
		r_time.hours &= (0x1FU);

#ifdef DEBUG_MODE__
		printf("cr_time: 0x%02X raw_hours; 0x%02X raw_minutes; 0x%02X raw_seconds\r\n", c_time.hours, c_time.minutes, c_time.seconds);
#endif

		convertToDecimal((uint8_t*)&r_time, sizeof(raw_time_t));

		/* Force 'hours' portion to 24-hour format. */
		if(isPM) {
			if(r_time.hours < 12) {
				r_time.hours += 12;
			}
		}
		else {
			if(r_time.hours == 12) {
				r_time.hours = 0;
			}
		}
	}
	else { /* 24-hour format. */
		convertToDecimal((uint8_t*)&r_time, sizeof(raw_time_t));
	}

	curTime->hours = r_time.hours;
	curTime->minutes = r_time.minutes;
	curTime->seconds = r_time.seconds;
} /* End of getCurrentTime(). */

void getCurrentDate(current_date_t* curDate) {

	raw_date_t r_date = getRawDate();

	if(r_date.month & CENTURY_BIT_Msk) {
		baseCentury += 100;
	}

	r_date.month &= ~CENTURY_BIT_Msk;

	convertToDecimal((uint8_t*)&r_date, sizeof(raw_date_t));

	curDate->date = r_date.date;
	curDate->dayOfWeek = r_date.dayOfWeek;
	curDate->month = r_date.month;
	curDate->year = baseCentury + r_date.year;
} /* End of getCurrentDate(). */

void printFormattedDateTime(const current_time_t time, const current_date_t date, uint8_t selection) {

} /* End of printFormattedDateTime(). */

/***** API definition END. *****/
