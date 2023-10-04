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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define K_COL_1_Pin GPIO_PIN_0
#define K_COL_1_GPIO_Port GPIOC
#define K_COL_2_Pin GPIO_PIN_1
#define K_COL_2_GPIO_Port GPIOC
#define K_COL_3_Pin GPIO_PIN_2
#define K_COL_3_GPIO_Port GPIOC
#define K_COL_4_Pin GPIO_PIN_3
#define K_COL_4_GPIO_Port GPIOC
#define DS18B20_Pin GPIO_PIN_0
#define DS18B20_GPIO_Port GPIOA
#define K_ROW_3_Pin GPIO_PIN_1
#define K_ROW_3_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define K_ROW_2_Pin GPIO_PIN_4
#define K_ROW_2_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_5
#define LED_GPIO_Port GPIOA
#define O_AC_6_Pin GPIO_PIN_6
#define O_AC_6_GPIO_Port GPIOA
#define O_DC_2_Pin GPIO_PIN_4
#define O_DC_2_GPIO_Port GPIOC
#define O_AC_4_Pin GPIO_PIN_5
#define O_AC_4_GPIO_Port GPIOC
#define K_ROW_1_Pin GPIO_PIN_0
#define K_ROW_1_GPIO_Port GPIOB
#define O_AC_8_Pin GPIO_PIN_12
#define O_AC_8_GPIO_Port GPIOB
#define O_H_1_Pin GPIO_PIN_13
#define O_H_1_GPIO_Port GPIOB
#define O_H_2_Pin GPIO_PIN_14
#define O_H_2_GPIO_Port GPIOB
#define O_H_3_Pin GPIO_PIN_15
#define O_H_3_GPIO_Port GPIOB
#define O_AC_3_Pin GPIO_PIN_6
#define O_AC_3_GPIO_Port GPIOC
#define O_AC_2_Pin GPIO_PIN_8
#define O_AC_2_GPIO_Port GPIOC
#define O_AC_1_Pin GPIO_PIN_9
#define O_AC_1_GPIO_Port GPIOC
#define O_DC_1_Pin GPIO_PIN_10
#define O_DC_1_GPIO_Port GPIOA
#define O_AC_7_Pin GPIO_PIN_11
#define O_AC_7_GPIO_Port GPIOA
#define O_AC_5_Pin GPIO_PIN_12
#define O_AC_5_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define K_ROW_5_Pin GPIO_PIN_15
#define K_ROW_5_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_2
#define BUZZER_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define O_DC_4_Pin GPIO_PIN_4
#define O_DC_4_GPIO_Port GPIOB
#define O_DC_3_Pin GPIO_PIN_5
#define O_DC_3_GPIO_Port GPIOB
#define K_ROW_4_Pin GPIO_PIN_7
#define K_ROW_4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
