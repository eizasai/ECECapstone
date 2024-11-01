/*
 * test_hillclimbing.h
 *
 *  Created on: Oct 25, 2024
 *      Author: eizak
 */

#ifndef INC_TEST_HILLCLIMBING_H_
#define INC_TEST_HILLCLIMBING_H_

#include "usart.h"
#include "main.h"

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

typedef struct TestSolarPanel_hc_struct {
	int32_t Previous_Voltage;
	int32_t Current_Voltage;
	int32_t Previous_Current;
	int32_t Current_Current;
	uint8_t Shading_Conditions;
	uint8_t At_MPP;
} TestSolarPanel_hc;

void Update_Test_Sensor_Values_hc(int32_t Voltage, int32_t Current);

void TestRead_Sensor_ValuesACS37800(uint8_t Converter_Index, int32_t *Voltage, int32_t *Current);

void TestUpdate_Reference_Voltage_TPS55288(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount);

/**
 * @brief Initializes the solar panels with initial measurements
 * @retval None.
 * @param Number_of_Converters: number of converters in the system
 * @param SolarPanels: array of solar panels and data collected
 */
void TestInitialize_HillClimbing(uint32_t Number_of_Converters, TestSolarPanel_hc *Panels);

/**
 * @brief Retrieves sensor data for voltage and current
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Voltage: Pointer to store voltage
 * @param Current: Pointer to store current
 */
void TestGet_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, int32_t *Voltage, int32_t *Current);

/**
 * @brief Calculates the average current of all the panels
 * @retval average current.
 * @param SolarPanels: array of solar panels and data collected
 */
int32_t TestCalculate_Average_Current_hc(TestSolarPanel_hc *Panels);

/**
 * @brief Calculates power with given voltage and current
 * @retval power.
 * @param Voltage: voltage value
 * @param Current: current value
 */
int32_t TestCalculate_Power_hc(int32_t Voltage, int32_t Current);

/**
 * @brief Updates reference voltage
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Increase: boolean to determine whether to add or subtract from the previous reference voltage
 * @param Change_Amount: amount to change reference voltage by
 */
void TestChange_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount);

/**
 * @brief Checks if all panels are at mpp
 * @retval Boolean of whether all panels are at mpp.
 * @param SolarPanels: array of solar panels and data collected
 */
uint8_t TestCheck_if_All_are_MPPT_hc(TestSolarPanel_hc *Panels);

/**
 * @brief Updates the panel values in the solar panel array
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param New_Voltage: set as current voltage
 * @param New_Current: set as current current
 * @param At_mpp: boolean of whether the converter is at mpp
 * @param Partially_shaded: boolean of whether the converter is partially shaded
 * @param SolarPanels: array of solar panels and data collected
 */
void TestUpdate_Panel_Parameters_hc(uint8_t Converter_Index, int32_t New_Voltage, int32_t New_Current, uint8_t At_mpp, uint8_t Partially_shaded, TestSolarPanel_hc *Panels);

/**
 * @brief Updates values for single converter
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param SolarPanels: array of solar panels and data collected
 */
void TestUpdate_Panel_State_hc(uint8_t Converter_Index, TestSolarPanel_hc *Panels);

/**
 * @brief Updates values for all converters in the system
 * @retval None.
 * @param SolarPanels: array of solar panels and data collected
 */
void TestUpdate_All_Panel_States_hc(TestSolarPanel_hc *Panels);

#endif /* INC_TEST_HILLCLIMBING_H_ */
