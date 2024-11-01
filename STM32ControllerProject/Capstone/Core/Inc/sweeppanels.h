/*
 * sweeppanels.h
 *
 *  Created on: Oct 21, 2024
 *      Author: eizak
 */

#ifndef INC_SWEEPPANELS_H_
#define INC_SWEEPPANELS_H_

#include "main.h"
#include "buckboostTPS55288.h"
#include "sensorACS37800.h"

/**
 * @brief Sets all the output voltages to 0
 * @retval None.
 * @param Number_of_Converters: the number of converters in the system
 */
void Reset_Converters(uint8_t Number_of_Converters);

/**
 * @brief Increases the output voltage of each converter
 * @retval None.
 * @param Increase_Rate: the amount to increase the reference voltage by
 */
void Increase_Converter_Reference_Voltage(uint8_t Increase_Rate);

#endif /* INC_SWEEPPANELS_H_ */
