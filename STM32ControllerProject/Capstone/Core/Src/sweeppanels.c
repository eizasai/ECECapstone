/*
 * sweeppanels.c
 *
 *  Created on: Oct 21, 2024
 *      Author: eizak
 */
#include "sweeppanels.h"

void Reset_Converters(uint8_t Number_of_Converters)
{
	for (int i = 0; i < Number_of_Converters; i++) {
		Set_Reference_Voltage_TPS55288(i, 0);
	}
}

void Increase_Converter_Reference_Voltage(uint32_t Increase_Rate);


void Get_Sensor_Measurements(float *Voltage, float *Current);

