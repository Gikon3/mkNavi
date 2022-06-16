#ifndef FRAMEWORK_INC_FT_SCREEN_IMPL_H_
#define FRAMEWORK_INC_FT_SCREEN_IMPL_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "util_types.h"
#include "ft_touch_impl.h"
#include "ft_keyboard_impl.h"

#define FT_SCREEN_TAGS_WORDS  4
#define FT_SCREEN_TAGS_BITS   32
#define FT_SCREEN_TAGS_START  128

struct _FT_Core;
struct _FT_Heap;
struct _FT_Widget;

typedef struct _FT_Screen
{
  struct _FT_Core   *core;
  SemaphoreHandle_t mut;
  Bool              draw;                       //< Необходимость генерации виджетов
  uint32_t          tags[FT_SCREEN_TAGS_WORDS];
  uint16_t          bitmapHandle;
  FT_TouchScreen    touchScreen;
  FT_Keyboard       keyboard;
  struct _FT_Widget *widget;
} FT_Screen;

#endif /* FRAMEWORK_INC_FT_SCREEN_IMPL_H_ */
