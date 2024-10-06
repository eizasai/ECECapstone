/*
 * buckboostTPS55288.h
 *
 *  Created on: Oct 5, 2024
 *      Author: eizak
 */

#ifndef INC_BUCKBOOSTTPS55288_H_
#define INC_BUCKBOOSTTPS55288_H_

#include "main.h"
#include "i2c.h"

#define MASKSTART 			0xff
// Register Addresses
#define REF0 				0x0 // Uses bits 7-0

#define REF1 				0x1 // Uses bits 9-8
#define REF1_VREF			0

#define IOUT_LIMIT 			0x2	// Current Limit Setting
#define IOUT_LIMIT_ENABLE	7
#define IOUT_LIMIT_SETTING	0

#define VOUT_SR 			0x3	// Slew Rate
#define VOUT_SR_OCP_DELAY 	4
#define VOUT_SR_SR			0

#define VOUT_FS				0x4	// Feedback Selection
#define VOUT_FS_FB			7
#define VOUT_FS_INTFB		0

#define CDC					0x5	// Cable Compensation
#define CDC_SC_MASK			7
#define CDC_OCP_MASK 		6
#define CDC_OVP_MASK		5
#define CDC_CDC_OPTION 		3
#define CDC_CDC				0

#define MODE 				0x6	// Mode Control
#define MODE_OE				7 // Output Enable
#define MODE_FSWDBL 		6
#define MODE_HICCUP 		5
#define MODE_DISCHG 		4
#define MODE_VCC			3
#define MODE_I2CADD 		2 // Set slave addresses
#define	MODE_PFM			1
#define MODE_MODE 			0

#define STATUS				0x7	// Operating Status
#define STATUS_SCP 			7
#define STATUS_OCP 			6
#define STATUS_OVP			5
#define STATUS_STATUS		0 // 00 boost, 01 buck, 10 buck-boost

/**
 * @brief Determines the bust to use depending on the arbitrary converter index
 * @retval I2C Line address.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
I2C_HandleTypeDef *Determine_I2C_Bus_TPS55288(uint8_t Converter_Index);

/**
 * @brief Configure the slave addresses of the converters
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
void Configure_Slave_AddressTPS55288(uint8_t Converter_Index);

/**
 * @brief Write a byte of data to the specified device address on the specified I2C line
 * @retval The HAL Status type.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Register_Address: The register address to write to and change converter settings
 * @param WriteData: Data to be written to the register
 */
HAL_StatusTypeDef WriteByteTPS55288(uint8_t Converter_Index, uint8_t Register_Address, uint8_t WriteData);

uint8_t ReadByteTPS55288(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling);

#endif /* INC_BUCKBOOSTTPS55288_H_ */
