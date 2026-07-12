///*-----------------------------------------------------------------------*/
///* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
///*-----------------------------------------------------------------------*/
///* If a working storage control module is available, it should be        */
///* attached to the FatFs via a glue function rather than modifying it.   */
///* This is an example of glue functions to attach various exsisting      */
///* storage control modules to the FatFs module with a defined API.       */
///*-----------------------------------------------------------------------*/
//#include <stdio.h>
//#include "ff.h"				/* Basic definitions of FatFs */
//#include "diskio.h"			/* Declarations FatFs MAI */
//#include "sd_card_driver.h" /* SD card register-level drivers. */
//#include "spi.h"
//
//
//#define DEV_MMC		0 /* Map MMC/SD card to physical drive 0*/
//
//
///*-----------------------------------------------------------------------*/
///* Get Drive Status                                                      */
///*-----------------------------------------------------------------------*/
//
//DSTATUS disk_status (
//	BYTE pdrv		/* Physical drive nmuber to identify the drive */
//)
//{
//	if(pdrv != DEV_MMC) {
//		return STA_NODISK;
//	}
////	slave_select();
//	uint8_t isInit = get_SDstatus();
////	slave_deselect();
//	return isInit ? 0 : STA_NOINIT;
//}
//
///*-----------------------------------------------------------------------*/
///* Inidialize a Drive                                                    */
///*-----------------------------------------------------------------------*/
//
//DSTATUS disk_initialize (
//	BYTE pdrv				/* Physical drive nmuber to identify the drive */
//)
//{
//	if(pdrv != DEV_MMC) {
//		return STA_NODISK;
//	}
////	slave_select();
//	uint8_t initStatus = sd_init();
//	return (initStatus ? 0 : STA_NOINIT);
//}
//
//
///*-----------------------------------------------------------------------*/
///* Read Sector(s)                                                        */
///*-----------------------------------------------------------------------*/
//
//DRESULT disk_read (
//	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
//	BYTE *buff,		/* Data buffer to store read data */
//	LBA_t sector,	/* Start sector in LBA */
//	UINT count		/* Number of sectors to read */
//)
//{
//	if(pdrv != DEV_MMC) {
//		return STA_NODISK;
//	}
//
//	for(int i = 0; i < count; ++i) {
//		if(!(sd_read(sector + i, buff + (512 * i)))) {
//			return RES_ERROR;
//		}
//	}
//
//	return RES_OK;
//}
//
//
//
///*-----------------------------------------------------------------------*/
///* Write Sector(s)                                                       */
///*-----------------------------------------------------------------------*/
//
//#if FF_FS_READONLY == 0
//
//DRESULT disk_write (
//	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
//	const BYTE *buff,	/* Data to be written */
//	LBA_t sector,		/* Start sector in LBA */
//	UINT count			/* Number of sectors to write */
//)
//{
//	if(pdrv != DEV_MMC) {
//		return STA_NODISK;
//	}
//
//	for(int i = 0; i < count; ++i) {
//		if(!(sd_write(sector + i, buff + (512 * i)))) {
//			return RES_ERROR;
//		}
//	}
//
//	return RES_OK;
//}
//
//#endif
//
//
///*-----------------------------------------------------------------------*/
///* Miscellaneous Functions                                               */
///*-----------------------------------------------------------------------*/
//
//DRESULT disk_ioctl (
//	BYTE pdrv,		/* Physical drive nmuber (0..) */
//	BYTE cmd,		/* Control code */
//	void *buff		/* Buffer to send/receive control data */
//)
//{
//	if(pdrv != DEV_MMC) {
//		return RES_ERROR;
//	}
//
//	uint8_t csdReg[16];
//	switch(cmd) {
//	case CTRL_SYNC:
//		sd_busy();
//		break;
//
//	case GET_SECTOR_COUNT:
//		if(sendCMD9(csdReg, 16)) {
//			uint32_t C_SIZE = ((csdReg[7] & 0x3FU) << 16) | (csdReg[8] << 8) | (csdReg[9]);
//			uint32_t sector_count = (C_SIZE + 1) * 1024;
//			*(DWORD*)buff = sector_count;
//			break;
//		}
//		return RES_ERROR;
//
//	case GET_SECTOR_SIZE:
//		*(DWORD*)buff = 512;
//		break;
//
//	case GET_BLOCK_SIZE:
//		*(DWORD*)buff = 128;
//		break;
//
//	case CTRL_TRIM:
//		return RES_OK;
//
//	default:
//		 return RES_PARERR;
//	}
//
//	return RES_OK;
//}
//


/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include "ff.h"				/* Basic definitions of FatFs */
#include "diskio.h"			/* Declarations FatFs MAI */
#include "sd_card_driver.h" /* SD card register-level drivers. */
#include "spi.h"


#define DEV_MMC		0 /* Map MMC/SD card to physical drive 0*/


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if(pdrv != DEV_MMC) {
		return STA_NODISK;
	}
//	slave_select();
	uint8_t isInit = get_SDstatus();
//	slave_deselect();
	return isInit ? 0 : STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	if(pdrv != DEV_MMC) {
		return STA_NODISK;
	}
//	slave_select();
	uint8_t initStatus = sd_init();
	return (initStatus ? 0 : STA_NOINIT);
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if(pdrv != DEV_MMC) {
		return STA_NODISK;
	}

	for(int i = 0; i < count; ++i) {
		if(!(sd_read(sector + i, buff + (512 * i)))) {
			return RES_ERROR;
		}
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if(pdrv != DEV_MMC) {
		return STA_NODISK;
	}

	for(int i = 0; i < count; ++i) {
		if(!(sd_write(sector + i, buff + (512 * i)))) {
			return RES_ERROR;
		}
	}

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if(pdrv != DEV_MMC) {
		return RES_ERROR;
	}

	uint8_t csdReg[16];
	switch(cmd) {
	case CTRL_SYNC:
		sd_busy();
		break;

	case GET_SECTOR_COUNT:
		if(sendCMD9(csdReg, 16)) {
			uint32_t C_SIZE = ((csdReg[7] & 0x3FU) << 16) | (csdReg[8] << 8) | (csdReg[9]);
			uint32_t sector_count = (C_SIZE + 1) * 1024;
			*(DWORD*)buff = sector_count;
			break;
		}
		return RES_ERROR;

	case GET_SECTOR_SIZE:
		*(DWORD*)buff = 512;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 128;
		break;

	case CTRL_TRIM:
		return RES_OK;

	default:
		 return RES_PARERR;
	}

	return RES_OK;
}

