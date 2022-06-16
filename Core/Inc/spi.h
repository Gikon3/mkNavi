/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

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

extern SPI_HandleTypeDef hspi1;

extern SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN Private defines */
typedef struct SpiIf_
{
  SPI_HandleTypeDef   *spi;
  GPIO_TypeDef        *csGpioPort;
  uint16_t            csGpioPin;
  uint32_t            prescaler;
  uint32_t            timeout;
  osMessageQueueId_t  queue;
  osSemaphoreId_t     ready;
} SPI_IF;

typedef struct SpiQNode_
{
  SPI_IF          *spi;
  TranType        type;
  uint32_t        address;
  uint16_t        addressSize;
  uint8_t         *data;
  uint16_t        dataSize;
  osSemaphoreId_t ready;
  SdNode          *sdParam;
  uint8_t         cmd;
} SpiQNode;

extern SPI_IF spiIF_TFT;
extern SPI_IF spiIF_Flash;
extern SPI_IF spiIF_SD;
extern SPI_IF spiIF_L3GD20H;  // Гироскоп
extern SPI_IF spiIF_LIS3MDL;  // Магнитометр
extern SPI_IF spiIF_MT25Q;     // FLASH
/* USER CODE END Private defines */

void MX_SPI1_Init(void);
void MX_SPI3_Init(void);

/* USER CODE BEGIN Prototypes */
void spi_init();
void spi_decs(SPI_IF* spiIf);
void spi_cs(SPI_IF* spiIf);
extern void spi_modify_presc(SPI_IF* spiIf);
HAL_StatusTypeDef spi_tx(SPI_IF* spiIf, uint8_t const* data, uint16_t size);
HAL_StatusTypeDef spi_rx(SPI_IF* spiIf, uint8_t* data, uint16_t size);
HAL_StatusTypeDef spi_tx_dma(SPI_IF* spiIf, uint8_t const* data, uint16_t size);
HAL_StatusTypeDef spi_rx_dma(SPI_IF* spiIf, uint8_t* data, uint16_t size);
HAL_SPI_StateTypeDef spi_get_state(SPI_IF* spiIf);
void spi_wait_clear(SPI_IF* spiIf);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

