/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define LED_R_Pin GPIO_PIN_2
#define LED_R_GPIO_Port GPIOE
#define LED_G_Pin GPIO_PIN_3
#define LED_G_GPIO_Port GPIOE
#define TFT_CS_Pin GPIO_PIN_4
#define TFT_CS_GPIO_Port GPIOE
#define TFT_INT_Pin GPIO_PIN_5
#define TFT_INT_GPIO_Port GPIOE
#define TFT_PD_Pin GPIO_PIN_6
#define TFT_PD_GPIO_Port GPIOE
#define TFT_GPIO0_Pin GPIO_PIN_0
#define TFT_GPIO0_GPIO_Port GPIOC
#define TFT_GPIO1_Pin GPIO_PIN_1
#define TFT_GPIO1_GPIO_Port GPIOC
#define TFT_GPIO2_Pin GPIO_PIN_2
#define TFT_GPIO2_GPIO_Port GPIOC
#define TFT_GPIO3_Pin GPIO_PIN_3
#define TFT_GPIO3_GPIO_Port GPIOC
#define BARO_INT_Pin GPIO_PIN_4
#define BARO_INT_GPIO_Port GPIOA
#define CPP_GPIO7_Pin GPIO_PIN_0
#define CPP_GPIO7_GPIO_Port GPIOB
#define BARO_ADR_Pin GPIO_PIN_1
#define BARO_ADR_GPIO_Port GPIOB
#define CPP_GPIO6_Pin GPIO_PIN_9
#define CPP_GPIO6_GPIO_Port GPIOE
#define CPP_GPIO5_Pin GPIO_PIN_10
#define CPP_GPIO5_GPIO_Port GPIOE
#define CPP_GPIO4_Pin GPIO_PIN_11
#define CPP_GPIO4_GPIO_Port GPIOE
#define CPP_GPIO3_Pin GPIO_PIN_12
#define CPP_GPIO3_GPIO_Port GPIOE
#define CPP_GPIO2_Pin GPIO_PIN_13
#define CPP_GPIO2_GPIO_Port GPIOE
#define CPP_S1_Pin GPIO_PIN_14
#define CPP_S1_GPIO_Port GPIOE
#define LIPO_VBAT_EN_Pin GPIO_PIN_15
#define LIPO_VBAT_EN_GPIO_Port GPIOE
#define USB_INT_Pin GPIO_PIN_8
#define USB_INT_GPIO_Port GPIOD
#define USB_PORT_ROLE_Pin GPIO_PIN_9
#define USB_PORT_ROLE_GPIO_Port GPIOD
#define GL_ON_OFF_Pin GPIO_PIN_10
#define GL_ON_OFF_GPIO_Port GPIOD
#define GL_1PPS_Pin GPIO_PIN_11
#define GL_1PPS_GPIO_Port GPIOD
#define GL_1PPS_EXTI_IRQn EXTI15_10_IRQn
#define MAGN_SPI_CS_Pin GPIO_PIN_12
#define MAGN_SPI_CS_GPIO_Port GPIOD
#define MAGN_IRQ_OUT2_Pin GPIO_PIN_13
#define MAGN_IRQ_OUT2_GPIO_Port GPIOD
#define MAGN_IRQ_OUT1_Pin GPIO_PIN_14
#define MAGN_IRQ_OUT1_GPIO_Port GPIOD
#define BTN_INT_Pin GPIO_PIN_15
#define BTN_INT_GPIO_Port GPIOD
#define BTN_INT_EXTI_IRQn EXTI15_10_IRQn
#define BTN_KILL_Pin GPIO_PIN_6
#define BTN_KILL_GPIO_Port GPIOC
#define BAT_INT_N_Pin GPIO_PIN_7
#define BAT_INT_N_GPIO_Port GPIOC
#define BAT_PG_N_Pin GPIO_PIN_8
#define BAT_PG_N_GPIO_Port GPIOC
#define BAT_CE_N_Pin GPIO_PIN_9
#define BAT_CE_N_GPIO_Port GPIOC
#define BLTH_RESET_Pin GPIO_PIN_8
#define BLTH_RESET_GPIO_Port GPIOA
#define FLASH_HOLD_Pin GPIO_PIN_0
#define FLASH_HOLD_GPIO_Port GPIOD
#define FLASH_WP_Pin GPIO_PIN_1
#define FLASH_WP_GPIO_Port GPIOD
#define FLASH_SPI_CS_Pin GPIO_PIN_2
#define FLASH_SPI_CS_GPIO_Port GPIOD
#define GYRO_IRQ_OUT1_Pin GPIO_PIN_3
#define GYRO_IRQ_OUT1_GPIO_Port GPIOD
#define GYRO_IRQ_OUT2_Pin GPIO_PIN_4
#define GYRO_IRQ_OUT2_GPIO_Port GPIOD
#define GYRO_SPI_CS_Pin GPIO_PIN_5
#define GYRO_SPI_CS_GPIO_Port GPIOD
#define ACCEL_IRQ_OUT1_Pin GPIO_PIN_6
#define ACCEL_IRQ_OUT1_GPIO_Port GPIOD
#define ACCEL_IRQ_OUT2_Pin GPIO_PIN_7
#define ACCEL_IRQ_OUT2_GPIO_Port GPIOD
#define SD_CS_Pin GPIO_PIN_4
#define SD_CS_GPIO_Port GPIOB
#define ACCEL_ADR_Pin GPIO_PIN_5
#define ACCEL_ADR_GPIO_Port GPIOB
#define SPI1_SS_Pin GPIO_PIN_8
#define SPI1_SS_GPIO_Port GPIOB
#define CLK_SWITCH_V2_Pin GPIO_PIN_9
#define CLK_SWITCH_V2_GPIO_Port GPIOB
#define CLK_SWITCH_V1_Pin GPIO_PIN_0
#define CLK_SWITCH_V1_GPIO_Port GPIOE
#define OSC_EN_Pin GPIO_PIN_1
#define OSC_EN_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
