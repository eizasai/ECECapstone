/*
 * sensoracs37800.h
 *
 *  Created on: Oct 16, 2024
 *      Author: eizak
 */

#ifndef INC_SENSORACS37800_H_
#define INC_SENSORACS37800_H_

#include "main.h"
#include "i2c.h"

#define RSENSE 	1
#define RISO	1

#define EEPROM 				0x00 // Permanent Memory
#define SHADOW				0x10 // RAM memory

#define SEL_REGISTER		0x0b
#define IAVGSELEN			22
#define SEL_MASK			0xffbfffff

#define I2C_ADDR_REGISTER 	0x0f
#define I2C_SLV_ADDR 		2
#define I2C_DIS_SLV_ADDR	9
#define I2C_ADDR_MASK		0x1fc
#define I2C_DIS_MASK		0xfffffdff

#define RMS_REGISTER		0x20
#define VRMS				0
#define IRMS 				16

#define NUMBER_OF_SAMPLES_FOR_RMS 127

/**
 * @brief Determines the bus to use depending on the arbitrary converter index
 * @retval I2C Line address.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
I2C_HandleTypeDef *Determine_I2C_Bus_ACS37800(uint8_t Converter_Index);

/**
 * @brief Updates Sample Values for sensor
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Number_of_Samples: Number of samples to take for RMS voltage and current
 */
void Update_Sample_Values(uint8_t Converter_Index, uint32_t Number_of_Samples);

/**
 * @brief Configure the slave addresses of the converters
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
void Configure_Slave_AddressACS37800(uint8_t Converter_Index);

/**
 * @brief Disables peripheral address setting
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
void Disable_Peripheral_Addressing_CircuitACS37800(uint8_t Converter_Index);

/**
 * @brief Calculates voltage based on register value
 * @retval RMS Voltage
 * @param Vin: Register value for input voltage
 */
uint32_t Calculate_Voltage_RMSACS37800(int16_t Vin);

/**
 * @brief Calculates Current based on register value
 * @retval RMS Current
 * @param Iin: Register value for input Current
 * @param Vin: Register value for input voltage
 * @param V_line: Calculated RMS voltage
 */
uint32_t Calculate_Current_RMSACS37800(int16_t Iin, int16_t Vin, float V_line);

/**
 * @brief Provides dummy values for testing
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Voltage: Pointer to store value from register
 * @param Current: Pointer to store value from register
 */
void Get_Sensor_Values_for_Panel_hc_test(uint8_t Converter_Index, float *Voltage, float *Current);

/**
 * @brief Enable Voltage Averaging
 * @retval None
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
void Voltage_Average_Select_EnableACS37800(uint8_t Converter_Index);

/**
 * @brief Enable Current Averaging
 * @retval None
 * @param Converter_Index: The arbitrary index of the converter in the system
 */
void Current_Average_Select_EnableACS37800(uint8_t Converter_Index);

/**
 * @brief Provides sensors values for voltage and current
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Voltage: Pointer to store value from register
 * @param Current: Pointer to store value from register
 */
void Read_Sensor_ValuesACS37800(uint8_t Converter_Index, float *Voltage, float *Current);

/**
 * @brief Write a byte of data to the specified device address on the specified I2C line
 * @retval The HAL Status type.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Register_Address: The register address to write to and change converter settings
 * @param WriteData: Data to be written to the register
 */
HAL_StatusTypeDef WriteByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, uint32_t WriteData);

/**
 * @brief Read a byte of data from the specified device address on the specified I2C line
 * @retval The Byte Stored at the Register Address.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Register_Address: The register address to read from
 * @param Error_Handling: A variable to store and check the HAL status after reading the byte
 */
uint32_t ReadByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling);

#endif /* INC_SENSORACS37800_H_ */
