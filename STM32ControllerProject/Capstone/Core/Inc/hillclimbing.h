/*
 * hillclimbing.h
 *
 *  Created on: Oct 18, 2024
 *      Author: eizak
 */

#ifndef INC_HILLCLIMBING_H_
#define INC_HILLCLIMBING_H_

#include <stdlib.h>
#include "main.h"
#include "buckboostTPS55288.h"
#include "sensorACS37800.h"

#define SHADED 		1
#define NOT_SHADED 	0
#define AT_MPP		1
#define NOT_AT_MPP	0
#define INCREASE	1
#define DECREASE	0
#define Current_Threshold		1 //TODO Determine these values experimentally
#define Voltage_Threshold		1
#define Power_Threshold			1
#define Partial_Shade_Threshold	1

void Initialize_HillClimbing(uint32_t Number_of_Converters);

void Get_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, uint32_t *Voltage, uint32_t *Current);

uint32_t Calculate_Power_hc(uint32_t Voltage, uint32_t Current);

void Change_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase);

void Update_Panel_Parameters_hc(uint8_t Converter_Index, uint32_t New_Voltage, uint32_t New_Current, uint8_t At_mpp, uint8_t Partially_shaded);

void Update_Panel_State_hc(uint8_t Converter_Index);

void Update_All_Panel_States_hc();

typedef struct SolarPanel_hc_struct {
	uint32_t Previous_Voltage;
	uint32_t Current_Voltage;
	uint32_t Previous_Current;
	uint32_t Current_Current;
	uint8_t Shading_Conditions;
	uint8_t At_MPP;
} SolarPanel_hc;

#endif /* INC_HILLCLIMBING_H_ */
