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

/**
 * @brief Initializes the solar panels with initial measurements
 * @retval None.
 * @param Number_of_Converters: number of converters in the system
 * @param SolarPanels: array of solar panels and data collected
 */
void Initialize_HillClimbing(uint32_t Number_of_Converters, SolarPanel *SolarPanels);

/**
 * @brief Retrieves sensor data for voltage and current
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Voltage: Pointer to store voltage
 * @param Current: Pointer to store current
 */
void Get_Sensor_Values_for_Panel_hc(uint8_t Converter_Index, float *Voltage, float *Current);

/**
 * @brief Calculates the average current of all the panels
 * @retval average current.
 * @param SolarPanels: array of solar panels and data collected
 */
float Calculate_Average_Current_hc(SolarPanel *SolarPanels);

/**
 * @brief Calculates power with given voltage and current
 * @retval power.
 * @param Voltage: voltage value
 * @param Current: current value
 */
float Calculate_Power_hc(float Voltage, float Current);

/**
 * @brief Checks if all panels are at mpp
 * @retval Boolean of whether all panels are at mpp.
 * @param SolarPanels: array of solar panels and data collected
 */
uint8_t Check_if_All_are_MPPT_hc(SolarPanel *SolarPanels);

/**
 * @brief Updates reference voltage
 * @retval None.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param Increase: boolean to determine whether to add or subtract from the previous reference voltage
 * @param Change_Amount: amount to change reference voltage by
 */
void Change_Panel_Values_hc(uint8_t Converter_Index, uint8_t Increase, uint8_t Change_Amount);

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
void Update_Panel_Parameters_hc(uint8_t Converter_Index, float New_Voltage, float New_Current, uint8_t At_mpp, uint8_t Partially_shaded, SolarPanel *SolarPanels);

/**
 * @brief Updates values for single converter
 * @retval The desired Vout of the panel.
 * @param Converter_Index: The arbitrary index of the converter in the system
 * @param SolarPanels: array of solar panels and data collected
 */
float Update_Panel_State_hc(uint8_t Converter_Index, SolarPanel *SolarPanels);

/**
 * @brief Updates values for all converters in the system
 * @retval None.
 * @param SolarPanels: array of solar panels and data collected
 */
void Update_All_Panel_States_hc(SolarPanel *SolarPanels);

#endif /* INC_HILLCLIMBING_H_ */
