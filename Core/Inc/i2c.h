/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "queue_node.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN Private defines */
#define I2C_LIS331HH_ADR  0x18
#define I2C_LIS331DLH_ADR 0x18
#define I2C_USB_ADR       0x47
#define I2C_OSC_ADR       0x6A
#define I2C_BAT_ADR       0x6B
#define I2C_I3G4250D_ADR  0x68
#define I2C_HMC5883L_ADR  0x1E
#define I2C_BMP280_ADR    0x76 /* 0x77 */

typedef struct
{
  I2C_HandleTypeDef* i2c;
  uint8_t devAddr;
  uint32_t timeout;
  HAL_StatusTypeDef status;
  osMessageQueueId_t queue;
} I2C_IF;

typedef struct
{
  I2C_IF* i2c;
  TranType type;
  uint8_t address;
  uint8_t* data;
  uint16_t dataSize;
  osSemaphoreId_t ready;
} I2cQNode;

extern I2C_IF i2cIF_Osc;
extern I2C_IF i2cIF_USB;
extern I2C_IF i2cIF_Bat;
extern I2C_IF i2cIF_LIS331HH;     // Акселерометр
extern I2C_IF i2cIF_LIS331DLH;    // Акселерометр
extern I2C_IF i2cIF_I3G4250D;     // Гироскоп
extern I2C_IF i2cIF_HMC5883L;     // Магнитометр
extern I2C_IF i2cIF_BMP280;       // Барометр
/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
size_t i2c_scan_devs(I2C_HandleTypeDef* i2c, uint8_t* devs, size_t size);
void i2c_init();
HAL_StatusTypeDef i2c_tx(I2C_IF* i2cIf, uint8_t const* data, uint16_t size);
HAL_StatusTypeDef i2c_rx(I2C_IF* i2cIf, uint8_t* data, uint16_t size);
HAL_StatusTypeDef i2c_is_ready(I2C_IF* i2cIf);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

