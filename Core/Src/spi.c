/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
extern osMessageQueueId_t qSpiHandle;
extern osSemaphoreId_t semSpiHandle;

SPI_IF spiIF_TFT;
SPI_IF spiIF_Flash;
SPI_IF spiIF_SD;
SPI_IF spiIF_L3GD20H;
SPI_IF spiIF_LIS3MDL;
SPI_IF spiIF_MT25Q;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef hdma_spi3_tx;
DMA_HandleTypeDef hdma_spi3_rx;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
/* SPI3 init function */
void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
  else if(spiHandle->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspInit 0 */

  /* USER CODE END SPI3_MspInit 0 */
    /* SPI3 clock enable */
    __HAL_RCC_SPI3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**SPI3 GPIO Configuration
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* SPI3 DMA Init */
    /* SPI3_TX Init */
    hdma_spi3_tx.Instance = DMA1_Stream7;
    hdma_spi3_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_tx.Init.Mode = DMA_NORMAL;
    hdma_spi3_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi3_tx);

    /* SPI3_RX Init */
    hdma_spi3_rx.Instance = DMA1_Stream0;
    hdma_spi3_rx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_rx.Init.Mode = DMA_NORMAL;
    hdma_spi3_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_spi3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi3_rx);

  /* USER CODE BEGIN SPI3_MspInit 1 */

  /* USER CODE END SPI3_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(spiHandle->Instance==SPI3)
  {
  /* USER CODE BEGIN SPI3_MspDeInit 0 */

  /* USER CODE END SPI3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI3_CLK_DISABLE();

    /**SPI3 GPIO Configuration
    PC10     ------> SPI3_SCK
    PC11     ------> SPI3_MISO
    PC12     ------> SPI3_MOSI
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);

    /* SPI3 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmatx);
    HAL_DMA_DeInit(spiHandle->hdmarx);
  /* USER CODE BEGIN SPI3_MspDeInit 1 */

  /* USER CODE END SPI3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void spi_init()
{
  spiIF_TFT.spi = &hspi3;
  spiIF_TFT.csGpioPort = TFT_CS_GPIO_Port;
  spiIF_TFT.csGpioPin = TFT_CS_Pin;
  spiIF_TFT.prescaler = SPI_BAUDRATEPRESCALER_128; /* SPI_BAUDRATEPRESCALER_2 later */
  spiIF_TFT.timeout = 10;
  spiIF_TFT.queue = qSpiHandle;
  spiIF_TFT.ready = semSpiHandle;

  spiIF_Flash.spi = &hspi3;
  spiIF_Flash.csGpioPort = FLASH_SPI_CS_GPIO_Port;
  spiIF_Flash.csGpioPin = FLASH_SPI_CS_Pin;
  spiIF_Flash.prescaler = SPI_BAUDRATEPRESCALER_2;
  spiIF_Flash.timeout = 10;
  spiIF_Flash.queue = qSpiHandle;
  spiIF_Flash.ready = semSpiHandle;

  spiIF_SD.spi = &hspi3;
  spiIF_SD.csGpioPort = SD_CS_GPIO_Port;
  spiIF_SD.csGpioPin = SD_CS_Pin;
  spiIF_SD.prescaler = SPI_BAUDRATEPRESCALER_128; /* SPI_BAUDRATEPRESCALER_8 later */
  spiIF_SD.timeout = 10;
  spiIF_SD.queue = qSpiHandle;
  spiIF_SD.ready = semSpiHandle;

  spiIF_L3GD20H.spi = &hspi3;
  spiIF_L3GD20H.csGpioPort = GYRO_SPI_CS_GPIO_Port;
  spiIF_L3GD20H.csGpioPin = GYRO_SPI_CS_Pin;
  spiIF_L3GD20H.prescaler = SPI_BAUDRATEPRESCALER_8;
  spiIF_L3GD20H.timeout = 10;
  spiIF_L3GD20H.queue = qSpiHandle;
  spiIF_L3GD20H.ready = semSpiHandle;

  spiIF_LIS3MDL.spi = &hspi3;
  spiIF_LIS3MDL.csGpioPort = MAGN_SPI_CS_GPIO_Port;
  spiIF_LIS3MDL.csGpioPin = MAGN_SPI_CS_Pin;
  spiIF_LIS3MDL.prescaler = SPI_BAUDRATEPRESCALER_8;
  spiIF_LIS3MDL.timeout = 10;
  spiIF_LIS3MDL.queue = qSpiHandle;
  spiIF_LIS3MDL.ready = semSpiHandle;

  spiIF_MT25Q.spi = &hspi3;
  spiIF_MT25Q.csGpioPort = FLASH_SPI_CS_GPIO_Port;
  spiIF_MT25Q.csGpioPin = FLASH_SPI_CS_Pin;
  spiIF_MT25Q.prescaler = SPI_BAUDRATEPRESCALER_2;
  spiIF_MT25Q.timeout = 10;
  spiIF_MT25Q.queue = qSpiHandle;
  spiIF_MT25Q.ready = semSpiHandle;
}

void spi_decs(SPI_IF* spiIF)
{
  HAL_GPIO_WritePin(spiIF->csGpioPort, spiIF->csGpioPin, SET);
}

void spi_cs(SPI_IF* spiIF)
{
  HAL_GPIO_WritePin(spiIF->csGpioPort, spiIF->csGpioPin, RESET);
}

inline void spi_modify_presc(SPI_IF* spiIf)
{
  MODIFY_REG(spiIf->spi->Instance->CR1, SPI_BAUDRATEPRESCALER_256, spiIf->prescaler);
  while ((spiIf->spi->Instance->CR1 & SPI_BAUDRATEPRESCALER_256) != spiIf->prescaler);
}

HAL_StatusTypeDef spi_tx(SPI_IF* spiIf, uint8_t const* data, uint16_t size)
{
  xSemaphoreTake(spiIf->ready, portMAX_DELAY);
  spi_modify_presc(spiIf);
  HAL_StatusTypeDef status = HAL_SPI_Transmit(spiIf->spi, (uint8_t*)data, size, spiIf->timeout * size);
  xSemaphoreGive(spiIf->ready);
  return status;
}

HAL_StatusTypeDef spi_rx(SPI_IF* spiIf, uint8_t* data, uint16_t size)
{
  xSemaphoreTake(spiIf->ready, portMAX_DELAY);
  spi_modify_presc(spiIf);
  HAL_StatusTypeDef status = HAL_SPI_Receive(spiIf->spi, data, size, spiIf->timeout * size);
  xSemaphoreGive(spiIf->ready);
  return status;
}

HAL_StatusTypeDef spi_tx_dma(SPI_IF* spiIf, uint8_t const* data, uint16_t size)
{
  xSemaphoreTake(spiIf->ready, portMAX_DELAY);
  spi_modify_presc(spiIf);
  HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(spiIf->spi, (uint8_t*)data, size);
  /* The callback HAL_SPI_TxCpltCallback gives the semaphore semSpi */
  return status;
}

HAL_StatusTypeDef spi_rx_dma(SPI_IF* spiIf, uint8_t* data, uint16_t size)
{
  xSemaphoreTake(spiIf->ready, portMAX_DELAY);
  spi_modify_presc(spiIf);
  HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(spiIf->spi, data, size);
  /* The callback HAL_SPI_RxCpltCallback gives the semaphore semSpi */
  return status;
}

HAL_SPI_StateTypeDef spi_get_state(SPI_IF* spiIf)
{
  return HAL_SPI_GetState(spiIf->spi);
}

void spi_wait_clear(SPI_IF* spiIf)
{
  xSemaphoreTake(spiIf->ready, portMAX_DELAY);
  xSemaphoreGive(spiIf->ready);
}
/* USER CODE END 1 */
