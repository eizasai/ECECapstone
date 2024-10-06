/*
 * buckboostTPS55288.c
 *
 *  Created on: Oct 5, 2024
 *      Author: eizak
 */
#include "buckboostTPS55288.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

const uint8_t Addresses[2] = {0x74, 0x75};

I2C_HandleTypeDef *Determine_I2C_Bus_TPS55288(uint8_t converter_index)
{
	uint8_t converter_set = converter_index / 2;
	I2C_HandleTypeDef *I2C_Line_Address;
	switch (converter_set) {
	case 0:
		I2C_Line_Address = &hi2c1;
		break;
	case 1:
		I2C_Line_Address = &hi2c2;
		break;
	case 2:
		I2C_Line_Address = &hi2c3;
		break;
	}
	return I2C_Line_Address;
}

void Configure_Slave_AddressTPS55288(uint8_t Converter_Index)
{
	uint8_t address_type = Converter_Index % 2; // 0 sets device address to 0x74, 1 to 0x75
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	HAL_Status = WriteByteTPS55288(Converter_Index, MODE, address_type);
	if (HAL_Status != HAL_OK) {
		HAL_Status = WriteByteTPS55288(Converter_Index + (Converter_Index % 2 ? -1 : 1), MODE, address_type);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	}
}

HAL_StatusTypeDef WriteByteTPS55288(uint8_t Converter_Index, uint8_t Register_Address, uint8_t WriteData)
{
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_TPS55288(Converter_Index);
	uint8_t Device_Address = Addresses[address_type];
	uint8_t BytesToSend[2];
	BytesToSend[0] = Register_Address;
	BytesToSend[1] = WriteData;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	HAL_Status = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 2, HAL_MAX_DELAY);
	if (HAL_Status != HAL_OK) {
		return HAL_Status;
//		Error_Handler();
	}
	while (HAL_I2C_IsDeviceReady(I2C_Line_Address, Device_Address, 1, HAL_MAX_DELAY));
	return HAL_Status;
}
