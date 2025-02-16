/*
 * hillclimbing.c
 *
 *  Created on: Oct 18, 2024
 *      Author: eizak
 */

#include "hillclimbing.h"

uint32_t Converter_Number_HC = 0;
//	TODO: Set V_Feedback
float V_Feedback = 0.0564f;


void Initialize_HillClimbing(uint32_t Number_of_Converters, SolarPanel *SolarPanels)
{
	Converter_Number_HC = Number_of_Converters;
	float Voltage;
	float Current;
	for (int i = 0; i < Converter_Number_HC; i++) {
		Get_Sensor_Values_for_Panel_hc(i, &Voltage, &Current);
		SolarPanels[i].Current_Current = Current;
		SolarPanels[i].Current_Voltage = Voltage;
		SolarPanels[i].Previous_Current = Current;
		SolarPanels[i].Previous_Voltage = Voltage;
		SolarPanels[i].Shading_Conditions = NOT_SHADED;
		SolarPanels[i].At_MPP = NOT_AT_MPP;
	}
}

void Get_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, float *Voltage, float *Current)
{
	uint16_t Voltage_Measurement;
	uint16_t Current_Measurement;
	Read_Sensor_ValuesACS37800(Converter_Index, &Voltage_Measurement, &Current_Measurement);
	*Voltage = (float) Voltage_Measurement / 5;
	*Current = (float) Current_Measurement / 5;
}

float Calculate_Power_hc(float Voltage, float Current)
{
	return Voltage * Current;
}

float Calculate_Average_Current_hc(SolarPanel *SolarPanels)
{
	float Total_Current = 0;
	for (int i = 0; i < Converter_Number_HC; i++) {
		Total_Current = Total_Current + SolarPanels[i].Current_Current;
	}
	return Total_Current / Converter_Number_HC;
}

void Change_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount)
{
	Update_Reference_Voltage_TPS55288(Converter_Index, Increase, Change_Amount);
}

void Update_Panel_Parameters_hc(uint8_t Converter_Index, float New_Voltage, float New_Current, uint8_t At_mpp, uint8_t Partially_shaded, SolarPanel *SolarPanels)
{
	SolarPanels[Converter_Index].Previous_Current = SolarPanels[Converter_Index].Current_Current;
	SolarPanels[Converter_Index].Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	SolarPanels[Converter_Index].Current_Current = New_Current;
	SolarPanels[Converter_Index].Current_Voltage = New_Voltage;
	SolarPanels[Converter_Index].Shading_Conditions = Partially_shaded;
	SolarPanels[Converter_Index].At_MPP = At_mpp;
}

uint8_t Check_if_All_are_MPPT_hc(SolarPanel *SolarPanels)
{
	for (int i = 0; i < Converter_Number_HC; i++) {
		if (!(SolarPanels[i].At_MPP)) {
			return 0;
		}
	}
	return 1;
}

float Update_Panel_State_hc(uint8_t Converter_Index, SolarPanel *SolarPanels)
{
	uint16_t reference_register = Get_Reference_Voltage_TPS55288(Converter_Index);
	float Previous_Voltage = SolarPanels[Converter_Index].Current_Voltage;
	float Previous_Current = SolarPanels[Converter_Index].Current_Current;
	float Voltage;
	float Current;
	Get_Sensor_Values_for_Panel_hc(Converter_Index, &Voltage, &Current);
	float Power = Calculate_Power_hc(Voltage, Current);
	float Previous_Power = Calculate_Power_hc(Previous_Voltage, Previous_Current);
	uint8_t Partially_shaded = SolarPanels[Converter_Index].Shading_Conditions;
	uint8_t At_mpp = SolarPanels[Converter_Index].At_MPP;
	if (Partially_shaded) {
		if (Voltage - Previous_Voltage > Partial_Shade_Threshold) {
			Partially_shaded = NOT_SHADED;
			At_mpp = NOT_AT_MPP;
		}
		else {
//			Change_Panel_Values_hc(Converter_Index, INCREASE, 8);
			reference_register += 8;
		}
	}
	else {
		if ((abs(Power - Previous_Power) < Power_Threshold) && (abs(Voltage - Previous_Voltage) < Voltage_Threshold)) {
			At_mpp = AT_MPP;
			if (Check_if_All_are_MPPT_hc(SolarPanels)) {
				float Average_Current = Calculate_Average_Current_hc(SolarPanels);
				if ((Average_Current - Current) > Current_Threshold) {
					Partially_shaded = SHADED;
				}
			}
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) > 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) < 0)) {
//			Change_Panel_Values_hc(Converter_Index, INCREASE, 8);
			reference_register += 8;
			At_mpp = NOT_AT_MPP;
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) < 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) > 0)) {
//			Change_Panel_Values_hc(Converter_Index, DECREASE, 8);
			reference_register -= 8;
			At_mpp = NOT_AT_MPP;
		}
	}
//	TODO: Cap reference register at min/max range
	Update_Panel_Parameters_hc(Converter_Index, Voltage, Current, At_mpp, Partially_shaded, SolarPanels);
	float Desired_Vout = ((45.f + reference_register*1.129f) / 1000.f) / (V_Feedback);
	SolarPanels[Converter_Index].Output_Voltage = Desired_Vout;
	SolarPanels[Converter_Index].Output_Voltage_Reference_Register = reference_register;
	return Desired_Vout;
}

void Update_All_Panel_States_hc(SolarPanel *SolarPanels)
{
	float total_vout = 0;
	for (int i = 0; i < Converter_Number_HC; i++) {
		total_vout += Update_Panel_State_hc(i, SolarPanels);
	}
//	TODO: Set to appropriate value
	float MAX_CHARGE_VOLTAGE = 32;
	if(total_vout > MAX_CHARGE_VOLTAGE) {
		float needed_vout_decrease = total_vout - MAX_CHARGE_VOLTAGE;
//		Amount that vout goes down for every decrease by 1 in reference register
		float vout_decrease_per_register_decrease = 1.129f / ( 1000.f * V_Feedback);
		uint16_t needed_register_decrease = 1 + needed_vout_decrease / vout_decrease_per_register_decrease;
		uint16_t base_decrease = needed_register_decrease / Converter_Number_HC;
		uint16_t extra_decrease = needed_register_decrease % Converter_Number_HC;
		for (int i = 0; i < Converter_Number_HC; i++) {
//			TODO: Account for going below min
			SolarPanels[i].Output_Voltage_Reference_Register -= base_decrease;
			if (extra_decrease > 0) {
				SolarPanels[i].Output_Voltage_Reference_Register -= 1;
				--extra_decrease;
			}
			SolarPanels[i].Output_Voltage = ((45.f + SolarPanels[i].Output_Voltage_Reference_Register*1.129f) / 1000.f) / (V_Feedback);
		}
		for (int i = 0; i < Converter_Number_HC; i++) {
			Set_Reference_Voltage_TPS55288(i, SolarPanels[i].Output_Voltage_Reference_Register);
		}
	}

}

