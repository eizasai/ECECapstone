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

#define I2C_ADDR_REGISTER 	0x0f
#define I2C_SLV_ADDR 		2
#define I2C_DIS_SLV_ADDR	9
#define I2C_ADDR_MASK		0x1fc
#define I2C_DIS_MASK		0xfffffdff

#define RMS_REGISTER		0x20
#define VRMS				0
#define IRMS 				16

#define NUMBER_OF_SAMPLES_FOR_RMS 127

I2C_HandleTypeDef *Determine_I2C_Bus_ACS37800(uint8_t Converter_Index);

void Update_Sample_Values(uint8_t Converter_Index, uint32_t Number_of_Samples);

void Configure_Slave_AddressACS37800(uint8_t Converter_Index);

void Disable_Peripheral_Addressing_CircuitACS37800(uint8_t Converter_Index);

uint32_t Calculate_Voltage_RMSACS37800(int16_t Vin);

uint32_t Calculate_Current_RMSACS37800(int16_t Iin, int16_t Vin, float V_line);

void Get_Sensor_Values_for_Panel_hc_test(uint8_t Converter_Index, float *Voltage, float *Current);

void Read_Sensor_ValuesACS37800(uint8_t Converter_Index, float *Voltage, float *Current);

HAL_StatusTypeDef WriteByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, uint32_t WriteData);

uint8_t ReadByteACS37800(uint8_t Converter_Index, uint8_t Register_Address, HAL_StatusTypeDef *Error_Handling);

#endif /* INC_SENSORACS37800_H_ */
