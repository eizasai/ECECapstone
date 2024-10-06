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

// Register Addresses
#define REF0 		0x0 // Reference Voltages
#define REF1 		0x1
#define IOUT_LIMIT 	0x2	// Current Limit Setting
#define VOUT_SR 	0x3	// Slew Rate
#define VOUT_FS		0x4	// Feedback Selection
#define CDC			0x5	// Cable Compensation
#define MODE 		0x6	// Mode Control
#define STATUS		0x7	// Operating Status

/**
 * @brief Determines the bust to use depending on the arbitrary converter index
 * @retval I2C Line address.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
I2C_HandleTypeDef Determine_I2C_Bus_TPS55288(uint8_t Converter_Index);

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

#endif /* INC_BUCKBOOSTTPS55288_H_ */
