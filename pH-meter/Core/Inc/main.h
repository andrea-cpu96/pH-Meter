/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

extern ADC_HandleTypeDef hadc;
extern SPI_HandleTypeDef hspi1;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SEL_Pin GPIO_PIN_13
#define SEL_GPIO_Port GPIOC
#define RIGHT_Pin GPIO_PIN_14
#define RIGHT_GPIO_Port GPIOC
#define LEFT_Pin GPIO_PIN_15
#define LEFT_GPIO_Port GPIOC
#define UP_Pin GPIO_PIN_0
#define UP_GPIO_Port GPIOA
#define DOWN_Pin GPIO_PIN_1
#define DOWN_GPIO_Port GPIOA
#define ADC1_IN8_Pin GPIO_PIN_0
#define ADC1_IN8_GPIO_Port GPIOB
#define ST7735_AD_Pin GPIO_PIN_1
#define ST7735_AD_GPIO_Port GPIOB
#define ST7735_RESET_Pin GPIO_PIN_2
#define ST7735_RESET_GPIO_Port GPIOB
#define ST7735_CS_Pin GPIO_PIN_10
#define ST7735_CS_GPIO_Port GPIOB
#define OPAMP_CALIB_Pin GPIO_PIN_12
#define OPAMP_CALIB_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
