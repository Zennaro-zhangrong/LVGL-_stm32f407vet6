/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define TEST_LED_Pin GPIO_PIN_13
#define TEST_LED_GPIO_Port GPIOC
#define LCD_RE_Pin GPIO_PIN_0
#define LCD_RE_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_1
#define LCD_CS_GPIO_Port GPIOC
#define LCD_MISO_Pin GPIO_PIN_2
#define LCD_MISO_GPIO_Port GPIOC
#define LCD_MOSI_Pin GPIO_PIN_3
#define LCD_MOSI_GPIO_Port GPIOC
#define Battery_Level_Pin GPIO_PIN_1
#define Battery_Level_GPIO_Port GPIOA
#define X_L_Pin GPIO_PIN_2
#define X_L_GPIO_Port GPIOA
#define Y_L_Pin GPIO_PIN_3
#define Y_L_GPIO_Port GPIOA
#define X_R_Pin GPIO_PIN_4
#define X_R_GPIO_Port GPIOA
#define PSRAM_SCK_Pin GPIO_PIN_5
#define PSRAM_SCK_GPIO_Port GPIOA
#define PSRAM_MISO_Pin GPIO_PIN_6
#define PSRAM_MISO_GPIO_Port GPIOA
#define PSRAM_MOSI_Pin GPIO_PIN_7
#define PSRAM_MOSI_GPIO_Port GPIOA
#define PSRAM_CS_Pin GPIO_PIN_4
#define PSRAM_CS_GPIO_Port GPIOC
#define Y_R_Pin GPIO_PIN_0
#define Y_R_GPIO_Port GPIOB
#define LCD_SCK_Pin GPIO_PIN_10
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_12
#define LCD_LED_GPIO_Port GPIOD
#define NRF_CE_Pin GPIO_PIN_13
#define NRF_CE_GPIO_Port GPIOD
#define NRF_CS_Pin GPIO_PIN_14
#define NRF_CS_GPIO_Port GPIOD
#define NRF_IRQ_Pin GPIO_PIN_15
#define NRF_IRQ_GPIO_Port GPIOD
#define NRF_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define NRF_SCK_Pin GPIO_PIN_10
#define NRF_SCK_GPIO_Port GPIOC
#define NRF_MISO_Pin GPIO_PIN_11
#define NRF_MISO_GPIO_Port GPIOC
#define NRF_MOSI_Pin GPIO_PIN_12
#define NRF_MOSI_GPIO_Port GPIOC
#define Fully_Charged_Pin GPIO_PIN_0
#define Fully_Charged_GPIO_Port GPIOD
#define Chargiing_Pin GPIO_PIN_1
#define Chargiing_GPIO_Port GPIOD
#define SW_R_Pin GPIO_PIN_2
#define SW_R_GPIO_Port GPIOD
#define SW_R_EXTI_IRQn EXTI2_IRQn
#define SW_L_Pin GPIO_PIN_3
#define SW_L_GPIO_Port GPIOD
#define SW_L_EXTI_IRQn EXTI3_IRQn
#define SD_CS_Pin GPIO_PIN_6
#define SD_CS_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_7
#define LCD_RST_GPIO_Port GPIOD
#define CTP_RST_Pin GPIO_PIN_4
#define CTP_RST_GPIO_Port GPIOB
#define CTP_IRQ_Pin GPIO_PIN_5
#define CTP_IRQ_GPIO_Port GPIOB
#define CTP_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define CTP_SCL_Pin GPIO_PIN_6
#define CTP_SCL_GPIO_Port GPIOB
#define CTP_SDA_Pin GPIO_PIN_7
#define CTP_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define CCM __attribute__((section(".ccmram")))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
