/*
 * test_hillclimbing.c
 *
 *  Created on: Oct 25, 2024
 *      Author: eizak
 */
#include "test_hillclimbing.h"

uint32_t TestConverter_Number = 0;
uint32_t Test_Sensor_Voltage = 0;
uint32_t Test_Sensor_Current = 0;

void Update_Test_Sensor_Values_hc(int32_t Voltage, int32_t Current)
{
	Test_Sensor_Voltage = Voltage;
	Test_Sensor_Current = Current;
}

void TestRead_Sensor_ValuesACS37800(uint8_t Converter_Index, int32_t *Voltage, int32_t *Current)
{
	*Voltage = Test_Sensor_Voltage;
	*Current = Test_Sensor_Current;
}

void TestUpdate_Reference_Voltage_TPS55288(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount)
{
	uint8_t TestOutputBuffer[13];
	switch (Increase) {
	case 1:
		sprintf((char *)TestOutputBuffer, "Increasing\r\n");
		PrintOutputBuffer(TestOutputBuffer);
		break;
	case 0:
		sprintf((char *)TestOutputBuffer, "Decreasing\r\n");
		PrintOutputBuffer(TestOutputBuffer);
		break;
	}
}

void TestInitialize_HillClimbing(uint32_t Number_of_Converters, TestSolarPanel_hc *Panels)
{
	TestConverter_Number = Number_of_Converters;
	int32_t Voltage;
	int32_t Current;
	for (int i = 0; i < TestConverter_Number; i++) {
		TestGet_Sensor_Values_for_Panel_hc(i, &Voltage, &Current);
		Panels[i].Current_Current = Current;
		Panels[i].Current_Voltage = Voltage;
		Panels[i].Previous_Current = Current;
		Panels[i].Previous_Voltage = Voltage;
		Panels[i].Shading_Conditions = NOT_SHADED;
		Panels[i].At_MPP = NOT_AT_MPP;
	}
}

void TestGet_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, int32_t *Voltage, int32_t *Current)
{
	TestRead_Sensor_ValuesACS37800(Converter_Index, Voltage, Current);
}

int32_t TestCalculate_Power_hc(int32_t Voltage, int32_t Current)
{
	return Voltage * Current;
}

int32_t TestCalculate_Average_Current_hc(TestSolarPanel_hc *Panels)
{
	uint32_t Total_Current = 0;
	for (int i = 0; i < TestConverter_Number; i++) {
		Total_Current = Total_Current + Panels[i].Current_Current;
	}
	return Total_Current / TestConverter_Number;
}

void TestChange_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount)
{
	TestUpdate_Reference_Voltage_TPS55288(Converter_Index, Increase, Change_Amount);
}

void TestUpdate_Panel_Parameters_hc(uint8_t Converter_Index, int32_t New_Voltage, int32_t New_Current, uint8_t At_mpp, uint8_t Partially_shaded, TestSolarPanel_hc *Panels)
{
	Panels[Converter_Index].Previous_Current = Panels[Converter_Index].Current_Current;
	Panels[Converter_Index].Previous_Voltage = Panels[Converter_Index].Current_Voltage;
	Panels[Converter_Index].Current_Current = New_Current;
	Panels[Converter_Index].Current_Voltage = New_Voltage;
	Panels[Converter_Index].Shading_Conditions = Partially_shaded;
	Panels[Converter_Index].At_MPP = At_mpp;
}

uint8_t TestCheck_if_All_are_MPPT_hc(TestSolarPanel_hc *Panels)
{
	for (int i = 0; i < TestConverter_Number; i++) {
		if (!(Panels[i].At_MPP)) {
			return 0;
		}
	}
	return 1;
}

void TestUpdate_Panel_State_hc(uint8_t Converter_Index, TestSolarPanel_hc *Panels)
{
	int32_t Previous_Voltage = Panels[Converter_Index].Current_Voltage;
	int32_t Previous_Current = Panels[Converter_Index].Current_Current;
	int32_t Voltage;
	int32_t Current;
	TestGet_Sensor_Values_for_Panel_hc(Converter_Index, &Voltage, &Current);
	int32_t Power = TestCalculate_Power_hc(Voltage, Current);
	int32_t Previous_Power = TestCalculate_Power_hc(Previous_Voltage, Previous_Current);
	uint8_t Partially_shaded = Panels[Converter_Index].Shading_Conditions;
	uint8_t At_mpp = Panels[Converter_Index].At_MPP;
	if (Partially_shaded) {
		if (Voltage - Previous_Voltage > Partial_Shade_Threshold) {
			Partially_shaded = NOT_SHADED;
			At_mpp = NOT_AT_MPP;
		}
		else {
			TestChange_Panel_Values_hc(Converter_Index, INCREASE, 8);
		}
	}
	else {
		if ((abs(Power - Previous_Power) <= Power_Threshold) && (abs(Voltage - Previous_Voltage) <= Voltage_Threshold)) {
			At_mpp = AT_MPP;
			if (TestCheck_if_All_are_MPPT_hc(Panels)) {
				uint32_t Average_Current = TestCalculate_Average_Current_hc(Panels);
				if ((Average_Current - Current) > Current_Threshold) {
					Partially_shaded = SHADED;
				}
			}
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) > 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) < 0)) {
			TestChange_Panel_Values_hc(Converter_Index, INCREASE, 8);
			At_mpp = NOT_AT_MPP;
		}
		else if (((Power - Previous_Power > 0) && (Voltage - Previous_Voltage) < 0) || ((Power - Previous_Power < 0) && (Voltage - Previous_Voltage) > 0)) {
			TestChange_Panel_Values_hc(Converter_Index, DECREASE, 8);
			At_mpp = NOT_AT_MPP;
		}
	}
	TestUpdate_Panel_Parameters_hc(Converter_Index, Voltage, Current, At_mpp, Partially_shaded, Panels);
}

void TestUpdate_All_Panel_States_hc(TestSolarPanel_hc *Panels)
{
	for (int i = 0; i < TestConverter_Number; i++) {
		TestUpdate_Panel_State_hc(i, Panels);
	}
}

