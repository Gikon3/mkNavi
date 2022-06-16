/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "spi.h"
#include "usart.h"
#include "i2c.h"
#include "rtc.h"
#include "usbd_cdc_if.h"
#include "user_diskio_spi.h"
#include "fatfs.h"
#include "util_heap.h"
#include "util_string.h"
#include "led.h"
#include "bat_ctrl.h"
#include "usb_port_ctrl.h"
#include "sd_card.h"
#include "flow_ctrl.h"
#include "station.h"
#include "flash.h"
#include "ppm.h"
#include "jet.h"
#include "gnss.h"
#include "mi.h"
#include "rtc_if.h"
#include "a_lis331xx.h"
#include "g_l3gxx.h"
#include "m_lis3mdl.h"
#include "m_hmc5883l.h"
#include "b_bmp280.h"
#include "mt25q.h"
#include "ft_core.h"
#include "shutdown.h"
#include "app_ui_manager.h"
#include "bluetooth.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
ShutdownState shutDownFl = shdownNone;
static Bluetooth *bluetooth = NULL;
/* USER CODE END Variables */
/* Definitions for main */
osThreadId_t mainHandle;
const osThreadAttr_t main_attributes = {
  .name = "main",
  .stack_size = 224 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for qSpi */
osMessageQueueId_t qSpiHandle;
const osMessageQueueAttr_t qSpi_attributes = {
  .name = "qSpi"
};
/* Definitions for qI2c */
osMessageQueueId_t qI2cHandle;
const osMessageQueueAttr_t qI2c_attributes = {
  .name = "qI2c"
};
/* Definitions for qGNSS_Tx1 */
osMessageQueueId_t qGNSS_Tx1Handle;
const osMessageQueueAttr_t qGNSS_Tx1_attributes = {
  .name = "qGNSS_Tx1"
};
/* Definitions for qPPM_Tx0 */
osMessageQueueId_t qPPM_Tx0Handle;
const osMessageQueueAttr_t qPPM_Tx0_attributes = {
  .name = "qPPM_Tx0"
};
/* Definitions for qCDC_Rx */
osMessageQueueId_t qCDC_RxHandle;
const osMessageQueueAttr_t qCDC_Rx_attributes = {
  .name = "qCDC_Rx"
};
/* Definitions for qCDC_Tx */
osMessageQueueId_t qCDC_TxHandle;
const osMessageQueueAttr_t qCDC_Tx_attributes = {
  .name = "qCDC_Tx"
};
/* Definitions for qMI_Tx */
osMessageQueueId_t qMI_TxHandle;
const osMessageQueueAttr_t qMI_Tx_attributes = {
  .name = "qMI_Tx"
};
/* Definitions for qJetTx */
osMessageQueueId_t qJetTxHandle;
const osMessageQueueAttr_t qJetTx_attributes = {
  .name = "qJetTx"
};
/* Definitions for qBlthTx */
osMessageQueueId_t qBlthTxHandle;
const osMessageQueueAttr_t qBlthTx_attributes = {
  .name = "qBlthTx"
};
/* Definitions for tmrSyncTime */
osTimerId_t tmrSyncTimeHandle;
const osTimerAttr_t tmrSyncTime_attributes = {
  .name = "tmrSyncTime"
};
/* Definitions for semSpi */
osSemaphoreId_t semSpiHandle;
const osSemaphoreAttr_t semSpi_attributes = {
  .name = "semSpi"
};
/* Definitions for semInitPpm */
osSemaphoreId_t semInitPpmHandle;
const osSemaphoreAttr_t semInitPpm_attributes = {
  .name = "semInitPpm"
};
/* Definitions for semSyncTime */
osSemaphoreId_t semSyncTimeHandle;
const osSemaphoreAttr_t semSyncTime_attributes = {
  .name = "semSyncTime"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void system_config();
Bluetooth* blth_instaince();
/* USER CODE END FunctionPrototypes */

void start_main(void *argument);
void callbackTmrSyncTime(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
}

__weak unsigned long getRunTimeCounterValue(void)
{
  return xTaskGetTickCount();
}

Bluetooth* blth_instanse()
{
  return bluetooth;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of semSpi */
  semSpiHandle = osSemaphoreNew(1, 1, &semSpi_attributes);

  /* creation of semInitPpm */
  semInitPpmHandle = osSemaphoreNew(1, 1, &semInitPpm_attributes);

  /* creation of semSyncTime */
  semSyncTimeHandle = osSemaphoreNew(1, 1, &semSyncTime_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of tmrSyncTime */
  tmrSyncTimeHandle = osTimerNew(callbackTmrSyncTime, osTimerPeriodic, NULL, &tmrSyncTime_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qSpi */
  qSpiHandle = osMessageQueueNew (16, sizeof(SpiQNode), &qSpi_attributes);

  /* creation of qI2c */
  qI2cHandle = osMessageQueueNew (16, sizeof(I2cQNode), &qI2c_attributes);

  /* creation of qGNSS_Tx1 */
  qGNSS_Tx1Handle = osMessageQueueNew (16, sizeof(String), &qGNSS_Tx1_attributes);

  /* creation of qPPM_Tx0 */
  qPPM_Tx0Handle = osMessageQueueNew (16, sizeof(String), &qPPM_Tx0_attributes);

  /* creation of qCDC_Rx */
  qCDC_RxHandle = osMessageQueueNew (16, sizeof(CDCRxData), &qCDC_Rx_attributes);

  /* creation of qCDC_Tx */
  qCDC_TxHandle = osMessageQueueNew (16, sizeof(String), &qCDC_Tx_attributes);

  /* creation of qMI_Tx */
  qMI_TxHandle = osMessageQueueNew (16, sizeof(String), &qMI_Tx_attributes);

  /* creation of qJetTx */
  qJetTxHandle = osMessageQueueNew (16, sizeof(String), &qJetTx_attributes);

  /* creation of qBlthTx */
  qBlthTxHandle = osMessageQueueNew (16, sizeof(String), &qBlthTx_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of main */
  mainHandle = osThreadNew(start_main, NULL, &main_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_start_main */
/**
  * @brief  Function implementing the main thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_main */
void start_main(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN start_main */
  system_config();

  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();



    osDelayUntil(start+pdMS_TO_TICKS(250));
  }
  /* USER CODE END start_main */
}

/* callbackTmrSyncTime function */
void callbackTmrSyncTime(void *argument)
{
  /* USER CODE BEGIN callbackTmrSyncTime */
  gnss_sync_time(&station);
  /* USER CODE END callbackTmrSyncTime */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void start_service_interrupt(void *argument)
{
  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();

    uint32_t intPending = EXTI->PR;
    if(intPending & USB_INT_Pin) {
      EXTI->PR = USB_INT_Pin;
      usbp_ctrl_clear_int();
    }
    if(intPending & TFT_INT_Pin) {
      EXTI->PR = TFT_INT_Pin;
//      uint8_t flags = ft_app_get_intfl(&dispHost);
    }
    if(intPending & BTN_INT_Pin) {
      EXTI->PR = BTN_INT_Pin;
      shutDownFl = shdownPend;
    }

    if(shutDownFl == shdownTrue)
      shutdown();

    osDelayUntil(start+pdMS_TO_TICKS(50));
  }
}

static void start_service_spi(void *argument)
{
  while(bTrue) {
    SpiQNode node;
    xQueueReceive(qSpiHandle, &node, portMAX_DELAY);

//    spi_cs(node.spi);
    if(node.spi == &spiIF_TFT) {
      spi_cs(node.spi);
      if(node.type == typeTxData) {
        spi_tx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
        vPortFree(node.data);
      }
      else if(node.type == typeRxData) {
        spi_rx(node.spi, node.data, node.dataSize);
//        spi_rx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
      }
      else if(node.type == typeTx) {
        uint8_t prefix[3];
        prefix[0] = 0x80 | (node.address >> 16 & 0x3F);
        prefix[1] = node.address >> 8;
        prefix[2] = node.address;
        spi_tx_dma(node.spi, prefix, sizeof(prefix));
        spi_wait_clear(node.spi);
        spi_tx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
        vPortFree(node.data);
      }
      else {
        uint8_t prefix[4];
        prefix[0] = node.address >> 16 & 0x3F;
        prefix[1] = node.address >> 8;
        prefix[2] = node.address;
        prefix[3] = 0x00;
        spi_tx_dma(node.spi, prefix, sizeof(prefix));
        spi_wait_clear(node.spi);
        spi_rx(node.spi, node.data, node.dataSize);
//        spi_rx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
      }
    }
    else if(node.spi == &spiIF_L3GD20H || node.spi == &spiIF_LIS3MDL) {
      spi_cs(node.spi);
      if(node.type == typeTx) {
        uint8_t prefix = 0x40 | (node.address & 0x3F);
        spi_tx_dma(node.spi, &prefix, sizeof(prefix));
        spi_wait_clear(node.spi);
        spi_tx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
        vPortFree(node.data);
      }
      else {
        uint8_t prefix = 0xC0 | (node.address & 0x3F);
        spi_tx_dma(node.spi, &prefix, sizeof(prefix));
        spi_wait_clear(node.spi);
        spi_rx(node.spi, node.data, node.dataSize);
//        spi_rx_dma(node.spi, node.data, node.dataSize);
        spi_wait_clear(node.spi);
      }
    }
    else if(node.spi == &spiIF_SD) {
      switch(node.sdParam->type) {
        case sdInit:
          node.sdParam->result = USER_SPI_initialize(node.sdParam->pdrv);
          break;
        case sdRead:
          node.sdParam->result = USER_SPI_read(node.sdParam->pdrv, node.sdParam->buff,
                                               node.sdParam->sector, node.sdParam->count);
          break;
        case sdWrite:
          node.sdParam->result = USER_SPI_write(node.sdParam->pdrv, node.sdParam->buff,
                                                node.sdParam->sector, node.sdParam->count);
          break;
        case sdIoctl:
          node.sdParam->result = USER_SPI_ioctl(node.sdParam->pdrv, node.sdParam->cmd,
                                                node.sdParam->buffCmd);
          break;
      }
    }
    else if(node.spi == &spiIF_MT25Q) {
      switch(node.type) {
        case typeTx:
          mt25q_spi_send(&spiIF_MT25Q, node.cmd, node.address, node.addressSize, node.data, node.dataSize);
          break;
        case typeRx:
          mt25q_spi_receive(&spiIF_MT25Q, node.cmd, node.address, node.addressSize, node.data, node.dataSize);
          break;
        default:
          ;
      }
    }

    spi_decs(node.spi);
    if(node.ready)
      xSemaphoreGive(node.ready);
  }
}

static void start_service_i2c(void *argument)
{
  while(bTrue) {
    I2cQNode node;
    xQueueReceive(qI2cHandle, &node, portMAX_DELAY);

    if(node.type == typeTx) {
      uint16_t sizeAll = sizeof(node.address) + node.dataSize;
      uint8_t* txData = pvPortMalloc(sizeAll);
      memcpy(txData, &node.address, sizeof(node.address));
      memcpy(&txData[sizeof(node.address)], node.data, node.dataSize);
      vPortFree(node.data);
      i2c_tx(node.i2c, txData, sizeAll);
      vPortFree(txData);
    }
    else if(node.type == typeRx){
      i2c_tx(node.i2c, &node.address, sizeof(node.address));
      i2c_rx(node.i2c, node.data, node.dataSize);
    }
    else /* typeIsReady */ {
      if(node.dataSize > 0) {
        node.data[0] = i2c_is_ready(node.i2c);
      }
    }

    if(node.ready)
      xSemaphoreGive(node.ready);
  }
}

static void start_service_sensors(void *argument)
{
  g_l3gd20h_init();
  g_l3g4250d_init();
  g_l3gxx_config(&l3gd20h);
  g_l3gxx_config(&l3g4250d);
  a_lis331hh_init();
  a_lis331dlh_init();
  a_lis331xx_config(&lis331hh);
  a_lis331xx_config(&lis331dlh);
  m_lis3mdl_init();
  m_hmc5883l_init();
  m_lis3mdl_config(&lis3mdl);
  m_hmc5883l_config(&hmc5883l);
  b_bmp230_init();
  b_bmp230_config(&bmp230);

  bat_config();
  bat_out_en();
  uint8_t batteryCount = 0;

  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();

    a_lis331xx_refresh_xyz(&lis331hh);
    a_lis331xx_refresh_xyz(&lis331dlh);
    m_lis3mdl_refresh_xyz(&lis3mdl);
    m_lis3mdl_refresh_temp(&lis3mdl);
    m_hmc5883l_refresh(&hmc5883l);
    b_bmp230_refresh(&bmp230);
    g_l3gxx_refresh_xyz(&l3gd20h);
    g_l3gxx_refresh_xyz(&l3g4250d);
    g_l3gxx_refresh_temp(&l3gd20h);
    g_l3gxx_refresh_temp(&l3g4250d);

    ++batteryCount;
    if(batteryCount == 10) {
      batteryCount = 0;
      bat_refresh_chrg_stat();
      bat_refresh_ibus();
      bat_refresh_ichrg();
      bat_refresh_vbus();
      bat_refresh_vbat();
      bat_refresh_vsys();
      bat_refresh_ts();
      bat_refresh_tdie();
    }

    osDelayUntil(start+pdMS_TO_TICKS(10));
  }
}

static void start_service_sd(void *argument)
{
  SD_Card *sd = sd_instance();
  sd_config(sd, &spiIF_SD);

  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();
    sd_write_loop(sd);
    osDelayUntil(start+pdMS_TO_TICKS(10));
  }
}

static void start_gnss_rx1(void *argument)
{
  while(bTrue) {
    flow_gnss1_process(&flowCtrl, &station);
    osDelay(pdMS_TO_TICKS(10));
  }
}
static void start_gnss_tx1(void *argument)
{
  while(bTrue) {
    String str;
    xQueueReceive(qGNSS_Tx1Handle, &str, portMAX_DELAY);
    gnss_uart1_tx(&flowCtrl, &str);
    str_clear(&str);
  }
}

static void start_ppm_rx0(void *argument)
{
  while(bTrue) {
    flow_ppm0_process(&flowCtrl, &station);
    osDelay(pdMS_TO_TICKS(10));
  }
}

static void start_ppm_tx0(void *argument)
{
  while(bTrue) {
    String str;
    xQueueReceive(qPPM_Tx0Handle, &str, portMAX_DELAY);
    ppm_uart0_tx(&flowCtrl, &str);
    str_clear(&str);
  }
}

static void start_ppm_init(void *argument)
{
  osDelay(pdMS_TO_TICKS(20000)); //< Ожидание инициализации платы ППМ
  ppm_config(&station, &flowCtrl);
  mi_config(&station, &flowCtrl);
  vTaskDelete(NULL);
}

static void start_cdc_rx(void *argument)
{
  while(bTrue) {
    flow_com_process(&flowCtrl, &station);
    osDelay(pdMS_TO_TICKS(20));
  }
}

static void start_cdc_tx(void *argument)
{
  while(bTrue) {
    String str;
    String storage = make_str_null();
    while(xQueueReceive(qCDC_TxHandle, &str, 0) == pdTRUE)
      str_append_str_own(&storage, &str);
    if(storage.len)
      CDC_Transmit_HS((uint8_t*)str_data(&storage), str_length(&storage));
    str_clear(&storage);
    osDelay(pdMS_TO_TICKS(5));
  }
}

static void start_mi_rx(void *argument)
{
  HAL_UART_Receive_DMA(flowCtrl.raw.mi.uart, (uint8_t*)flowCtrl.raw.mi.searchItem.beginBuffer, flowCtrl.raw.mi.searchItem.sizeBuffer);
  while(bTrue) {
    flow_mi_process(&flowCtrl, &station);
    osDelay(pdMS_TO_TICKS(20));
  }
}

static void start_mi_tx(void *argument)
{
  while(bTrue) {
    String str;
    xQueueReceive(qMI_TxHandle, &str, portMAX_DELAY);
    HAL_UART_Transmit(flowCtrl.raw.mi.uart, (uint8_t*)str_data(&str), str_length(&str), 1000);
    str_clear(&str);
  }
}

static void start_jet_rx(void *argument)
{
  jet_config(&station, &flowCtrl);
  while(bTrue) {
    flow_jet_process(&flowCtrl, &station);
    osDelay(pdMS_TO_TICKS(20));
  }
}

static void start_jet_tx(void *argument)
{
  while(bTrue) {
    String str;
    xQueueReceive(qJetTxHandle, &str, portMAX_DELAY);
    HAL_UART_Transmit(flowCtrl.raw.jet.uart, (uint8_t*)str_data(&str), str_length(&str), 1000);
    str_clear(&str);
  }
}

static void start_bluetooth_tx(void *argument)
{
  while(bTrue) {
    String str;
    xQueueReceive(qBlthTxHandle, &str, portMAX_DELAY);
    blth_send(bluetooth, (uint8_t*)str.data, str.len);
    str_clear(&str);
  }
}

static void start_sta_regular(void *argument)
{
  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();

    sta_send_telemetry(&station);
    sta_send_sys_info(&station);
    sta_send_sensor(&station);
    sta_calculate(&station);
    sta_send_jet(&flowCtrl, &station);

    flow_gnss0_tx_data(&flowCtrl, &station);
    flow_gnss1_tx_data(&flowCtrl, &station);

    osDelayUntil(start+pdMS_TO_TICKS(station.send.period));
  }
}

static void start_sync_time(void *argument)
{
  xSemaphoreTake(semSyncTimeHandle, portMAX_DELAY);
  while(bTrue) {
    xSemaphoreTake(semSyncTimeHandle, portMAX_DELAY);
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

    GNSS_SyncDateTime const* dt = gnss_datetime(&station);
    rtcIF.date.day = dt->date.day;
    rtcIF.date.month = dt->date.month;
    rtcIF.date.year = dt->date.year;
    rtcIF.time.sec = dt->time.second;
    rtcIF.time.min = dt->time.minute;
    rtcIF.time.hour = dt->time.hour;
    rtc_sync_datetime(&rtcIF);

    // Периодичный таймер для синхронизации времени с GNSS. Если не запущен - запустить
    if(xTimerIsTimerActive(tmrSyncTimeHandle) == pdFALSE) {
      static const TickType_t syncPeriod = (1 * 60 * 60 * 1000) / portTICK_PERIOD_MS;   // 1 час
      xTimerChangePeriod(tmrSyncTimeHandle, syncPeriod, portMAX_DELAY);
      xTimerStart(tmrSyncTimeHandle, portMAX_DELAY);
    }
  }
}

static FT_Core eve2;
static AppUI ui;
static void start_display(void* argument)
{
  {
    const FT_Display param = {
        .h.size = 800,
        .h.cycle = 928,
        .h.offset = 88,
        .h.sync0 = 0,
        .h.sync1 = 48,
        .v.size = 480,
        .v.cycle = 525,
        .v.offset = 32,
        .v.sync0 = 0,
        .v.sync1 = 3,
        .pclk = 2,
        .swizzle = 0,
        .pclkpol = 0,
        .cspread = 1,
        .dither = 1
    };
    ft_core_init(&eve2, &spiIF_TFT, &param, TFT_PD_GPIO_Port, TFT_PD_Pin, 20);

    const FT_Theme theme = {
        .clearColor.value = 0xFFFFFFFF,
        .color.value = 0xFF,
        .fgColor.value = 0xFF8000FF,
        .bgColor.value = 0xFF
    };
    ft_core_config(&eve2, 256, 16, &theme);
  }

  app_ui_manager_init(&ui, &eve2);
  while(bTrue) {
    const uint32_t start = xTaskGetTickCount();
    app_ui_manager(&ui);
    osDelayUntil(start+pdMS_TO_TICKS(20));
  }
}

static void system_config()
{
  xTaskCreate(start_service_interrupt, "serviceInt", 128, NULL, osPriorityAboveNormal, NULL);

  spi_init();
  xTaskCreate(start_service_spi, "serviceSPI", 128, NULL, osPriorityAboveNormal, NULL);

  i2c_init();
  xTaskCreate(start_service_i2c, "serviceI2C", 128, NULL, osPriorityAboveNormal, NULL);

  usbp_ctrl_config();

  mt25q_init(mt25q_instance());
  mt25q_config(mt25q_instance());

  xTaskCreate(start_service_sensors, "serviceSensor", 150, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_service_sd, "serviceSD", 448, NULL, osPriorityNormal, NULL);

  flow_init();
  sta_init();
  flash_read_params(&station);
  xTaskCreate(start_gnss_rx1, "gnssRx1", 500, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_gnss_tx1, "gnssTx1", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_ppm_rx0, "ppmRx1", 500, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_ppm_tx0, "ppmTx1", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_ppm_init, "ppmInit", 380, NULL, osPriorityAboveNormal, NULL);
  xTaskCreate(start_cdc_rx, "cdcRx", 400, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_cdc_tx, "cdcTx", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_mi_rx, "miRx", 400, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_mi_tx, "miTx", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_jet_rx, "jetRx", 250, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_jet_tx, "jetTx", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_bluetooth_tx, "blthTx", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(start_sta_regular, "staRegular", 544, NULL, osPriorityNormal, NULL);

  gnss_config(&station, &flowCtrl);
  xTaskCreate(start_sync_time, "timeSync", 128, NULL, osPriorityHigh, NULL);

  xTaskCreate(start_display, "display", 320, NULL, osPriorityNormal, NULL);

  bluetooth = blth_create(&huart1, BLTH_RESET_GPIO_Port, BLTH_RESET_Pin);
}
/* USER CODE END Application */

