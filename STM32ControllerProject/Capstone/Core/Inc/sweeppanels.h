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

void Reset_Converters(uint8_t Number_of_Converters);

void Increase_Converter_Reference_Voltage(uint8_t Increase_Rate);

#endif /* INC_SWEEPPANELS_H_ */
