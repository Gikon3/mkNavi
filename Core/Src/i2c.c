/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
extern osMessageQueueId_t qI2cHandle;

I2C_IF i2cIF_Osc;
I2C_IF i2cIF_USB;
I2C_IF i2cIF_Bat;
I2C_IF i2cIF_LIS331HH;    // Акселерометр
I2C_IF i2cIF_LIS331DLH;   // Акселерометр
I2C_IF i2cIF_I3G4250D;    // Гироскоп
I2C_IF i2cIF_HMC5883L;    // Магнитометр
I2C_IF i2cIF_BMP280;      // Барометр

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
size_t i2c_scan_devs(I2C_HandleTypeDef* i2c, uint8_t* devs, size_t size)
{
  HAL_StatusTypeDef stat;
  size_t count = 0;
  for (uint16_t i = 0; i < 128; ++i) {
    stat = HAL_I2C_IsDeviceReady(i2c, i << 1, 2, 5);
    if (stat == HAL_OK) {
      devs[count++] = i;
    }
    if (count >= size) {
      break;
    }
  }
  return count;
}

void i2c_init()
{
  i2cIF_Osc.i2c = &hi2c1;
  i2cIF_Osc.devAddr = I2C_OSC_ADR;
  i2cIF_Osc.timeout = 5;
  i2cIF_Osc.status = HAL_OK;
  i2cIF_Osc.queue = qI2cHandle;

  i2cIF_USB.i2c = &hi2c1;
  i2cIF_USB.devAddr = I2C_USB_ADR;
  i2cIF_USB.timeout = 5;
  i2cIF_USB.status = HAL_OK;
  i2cIF_USB.queue = qI2cHandle;

  i2cIF_Bat.i2c = &hi2c1;
  i2cIF_Bat.devAddr = I2C_BAT_ADR;
  i2cIF_Bat.timeout = 5;
  i2cIF_Bat.status = HAL_OK;
  i2cIF_Bat.queue = qI2cHandle;

  i2cIF_LIS331HH.i2c = &hi2c1;
  i2cIF_LIS331HH.devAddr = I2C_LIS331HH_ADR;
  i2cIF_LIS331HH.timeout = 5;
  i2cIF_LIS331HH.status = HAL_OK;
  i2cIF_LIS331HH.queue = qI2cHandle;

  i2cIF_LIS331DLH.i2c = &hi2c1;
  i2cIF_LIS331DLH.devAddr = I2C_LIS331DLH_ADR;
  i2cIF_LIS331DLH.timeout = 5;
  i2cIF_LIS331DLH.status = HAL_OK;
  i2cIF_LIS331DLH.queue = qI2cHandle;

  i2cIF_I3G4250D.i2c = &hi2c1;
  i2cIF_I3G4250D.devAddr = I2C_I3G4250D_ADR;
  i2cIF_I3G4250D.timeout = 5;
  i2cIF_I3G4250D.status = HAL_OK;
  i2cIF_I3G4250D.queue = qI2cHandle;

  i2cIF_HMC5883L.i2c = &hi2c1;
  i2cIF_HMC5883L.devAddr = I2C_HMC5883L_ADR;
  i2cIF_HMC5883L.timeout = 5;
  i2cIF_HMC5883L.status = HAL_OK;
  i2cIF_HMC5883L.queue = qI2cHandle;

  i2cIF_BMP280.i2c = &hi2c1;
  i2cIF_BMP280.devAddr = I2C_BMP280_ADR;
  i2cIF_BMP280.timeout = 5;
  i2cIF_BMP280.status = HAL_OK;
  i2cIF_BMP280.queue = qI2cHandle;
}

HAL_StatusTypeDef i2c_tx(I2C_IF* i2cIf, uint8_t const* data, uint16_t size)
{
  HAL_StatusTypeDef status;
  status = HAL_I2C_Master_Transmit(i2cIf->i2c, i2cIf->devAddr << 1, (uint8_t*)data, size, i2cIf->timeout * size);
  i2cIf->status = status;
  return status;
}

HAL_StatusTypeDef i2c_rx(I2C_IF* i2cIf, uint8_t* data, uint16_t size)
{
  HAL_StatusTypeDef status;
  status = HAL_I2C_Master_Receive(i2cIf->i2c, i2cIf->devAddr << 1, data, size, i2cIf->timeout * size);
  i2cIf->status = status;
  return status;
}

HAL_StatusTypeDef i2c_is_ready(I2C_IF* i2cIf)
{
  HAL_StatusTypeDef status;
  status = HAL_I2C_IsDeviceReady(i2cIf->i2c, i2cIf->devAddr << 1, 1, i2cIf->timeout);
  i2cIf->status = status;
  return status;
}
/* USER CODE END 1 */
