#ifndef FRAMEWORK_INC_FT_CORE_BASE_H_
#define FRAMEWORK_INC_FT_CORE_BASE_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "util_types.h"
#include "util_dlist.h"
#include "util_vector.h"
#include "ft_config.h"
#include "ft_utils_impl.h"
#include "ft_host_impl.h"
#include "ft_heap_impl.h"

struct _FT_Screen;

typedef struct _FT_Core
{
  FT_Host           host;
  FT_Heap           heap;
  uint32_t          delayMS;
  TaskHandle_t      tskMain;
  TaskHandle_t      tskSlotProcess;
  QueueHandle_t     qSlotItem;
  FT_Font           fontTable;
  FT_HeapParam      startFrame;
  struct _FT_Screen *screen;        //< Текущий отображаемый экран
} FT_Core;

typedef struct
{
  FT_Color  clearColor;
  FT_Color  color;
  FT_Color  fgColor;
  FT_Color  bgColor;
} FT_Theme;

#endif /* FRAMEWORK_INC_FT_BASE_H_ */
