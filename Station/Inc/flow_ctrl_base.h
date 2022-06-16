#ifndef INC_FLOW_CTRL_BASE_H_
#define INC_FLOW_CTRL_BASE_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "queue.h"
#include "util_types.h"
#include "util_search.h"

typedef enum {
  flowPPM0,
  flowPPM1,
  flowGNSS0,
  flowGNSS1,
  flowMI,
  flowCOM,
  flowJet,
  flowBluetooth,
} FlowPort;

struct _Station;
typedef struct
{
  FlowPort        srcPort;
  struct _Station *sta;
} FlowCtrlProcessArgument;

typedef struct
{
  struct
  {
    struct _FlowUart
    {
      UART_HandleTypeDef* uart;       // Указатель на структуру huart
      char const*         buff;       // Буфер UART
      size_t              size;       // Его размер
      char const*         readPoint;  // Указатель на конец последнего обработанного сообщения
    } ppm[2];
    struct _FlowUart      gnss[2];
    struct
    {
      UART_HandleTypeDef      *uart;
      RingBufferItem          searchItem;
      FlowCtrlProcessArgument argument;
    } mi;
    struct
    {
      osMessageQueueId_t  rxQ;
      char const*         readPoint;  // Указатель на конец последнего обработанного сообщения
    } com;
    struct _FlowUart      jet;        // UART на Jetson Nano
    struct
    {
      char*               buff;
      size_t              size;
      char const*         readPoint;
      uint32_t            writeIndex;
    } bluetooth;
  } raw;
  struct
  {
    struct _FlowRipe
    {
      Bool                en;         // Разрешение на отправку
      osMessageQueueId_t  queue;      // Очередь в которой собираются сообщения
    } ppm[2];
    struct
    {
      Bool                en;
      osMessageQueueId_t  queue;
      String              accum;
      uint32_t            start;
      uint32_t            timeout;
    } gnss[2];;
    struct _FlowRipe      mi;
    struct _FlowRipe      com;
    struct _FlowRipe      jet;
    struct
    {
      struct _FlowRipe2
      {
        Bool              en;
        QueueHandle_t     queue;
      } ppm;
      struct _FlowRipe2   gnss;
    } screen;
    struct _FlowRipe      bluetooth;
  } ripe;
} FlowCtrl;

#endif /* INC_FLOW_CTRL_BASE_H_ */
