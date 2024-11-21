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
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t ReadValue = 0;
	uint32_t WriteValue = 0;
	uint8_t Address = I2C_ADDR_REGISTER + EEPROM;
	uint8_t address_type = Converter_Index % 2;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	WriteValue = (ReadValue & I2C_ADDR_MASK) + ((uint32_t) AddressesACS37800[address_type] << I2C_SLV_ADDR);
	if (HAL_Status != HAL_OK) {
		Converter_Index = Converter_Index + (Converter_Index % 2 ? -1 : 1);
		ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
		WriteValue = (ReadValue & I2C_ADDR_MASK) + ((uint32_t) AddressesACS37800[address_type] << I2C_SLV_ADDR);
	}
	HAL_Status = WriteBytesACS37800(Converter_Index, ACCESS_CODE_REGISTER, ACCESS_CODE);
	HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	Disable_Peripheral_Addressing_CircuitACS37800(Converter_Index);
	Update_Sample_Values(Converter_Index);
	Bypass_N_EN_ACS37800(Converter_Index);
	Set_N_ACS37800(Converter_Index);
	Current_Average_Select_EnableACS37800(Converter_Index);
}

void Bypass_N_EN_ACS37800(uint8_t Converter_Index)
{
	uint32_t ReadValue = 0;
	uint32_t WriteValue = 0;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Address = I2C_ADDR_REGISTER + EEPROM;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	if (HAL_Status == HAL_OK) {
		WriteValue = ReadValue | (1 << BYPASS_N_EN);
		HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	}
}

void Set_N_ACS37800(uint8_t Converter_Index)
{
	uint32_t ReadValue = 0;
	uint32_t WriteValue = 0;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Address = I2C_ADDR_REGISTER + EEPROM;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	if (HAL_Status == HAL_OK) {
		WriteValue = (ReadValue & N_MASK) + (NUMBER_OF_SAMPLES_FOR_RMS << N);
		HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	}
}

void Update_Sample_Values(uint8_t Converter_Index)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t ReadValue = 0;
	uint32_t WriteValue = 0;
	uint8_t Address = NUM_AVG_REGISTER + EEPROM;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	WriteValue = (ReadValue & NUM_AVG_MASK) + NUMBER_OF_SAMPLES_FOR_RMS;
	WriteBytesACS37800(Converter_Index, Address, WriteValue);
}

void Disable_Peripheral_Addressing_CircuitACS37800(uint8_t Converter_Index)
{
	uint32_t ReadValue = 0;
	uint32_t WriteValue = 0;
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Address = I2C_ADDR_REGISTER + EEPROM;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	if (HAL_Status == HAL_OK) {
		WriteValue = (ReadValue & I2C_DIS_MASK) + (1 << I2C_DIS_SLV_ADDR);
		HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	}
}

uint32_t Calculate_Voltage_RMSACS37800(int16_t Vin)
{
	float Vin_as_float = Vin / (1 << 14);
	uint32_t V_line = (Vin_as_float * (RISO + RSENSE)) / RSENSE;
	return V_line;
}

uint32_t Calculate_Current_RMSACS37800(int16_t Iin, int16_t Vin, float V_line)
{
	float Vin_as_float = Vin / (1 << 14);
	float Iin_as_float = Iin / (1 << 14);
	uint32_t I_line = Iin_as_float * (V_line / Vin_as_float);
	return I_line;
}

void Get_Sensor_Values_for_Panel_hc_test(uint8_t Converter_Index, float *Voltage, float *Current)
{
	*Voltage = 1.6;
	*Current = 7.3;
}

void Read_Sensor_ValuesACS37800(uint8_t Converter_Index, float *Voltage, float *Current)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint32_t ReadValue;
	int16_t Vin;
	int16_t Iin;
	ReadValue = ReadBytesACS37800(Converter_Index, RMS_REGISTER, &HAL_Status);
	Vin = ReadValue & 0xffff;
	Iin = (ReadValue >> 16) & 0xffff;
	*Voltage = Calculate_Voltage_RMSACS37800(Vin);
	*Current = Calculate_Current_RMSACS37800(Iin, Vin, *Voltage);
}

void Voltage_Average_Select_EnableACS37800(uint8_t Converter_Index)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Address = SEL_REGISTER + EEPROM;
	uint32_t ReadValue;
	uint32_t WriteValue;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	WriteValue = ReadValue & SEL_MASK;
	HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
}

void Current_Average_Select_EnableACS37800(uint8_t Converter_Index)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Address = SEL_REGISTER + EEPROM;
	uint32_t ReadValue;
	uint32_t WriteValue;
	ReadValue = ReadBytesACS37800(Converter_Index, Address, &HAL_Status);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
	WriteValue = (ReadValue & SEL_MASK) + (1 << IAVGSELEN);
	HAL_Status = WriteBytesACS37800(Converter_Index, Address, WriteValue);
	if (HAL_Status != HAL_OK) {
		Error_Handler();
	}
}

HAL_StatusTypeDef WriteBytesACS37800(uint8_t Converter_Index, uint8_t Register_Address, uint32_t WriteData)
{
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_ACS37800(Converter_Index);
	uint8_t Device_Address = AddressesACS37800[address_type] << 1;
	uint8_t BytesToSend[5];
	BytesToSend[0] = Register_Address;
	for (int i = 0; i < 4; i++) {
		BytesToSend[i + 1] = (uint8_t) (WriteData >> (8 * i)) & 0xff;
	}
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	HAL_Status = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 5, HAL_MAX_DELAY);
	if (HAL_Status != HAL_OK) {
		return HAL_Status;
//		Error_Handler();
	}
	while (HAL_I2C_IsDeviceReady(I2C_Line_Address, Device_Address, 1, HAL_MAX_DELAY));
	if (Register_Address < 0x10) {
		WriteBytesACS37800(Converter_Index, Register_Address + SHADOW, WriteData);
	}
	return HAL_Status;
}

uint32_t ReadBytesACS37800(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling)
{
	uint32_t ReturnValue = 0;
	uint8_t ReadValue[4];
	uint8_t address_type = Converter_Index % 2;
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_ACS37800(Converter_Index);
	uint8_t Device_Address = AddressesACS37800[address_type] << 1;
	uint8_t BytesToSend[1];
	BytesToSend[0] = Register_Address;
	*Error_Handling = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 1, HAL_MAX_DELAY);
	*Error_Handling = HAL_I2C_Master_Receive(I2C_Line_Address, Device_Address, ReadValue, 4, HAL_MAX_DELAY);
	for (int i = 0; i < 4; i++) {
		ReturnValue = ReturnValue + ((uint32_t) ReadValue[i] << (8 * i));
	}
	return ReturnValue;
}

void TestAddressesACS37800(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling)
{
	uint8_t Potential_addresses[16] = {96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 127};
	uint8_t ReadValue[4];
	I2C_HandleTypeDef *I2C_Line_Address = Determine_I2C_Bus_ACS37800(Converter_Index);
	uint8_t BytesToSend[1];
	uint8_t Device_Address;
	for (int i = 0; i < 16; i++) {
		Device_Address = Potential_addresses[i] << 1;
		BytesToSend[0] = Register_Address;
		*Error_Handling = HAL_I2C_Master_Transmit(I2C_Line_Address, Device_Address, BytesToSend, 1, HAL_MAX_DELAY);
		*Error_Handling = HAL_I2C_Master_Receive(I2C_Line_Address, Device_Address, ReadValue, 4, HAL_MAX_DELAY);
		if (*Error_Handling == HAL_OK) {
			Configure_Slave_AddressACS37800(Converter_Index);
			return;
		}
	}
}





