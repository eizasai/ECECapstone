/*
 * hillclimbing.h
 *
 *  Created on: Oct 18, 2024
 *      Author: eizak
 */

#ifndef INC_HILLCLIMBING_H_
#define INC_HILLCLIMBING_H_

#include "main.h"
#include "buckboostTPS55288.h"
#include "sensorACS37800.h"

#define SHADED 		1
#define NOT_SHADED 	0
#define AT_MPP		1
#define NOT_AT_MPP	0
#define INCREASE	1
#define DECREASE	0
#define Current_Threshold		0 //TODO Determine these values experimentally
#define Voltage_Threshold		0
#define Power_Threshold			0
#define Partial_Shade_Threshold	0

void Initialize_HillClimbing(uint32_t Number_of_Converters, SolarPanel *SolarPanels);

void Get_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, float *Voltage, float *Current);

float Calculate_Average_Current_hc(SolarPanel *SolarPanels);

float Calculate_Power_hc(float Voltage, float Current);

void Change_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount);

void Update_Panel_Parameters_hc(uint8_t Converter_Index, float New_Voltage, float New_Current, uint8_t At_mpp, uint8_t Partially_shaded, SolarPanel *SolarPanels);

void Update_Panel_State_hc(uint8_t Converter_Index, SolarPanel *SolarPanels);

void Update_All_Panel_States_hc(SolarPanel *SolarPanels);

#endif /* INC_HILLCLIMBING_H_ */
