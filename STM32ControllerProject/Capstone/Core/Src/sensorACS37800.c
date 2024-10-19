/*
 * sensoracs37800.c
 *
 *  Created on: Oct 16, 2024
 *      Author: eizak
 */
#include <sensorACS37800.h>

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

const uint8_t AddressesACS37800[2] = {0x60, 0x61};

I2C_HandleTypeDef *Determine_I2C_Bus_ACS37800(uint8_t Converter_Index)
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

void Configure_Slave_AddressACS37800(uint8_t Converter_Index)
{
	uint8_t address_type = AddressesACS37800[Converter_Index % 2]; // 0 sets device address to 0x74, 1 to 0x75
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t ReadValue = 0;
	uint8_t WriteValue = 0;
	ReadValue = ReadByteTPS55288(Converter_Index, I2C_ADDR_REGISTER + EEPROM, &HAL_Status);
	WriteValue = (ReadValue & I2C_ADDR_MASK) + (address_type << I2C_SLV_ADDR);
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
	Disable_Peripheral_Addressing_CircuitACS37800(Converter_Index);
}

void Disable_Peripheral_Addressing_CircuitACS37800(uint8_t Converter_Index)
{
	uint8_t ReadValue = 0;
	uint8_t WriteValue = 0;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	ReadValue = ReadByteTPS55288(Converter_Index, I2C_ADDR_REGISTER + EEPROM, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	WriteValue = (ReadValue & I2C_DIS_MASK) + (1 << I2C_DIS_SLV_ADDR);
	if (HAL_Status == HAL_OK) {
		HAL_Status = WriteByteTPS55288(Converter_Index, MODE, WriteValue);
	}
}

uint32_t Calculate_Voltage_RMSACS37800(uint16_t Vin)
{
	uint32_t V_line = (Vin * (RISO + RSENSE)) / RSENSE;
	return V_line;
}

uint32_t Calculate_Current_RMSACS37800(uint16_t Iin, uint16_t Vin, uint16_t V_line)
{
	uint32_t I_line = Iin * (V_line / Vin);
	return I_line;
}

void Get_Sensor_Values_for_Panel_hc_test(uint8_t Converter_Index, uint32_t *Voltage, uint32_t *Current)
{
	*Voltage = 20;
	*Current = 10;
}

void Read_Sensor_ValuesACS37800(uint8_t Converter_Index, uint32_t *Voltage, uint32_t *Current)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t ReadValue;
	uint16_t Vin;
	uint16_t Iin;
	ReadValue = ReadByteACS37800(Converter_Index, RMS_REGISTER, &HAL_Status);
	Vin = ReadValue & 0xffff;
	Iin = (ReadValue >> 16) & 0xffff;
	*Voltage = Calculate_Voltage_RMSACS37800(Vin);
	*Current = Calculate_Current_RMSACS37800(Iin, Vin, *Voltage);
}

HAL_StatusTypeDef WriteByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, uint32_t WriteData)
{
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_ACS37800(Converter_Index);
	uint8_t Device_Address = AddressesACS37800[address_type];
	uint8_t BytesToSend[5];
	BytesToSend[0] = Register_Address;
	uint8_t WriteByte;
	for (int i = 0; i > 4; i++) {
		WriteByte = (WriteData >> (8 * i)) & 0xff;
		BytesToSend[4 - i] = WriteByte;
	}
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	HAL_Status = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 5, HAL_MAX_DELAY);
	if (HAL_Status != HAL_OK) {
		return HAL_Status;
//		Error_Handler();
	}
	while (HAL_I2C_IsDeviceReady(I2C_Line_Address, Device_Address, 1, HAL_MAX_DELAY));
	return HAL_Status;
}

uint8_t ReadByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling)
{
	uint32_t ReturnValue = 0;
	uint8_t ReadValue[4];
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_ACS37800(Converter_Index);
	uint8_t Device_Address = AddressesACS37800[address_type];
	uint8_t BytesToSend[1];
	BytesToSend[0] = Register_Address;
	*Error_Handling = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 1, HAL_MAX_DELAY);
	*Error_Handling = HAL_I2C_Master_Receive(I2C_Line_Address, Device_Address, ReadValue, 4, HAL_MAX_DELAY);
	for (int i = 0; i < 4; i++) {
		ReturnValue = ReturnValue + (ReadValue[i] << (8 *(3 - i)) & 0xff);
	}
	return ReturnValue;
}





