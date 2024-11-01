/*
 * perturb_and_observe.c
 *
 *  Created on: Oct 31, 2024
 *      Author: eizak
 */
#include "perturb_and_observe.h"

/*
 * hillclimbing.c
 *
 *  Created on: Oct 18, 2024
 *      Author: eizak
 */

#include "hillclimbing.h"

uint32_t Converter_Number_PO = 0;

void Initialize_PerturbAndObserve(uint32_t Number_of_Converters, SolarPanel *SolarPanels)
{
	Converter_Number_PO = Number_of_Converters;
	float Voltage;
	float Current;
	for (int i = 0; i < Converter_Number_PO; i++) {
		Get_Sensor_Values_for_Panel_po(i, &Voltage, &Current);
		SolarPanels[i].Current_Current = Current;
		SolarPanels[i].Current_Voltage = Voltage;
		SolarPanels[i].Previous_Current = Current;
		SolarPanels[i].Previous_Voltage = Voltage;
		SolarPanels[i].Shading_Conditions = NOT_SHADED;
		SolarPanels[i].At_MPP = NOT_AT_MPP;
	}
}

void Get_Sensor_Values_for_Panel_po(uint8_t Converter_Index, float *Voltage, float *Current)
{
	Read_Sensor_ValuesACS37800(Converter_Index, Voltage, Current);
}

float Calculate_Power_po(float Voltage, float Current)
{
	return Voltage * Current;
}

float Calculate_Average_Current_po(SolarPanel *SolarPanels)
{
	float Total_Current = 0;
	for (int i = 0; i < Converter_Number_PO; i++) {
		Total_Current = Total_Current + SolarPanels[i].Current_Current;
	}
	return Total_Current / Converter_Number_PO;
}

void Change_Panel_Values_po(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount)
{
	Update_Reference_Voltage_TPS55288(Converter_Index, Increase, Change_Amount);
}

void Update_Panel_Parameters_po(uint8_t Converter_Index, float New_Voltage, float New_Current, uint8_t At_mpp, uint8_t Partially_shaded, SolarPanel *SolarPanels)
{
	SolarPanels[Converter_Index].Previous_Current = SolarPanels[Converter_Index].Current_Current;
	SolarPanels[Converter_Index].Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	SolarPanels[Converter_Index].Current_Current = New_Current;
	SolarPanels[Converter_Index].Current_Voltage = New_Voltage;
	SolarPanels[Converter_Index].Shading_Conditions = Partially_shaded;
	SolarPanels[Converter_Index].At_MPP = At_mpp;
}

uint8_t Check_if_All_are_MPPT_po(SolarPanel *SolarPanels)
{
	for (int i = 0; i < Converter_Number_PO; i++) {
		if (!(SolarPanels[i].At_MPP)) {
			return 0;
		}
	}
	return 1;
}

void Update_Panel_State_po(uint8_t Converter_Index, SolarPanel *SolarPanels)
{
	float Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	float Previous_Current = SolarPanels[Converter_Index].Current_Current;
	float Voltage;
	float Current;
	Get_Sensor_Values_for_Panel_po(Converter_Index, &Voltage, &Current);
	float Power = Calculate_Power_po(Voltage, Current);
	float Previous_Power = Calculate_Power_po(Previous_Voltage, Previous_Current);
	uint8_t Partially_shaded = SolarPanels[Converter_Index].Shading_Conditions;
	uint8_t At_mpp = SolarPanels[Converter_Index].At_MPP;
	if (Partially_shaded) {
		if (Voltage - Previous_Voltage > Partial_Shade_Threshold) {
			Partially_shaded = NOT_SHADED;
			At_mpp = NOT_AT_MPP;
		}
		else {
			Change_Panel_Values_po(Converter_Index, INCREASE, 8);
		}
	}
	else {
		if ((abs(Power - Previous_Power) < Power_Threshold) && (abs(Voltage - Previous_Voltage) < Voltage_Threshold)) {
			At_mpp = AT_MPP;
			if (Check_if_All_are_MPPT_po(SolarPanels)) {
				float Average_Current = Calculate_Average_Current_po(SolarPanels);
				if ((Average_Current - Current) > Current_Threshold) {
					Partially_shaded = SHADED;
				}
			}
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) > 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) < 0)) {
			Change_Panel_Values_po(Converter_Index, INCREASE, 8);
			At_mpp = NOT_AT_MPP;
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) < 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) > 0)) {
			Change_Panel_Values_po(Converter_Index, DECREASE, 8);
			At_mpp = NOT_AT_MPP;
		}
	}
	Update_Panel_Parameters_po(Converter_Index, Voltage, Current, At_mpp, Partially_shaded, SolarPanels);
}

void Update_All_Panel_States_PO(SolarPanel *SolarPanels)
{
	for (int i = 0; i < Converter_Number_PO; i++) {
		Update_Panel_State_po(i, SolarPanels);
	}
}



