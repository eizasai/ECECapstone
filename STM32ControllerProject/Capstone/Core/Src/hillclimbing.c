/*
 * hillclimbing.c
 *
 *  Created on: Oct 18, 2024
 *      Author: eizak
 */
#include "hillclimbing.h"

uint32_t Converter_Number = 0;
SolarPanel_hc *SolarPanels;

void Initialize_HillClimbing(uint32_t Number_of_Converters)
{
	Converter_Number = Number_of_Converters;
	SolarPanels = (SolarPanel_hc*)malloc(Number_of_Converters * sizeof(SolarPanel_hc));
	uint32_t Voltage;
	uint32_t Current;
	for (int i = 0; i < Converter_Number; i++) {
		Get_Sensor_Values_for_Panel_hc(i, &Voltage, &Current);
		SolarPanels[i].Current_Current = Current;
		SolarPanels[i].Current_Voltage = Voltage;
		SolarPanels[i].Previous_Current = Current;
		SolarPanels[i].Previous_Voltage = Voltage;
		SolarPanels[i].Shading_Conditions = NOT_SHADED;
		SolarPanels[i].At_MPP = NOT_AT_MPP;
	}
}

void Get_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, uint32_t *Voltage, uint32_t *Current)
{
	Read_Sensor_ValuesACS37800(Converter_Index, Voltage, Current);
}

uint32_t Calculate_Power_hc(uint32_t Voltage, uint32_t Current)
{
	return Voltage * Current;
}

uint32_t Calculate_Average_Current_hc()
{
	uint32_t Total_Current = 0;
	for (int i = 0; i < Converter_Number; i++) {
		Total_Current = Total_Current + SolarPanels[i].Current_Current;
	}
	return Total_Current / Converter_Number;
}

void Change_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase)
{
	Update_Reference_Voltage_TPS55288(Converter_Index, Increase);
}

void Update_Panel_Parameters_hc(uint8_t Converter_Index, uint32_t New_Voltage, uint32_t New_Current, uint8_t At_mpp, uint8_t Partially_shaded)
{
	SolarPanels[Converter_Index].Previous_Current = SolarPanels[Converter_Index].Current_Current;
	SolarPanels[Converter_Index].Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	SolarPanels[Converter_Index].Current_Current = New_Current;
	SolarPanels[Converter_Index].Current_Voltage = New_Voltage;
	SolarPanels[Converter_Index].Shading_Conditions = Partially_shaded;
	SolarPanels[Converter_Index].At_MPP = At_mpp;
}

uint8_t Check_if_All_are_MPPT_hc()
{
	for (int i = 0; i < Converter_Number; i++) {
		if (!(SolarPanels[i].At_MPP)) {
			return 0;
		}
	}
	return 1;
}

void Update_Panel_State_hc(uint8_t Converter_Index)
{
	uint32_t Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	uint32_t Previous_Current = SolarPanels[Converter_Index].Current_Current;
	uint32_t Voltage;
	uint32_t Current;
	Get_Sensor_Values_for_Panel_hc(Converter_Index, &Voltage, &Current);
	uint32_t Power = Calculate_Power_hc(Voltage, Current);
	uint32_t Previous_Power = Calculate_Power_hc(Previous_Voltage, Previous_Current);
	uint8_t Partially_shaded = SolarPanels[Converter_Index].Shading_Conditions;
	uint8_t At_mpp = SolarPanels[Converter_Index].At_MPP;
	if (Partially_shaded) {
		if (Voltage - Previous_Voltage > Partial_Shade_Threshold) {
			Partially_shaded = NOT_SHADED;
			At_mpp = NOT_AT_MPP;
		}
		else {
			Change_Panel_Values_hc(Converter_Index, DECREASE);
		}
	}
	else {
		if ((abs(Power - Previous_Power) < Power_Threshold) && (abs(Voltage - Previous_Voltage) < Voltage_Threshold)) {
			At_mpp = AT_MPP;
			if (Check_if_All_are_MPPT_hc()) {
				uint32_t Average_Current = Calculate_Average_Current_hc();
				if ((Average_Current - Current) > Current_Threshold) {
					Partially_shaded = SHADED;
				}
			}
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) > 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) < 0)) {
			Change_Panel_Values_hc(Converter_Index, INCREASE);
			At_mpp = NOT_AT_MPP;
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) < 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) > 0)) {
			Change_Panel_Values_hc(Converter_Index, DECREASE);
			At_mpp = NOT_AT_MPP;
		}
	}
	Update_Panel_Parameters_hc(Converter_Index, Voltage, Current, At_mpp, Partially_shaded);
}

void Update_All_Panel_States_hc()
{
	for (int i = 0; i < Converter_Number; i++) {
		Update_Panel_State_hc(i);
	}
}

