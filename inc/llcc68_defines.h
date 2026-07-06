/* Author: Marcellus Von Sacramento
 *
 * Purpose: Contains MACRO definitions based on RYLR689 datasheet.
 *
 * Note: Read the RYLR689 datasheet datasheet for more information.
 *
 * Copyright(c).
 *
 */

#ifndef LLCC68_DEFINES_H__
#define LLCC68_DEFINES_H__

/* MISC. */
#define LLCC68_NOP (0x00U) /* No Operation. */

/* ============================================================
 * 11.1 Operational Mode Commands
 * ============================================================ */
#define LLCC68_CMD_SET_SLEEP                 (0x84U)  /* SetSleep — Set Chip in SLEEP mode */
#define LLCC68_CMD_SET_STANDBY               (0x80U)  /* SetStandby — Set Chip in STDBY_RC or STDBY_XOSC mode */
#define LLCC68_CMD_SET_FS                    (0xC1U)  /* SetFs — Set Chip in Frequency Synthesis mode */
#define LLCC68_CMD_SET_TX                    (0x83U)  /* SetTx — Set Chip in Tx mode */
#define LLCC68_CMD_SET_RX                    (0x82U)  /* SetRx — Set Chip in Rx mode */
#define LLCC68_CMD_STOP_TIMER_ON_PREAMBLE    (0x9FU)  /* StopTimerOnPreamble — Stop Rx timeout on Sync Word/Header or preamble detection */
#define LLCC68_CMD_SET_RX_DUTY_CYCLE         (0x94U)  /* SetRxDutyCycle — Store RTC listen mode values; if period != 0, enter RX mode */
#define LLCC68_CMD_SET_CAD                   (0xC5U)  /* SetCad — Set chip in RX mode with passed CAD parameters */
#define LLCC68_CMD_SET_TX_CONT_WAVE          (0xD1U)  /* SetTxContinuousWave — Set chip in TX mode with infinite carrier wave settings */
#define LLCC68_CMD_SET_TX_INFINITE_PREAMBLE  (0xD2U)  /* SetTxInfinitePreamble — Set chip into TX mode with infinite preamble settings */
#define LLCC68_CMD_SET_REGULATOR_MODE        (0x96U)  /* SetRegulatorMode — Select LDO or DC_DC+LDO for CFG_XOSC, FS, RX or TX mode */
#define LLCC68_CMD_CALIBRATE                 (0x89U)  /* Calibrate — Calibrate RC13, RC64, ADC, PLL, Image */
#define LLCC68_CMD_CALIBRATE_IMAGE           (0x98U)  /* CalibrateImage — Launch image calibration at given frequencies */
#define LLCC68_CMD_SET_PA_CONFIG             (0x95U)  /* SetPaConfig — Configure PA duty cycle, max power, device, LUT */
#define LLCC68_CMD_SET_RXTX_FALLBACK_MODE    (0x93U)  /* SetRxTxFallbackMode — Defines fallback mode after TX/RX done */

/* ============================================================
 * 11.2 Register and Buffer Access Commands
 * ============================================================ */
#define LLCC68_CMD_WRITE_REGISTER            (0x0DU)  /* WriteRegister — Write into one or several registers */
#define LLCC68_CMD_READ_REGISTER             (0x1DU)  /* ReadRegister — Read one or several registers */
#define LLCC68_CMD_WRITE_BUFFER              (0x0EU)  /* WriteBuffer — Write data into the FIFO */
#define LLCC68_CMD_READ_BUFFER               (0x1EU)  /* ReadBuffer — Read data from the FIFO */

/* ============================================================
 * 11.3 DIO and IRQ Control Commands
 * ============================================================ */
#define LLCC68_CMD_SET_DIO_IRQ_PARAMS        (0x08U)  /* SetDioIrqParams — Configure the IRQ and the DIOs attached to each IRQ */
#define LLCC68_CMD_GET_IRQ_STATUS            (0x12U)  /* GetIrqStatus — Get the values of the triggered IRQs */
#define LLCC68_CMD_CLEAR_IRQ_STATUS          (0x02U)  /* ClearIrqStatus — Clear one or several of the IRQs */
#define LLCC68_CMD_SET_DIO2_RF_SWITCH_CTRL   (0x9DU)  /* SetDIO2AsRfSwitchCtrl — Configure radio to control an RF switch from DIO2 */
#define LLCC68_CMD_SET_DIO3_TCXO_CTRL        (0x97U)  /* SetDIO3AsTcxoCtrl — Configure radio to use a TCXO controlled by DIO3 */

/* ============================================================
 * 11.4 RF, Modulation and Packet Commands
 * ============================================================ */
#define LLCC68_CMD_SET_RF_FREQUENCY          (0x86U)  /* SetRfFrequency — Set the RF frequency of the radio */
#define LLCC68_CMD_SET_PACKET_TYPE           (0x8AU)  /* SetPacketType — Select the packet type corresponding to the modem */
#define LLCC68_CMD_GET_PACKET_TYPE           (0x11U)  /* GetPacketType — Get the current packet configuration for the device */
#define LLCC68_CMD_SET_TX_PARAMS             (0x8EU)  /* SetTxParams — Set output power and ramp time for the PA */
#define LLCC68_CMD_SET_MODULATION_PARAMS     (0x8BU)  /* SetModulationParams — Compute and set modem values for given modulation parameters */
#define LLCC68_CMD_SET_PACKET_PARAMS         (0x8CU)  /* SetPacketParams — Set modem values for given packet parameters */
#define LLCC68_CMD_SET_CAD_PARAMS            (0x88U)  /* SetCadParams — Set parameters used for performing CAD (LoRa only) */
#define LLCC68_CMD_SET_BUFFER_BASE_ADDRESS   (0x8FU)  /* SetBufferBaseAddress — Store TX and RX base address in modem registers */
#define LLCC68_CMD_SET_LORA_SYMB_TIMEOUT     (0xA0U)  /* SetLoRaSymbNumTimeout — Set number of symbols modem waits to validate lock */

/* ============================================================
 * 11.5 Status Commands
 * ============================================================ */
#define LLCC68_CMD_GET_STATUS                (0xC0U)  /* GetStatus — Returns the current status of the device */
#define LLCC68_CMD_GET_RSSI_INST             (0x15U)  /* GetRssiInst — Returns instantaneous measured RSSI while in Rx mode */
#define LLCC68_CMD_GET_RX_BUFFER_STATUS      (0x13U)  /* GetRxBufferStatus — Returns PayloadLengthRx and RxBufferPointer */
#define LLCC68_CMD_GET_PACKET_STATUS         (0x14U)  /* GetPacketStatus — Returns packet status (FSK or LoRa) */
#define LLCC68_CMD_GET_DEVICE_ERRORS         (0x17U)  /* GetDeviceErrors — Returns the error which has occurred in the device */
#define LLCC68_CMD_CLEAR_DEVICE_ERRORS       (0x07U)  /* ClearDeviceErrors — Clear all the error(s) */
#define LLCC68_CMD_GET_STATS                 (0x10U)  /* GetStats — Returns statistics on the last few received packets */
#define LLCC68_CMD_RESET_STATS               (0x00U)  /* ResetStats — Resets the values read by GetStats */

/* ============================================================
 * 12. Register Map
 * ============================================================ */

/* ---------------- DIO Configuration ---------------- */
#define LLCC68_REG_DIOX_OUTPUT_ENABLE        (0x0580U)  /* DIOx output enable — Non-standard DIOx control */
#define LLCC68_REG_DIOX_INPUT_ENABLE         (0x0583U)  /* DIOx input enable */
#define LLCC68_REG_DIOX_PULL_UP              (0x0584U)  /* DIOx pull-up control */
#define LLCC68_REG_DIOX_PULL_DOWN            (0x0585U)  /* DIOx pull-down control */

/* ---------------- Whitening / CRC (FSK Mode) ---------------- */
#define LLCC68_REG_WHITENING_INIT_MSB        (0x06B8U)  /* Whitening initial value MSB — Initial value used for the whitening LFSR in FSK mode. User should not change upper 7 bits. */
#define LLCC68_REG_WHITENING_INIT_LSB        (0x06B9U)  /* Whitening initial value LSB */

#define LLCC68_REG_CRC_INIT_MSB              (0x06BCU)  /* CRC MSB Initial Value [0] — Initial value used for the polynomial used to compute the CRC in FSK mode */
#define LLCC68_REG_CRC_INIT_LSB              (0x06BDU)  /* CRC LSB Initial Value [1] */

#define LLCC68_REG_CRC_POLY_MSB              (0x06BEU)  /* CRC MSB polynomial Value [0] — Polynomial used to compute the CRC in FSK mode */
#define LLCC68_REG_CRC_POLY_LSB              (0x06BFU)  /* CRC LSB polynomial Value [1] */

/* ---------------- FSK Sync Word ---------------- */
#define LLCC68_REG_SYNCWORD_0                (0x06C0U)  /* SyncWord[0] — 1st byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_1                (0x06C1U)  /* SyncWord[1] — 2nd byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_2                (0x06C2U)  /* SyncWord[2] — 3rd byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_3                (0x06C3U)  /* SyncWord[3] — 4th byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_4                (0x06C4U)  /* SyncWord[4] — 5th byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_5                (0x06C5U)  /* SyncWord[5] — 6th byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_6                (0x06C6U)  /* SyncWord[6] — 7th byte of the Sync Word in FSK mode */
#define LLCC68_REG_SYNCWORD_7                (0x06C7U)  /* SyncWord[7] — 8th byte of the Sync Word in FSK mode */

/* ---------------- FSK Addressing ---------------- */
#define LLCC68_REG_NODE_ADDRESS              (0x06CDU)  /* Node Address — Node Address used in FSK mode */
#define LLCC68_REG_BROADCAST_ADDRESS         (0x06CEU)  /* Broadcast Address — Broadcast Address used in FSK mode */

/* ---------------- LoRa Sync Word ---------------- */
#define LLCC68_REG_IQ_POLARITY_SETUP         (0x0736U)  /* IQ Polarity Setup — Optimize the inverted IQ operation */

#define LLCC68_REG_LORA_SYNCWORD_MSB         (0x0740U)  /* LoRa Sync Word MSB — Differentiate LoRa Public/Private Network */
#define LLCC68_REG_LORA_SYNCWORD_LSB         (0x0741U)  /* LoRa Sync Word LSB */

/* ---------------- LoRa Header Extraction ---------------- */
#define LLCC68_REG_LORA_CODING_RATE_RX       (0x0749U)  /* LoRa Coding Rate Rx — Bits 6–4: coding rate extracted from header (explicit mode only) */
#define LLCC68_REG_LORA_CRC_CONFIG_RX        (0x076BU)  /* LoRa CRC configuration Rx — Bit 4: CRC config extracted from header (explicit mode only) */

/* ---------------- BW1000 Registers ---------------- */
#define LLCC68_REG_DCC_CTRL                  (0x0805U)  /* DccCtrl — For BW1000 usage */
#define LLCC68_REG_MIX_CTRL                  (0x0806U)  /* MixCtrl — For BW1000 usage */
#define LLCC68_REG_MIX_MODE                  (0x0818U)  /* MixMode — For BW1000 usage */

#define LLCC68_REG_IF_FREQ_0                 (0x088FU)  /* IF Freq[0] — For BW1000 usage */
#define LLCC68_REG_IF_FREQ_1                 (0x0890U)  /* IF Freq[1] — For BW1000 usage */
#define LLCC68_REG_IF_FREQ_2                 (0x0891U)  /* IF Freq[2] — For BW1000 usage */

/* ---------------- Random Number Generator ---------------- */
#define LLCC68_REG_RANDOM_0                  (0x0819U)  /* RandomNumberGen[0] — Can be used to get a 32‑bit random number */
#define LLCC68_REG_RANDOM_1                  (0x081AU)  /* RandomNumberGen[1] */
#define LLCC68_REG_RANDOM_2                  (0x081BU)  /* RandomNumberGen[2] */
#define LLCC68_REG_RANDOM_3                  (0x081CU)  /* RandomNumberGen[3] */

/* ---------------- TX/RX Configuration ---------------- */
#define LLCC68_REG_TX_MODULATION             (0x0889U)  /* TxModulation — Refer to Section 15 */
#define LLCC68_REG_RX_GAIN                   (0x08ACU)  /* Rx Gain — 0x94=Power Saving, 0x96=Boosted */
#define LLCC68_REG_TX_CLAMP_CONFIG           (0x08D8U)  /* TxClampConfig — Refer to Section 15 */

#define LLCC68_REG_OCP_CONFIGURATION         (0x08E7U)  /* OCP Configuration — Set Over Current Protection level (default 0x38 = 140mA) */

/* ---------------- RTC / XTAL ---------------- */
#define LLCC68_REG_RTC_CONTROL               (0x0902U)  /* RTC Control — Enable or disable RTC Timer */

#define LLCC68_REG_XTA_TRIM                  (0x0911U)  /* XTA trim — Trimming cap on XTA pin (STDBY_XOSC only) */
#define LLCC68_REG_XTB_TRIM                  (0x0912U)  /* XTB trim — Trimming cap on XTB pin (STDBY_XOSC only) */

/* ---------------- DIO3 / Events ---------------- */
#define LLCC68_REG_DIO3_VOLTAGE_CTRL         (0x0920U)  /* DIO3 output voltage control — Non-standard DIO3 control */
#define LLCC68_REG_EVENT_MASK                (0x0944U)  /* Event Mask — Used to clear events */



#endif /* LLCC68_DEFINES_H__ */
