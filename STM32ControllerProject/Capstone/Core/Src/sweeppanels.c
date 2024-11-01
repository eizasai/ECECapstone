/*
 * sweeppanels.c
 *
 *  Created on: Oct 21, 2024
 *      Author: eizak
 */
#include "sweeppanels.h"

uint8_t Converter_Number_PS;

void Reset_Converters(uint8_t Number_of_Converters)
{
	for (int i = 0; i < Number_of_Converters; i++) {
		Set_Reference_Voltage_TPS55288(i, 0);
	}
	Converter_Number_PS = Number_of_Converters;
}

void Increase_Converter_Reference_Voltage(uint8_t Increase_Rate)
{
	for (int i = 0; i < Converter_Number_PS; i++) {
		Update_Reference_Voltage_TPS55288(i, 1, Increase_Rate);
	}
}

