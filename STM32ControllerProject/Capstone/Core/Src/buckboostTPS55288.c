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

const uint8_t AddressesTPS55288[2] = {0x74, 0x75};

I2C_HandleTypeDef *Determine_I2C_Bus_TPS55288(uint8_t Converter_Index)
{
	uint8_t converter_set = Converter_Index / 2;
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
	uint8_t address_type = AddressesTPS55288[Converter_Index % 2]; // 0 sets device address to 0x74, 1 to 0x75
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t ReadValue = 0;
	uint8_t WriteValue = 0;
	uint8_t mask = MASKSTART ^ (1 << MODE_I2CADD);
	ReadValue = ReadByteTPS55288(Converter_Index, MODE, &HAL_Status);
	WriteValue = (ReadValue & mask) + (address_type << MODE_I2CADD);
	if (HAL_Status == HAL_OK) {
		HAL_Status = WriteByteTPS55288(Converter_Index, MODE, WriteValue);
	}
	else {
		Converter_Index = Converter_Index + (Converter_Index % 2 ? -1 : 1);
		ReadValue = ReadByteTPS55288(Converter_Index, MODE, &HAL_Status);
		if (HAL_Status != HAL_OK) {
					Error_Handler();
		}
		HAL_Status = WriteByteTPS55288(Converter_Index, MODE, WriteValue);
		if (HAL_Status != HAL_OK) {
			Error_Handler();
		}
	}
}

void Enable_Output_TPS55288(uint8_t Converter_Index)
{
	uint8_t ReadValue = 0;
	uint8_t WriteValue = 0;
	HAL_StatusTypeDef HAL_Status;
	ReadValue = ReadByteTPS55288(Converter_Index, MODE, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	WriteValue = (ReadValue | (1 << MODE_OE));
	HAL_Status = WriteByteTPS55288(Converter_Index, MODE, WriteValue);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
}

void Update_Reference_Voltage_TPS55288(uint8_t Converter_Index, uint8_t Increase_Boolean, uint8_t Change_Amount)
{
	uint16_t Current_Reference_Voltage = 0;
	uint8_t ReadValue = 0;
	HAL_StatusTypeDef HAL_Status;
	ReadValue = ReadByteTPS55288(Converter_Index, REF0, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	Current_Reference_Voltage = Current_Reference_Voltage + ReadValue;
	ReadValue = ReadByteTPS55288(Converter_Index, REF1, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	Current_Reference_Voltage = Current_Reference_Voltage + ((ReadValue * REF1MASK) << 8);
	uint16_t Next_Reference_Voltage;
	uint8_t Ref1_Write;
	uint8_t Ref0_Write;
	if (Increase_Boolean && Current_Reference_Voltage <= (0x3FF - Change_Amount)) {
		Next_Reference_Voltage = Current_Reference_Voltage + Change_Amount;
		Ref1_Write = ((Next_Reference_Voltage >> 8) & MASKSTART) + (ReadValue & (~REF1MASK));
		Ref0_Write = Next_Reference_Voltage & MASKSTART;
		WriteByteTPS55288(Converter_Index, REF0, Ref0_Write);
		WriteByteTPS55288(Converter_Index, REF1, Ref1_Write);
	}
	else if ((!Increase_Boolean) && Current_Reference_Voltage >= Change_Amount) {
		Next_Reference_Voltage = Current_Reference_Voltage - Change_Amount;
		Ref1_Write = ((Next_Reference_Voltage >> 8) & MASKSTART) + (ReadValue & (~REF1MASK));
		Ref0_Write = Next_Reference_Voltage & MASKSTART;
		WriteByteTPS55288(Converter_Index, REF0, Ref0_Write);
		WriteByteTPS55288(Converter_Index, REF1, Ref1_Write);
	}
}

void Set_Reference_Voltage_TPS55288(uint8_t Converter_Index, uint32_t Reference_Voltage)
{
	uint8_t ReadValue = 0;
	HAL_StatusTypeDef HAL_Status;
	ReadValue = ReadByteTPS55288(Converter_Index, REF1, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	uint8_t Ref1_Write;
	uint8_t Ref0_Write;
	Ref1_Write = ((Reference_Voltage >> 8) & MASKSTART) + (ReadValue & (~REF1MASK));
	Ref0_Write = Reference_Voltage & MASKSTART;
	WriteByteTPS55288(Converter_Index, REF0, Ref0_Write);
	WriteByteTPS55288(Converter_Index, REF1, Ref1_Write);
}

HAL_StatusTypeDef WriteByteTPS55288(uint8_t Converter_Index, uint8_t Register_Address, uint8_t WriteData)
{
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_TPS55288(Converter_Index);
	uint8_t Device_Address = AddressesTPS55288[address_type];
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

uint8_t ReadByteTPS55288(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling)
{
	uint8_t ReturnValue = 0;
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_TPS55288(Converter_Index);
	uint8_t Device_Address = AddressesTPS55288[address_type];
	uint8_t BytesToSend[1];
	BytesToSend[0] = Register_Address;
	*Error_Handling = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 1, HAL_MAX_DELAY);
	*Error_Handling = HAL_I2C_Master_Receive(I2C_Line_Address, Device_Address, &ReturnValue, 1, HAL_MAX_DELAY);
	return ReturnValue;
}
