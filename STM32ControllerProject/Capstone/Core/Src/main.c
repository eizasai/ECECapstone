/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFERLENGTH 				64
#define DATALENGTH					12
#define DEBUG_CONFIGURATION_MODE 	1
#define NUMBER_OF_CONVERTERS 		3
#define PERTURB_AND_OBSERVE 		1
#define HILL_CLIMBING 				2
#define SMART_ALGORITHM 			3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t ReceiveCharacter;
uint8_t OutputBuffer[BUFFERLENGTH];
uint8_t test_variable = 0;
HAL_StatusTypeDef HAL_Status;
uint8_t Current_Mode = PERTURB_AND_OBSERVE;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//	uint8_t Single_Converter_Index = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
//  if (DEBUG_CONFIGURATION_MODE) { // Configures each of the buck converter device addresses to prevent conflicts
//	  Configure_Slave_AddressTPS55288(Single_Converter_Index);
//  }
  sprintf((char *)OutputBuffer, "Communication Started\r\n");
  PrintOutputBuffer(OutputBuffer);
  sprintf((char *)OutputBuffer, "Receiving Character\r\n");
  PrintOutputBuffer(OutputBuffer);
  HAL_Status = HAL_UART_Receive_IT(&huart2, &ReceiveCharacter, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (test_variable) {
	  	  test_variable = 0;
	  	  HAL_Status = HAL_UART_Receive_IT(&huart2, &ReceiveCharacter, 1);
	  }
	  HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  test_variable = 1;
  HAL_Status = HAL_UART_Receive_IT(&huart2, &ReceiveCharacter, 1);
  if (ReceiveCharacter == '0') {
	  uint32_t Voltage;
	  uint32_t Current;
	  uint32_t Power;
	  //Read_Sensor_ValuesACS37800(NUMBER_OF_CONVERTERS - 1, &Voltage, &Current);
	  Get_Sensor_Values_for_Panel_hc_test(NUMBER_OF_CONVERTERS - 1, &Voltage, &Current);
	  Power = Calculate_Power_hc(Voltage, Current);
	  for (int i = 0; i < 12; i++) {
		  if (i / 3 == 0) {
			  OutputBuffer[i] = (Voltage >> (i % 4)) & 0xff;
		  }
		  else if (i / 3 == 1) {
			  OutputBuffer[i] = (Current >> (i % 4)) & 0xff;
		  }
		  else {
			  OutputBuffer[i] = (Power >> (i % 4)) & 0xff;
		  }
	  }
	  OutputBuffer[DATALENGTH] = '\r';
	  OutputBuffer[DATALENGTH + 1] = '\n';
	  OutputBuffer[DATALENGTH + 2] = '\0';
	  PrintOutputData(OutputBuffer);
  }
  else if (ReceiveCharacter == '1') {
	  sprintf((char *)OutputBuffer, "P&O\r\n");
	  PrintOutputBuffer(OutputBuffer);
  }
  else if (ReceiveCharacter == '2') {
	  sprintf((char *)OutputBuffer, "HillClimbing\r\n");
	  PrintOutputBuffer(OutputBuffer);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
