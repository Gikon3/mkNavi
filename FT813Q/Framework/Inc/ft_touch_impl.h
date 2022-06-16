#ifndef FRAMEWORK_INC_FT_TOUCH_IMPL_H_
#define FRAMEWORK_INC_FT_TOUCH_IMPL_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "queue.h"
#include "util_types.h"
#include "util_vector.h"
#include "ft_config.h"

typedef enum {
  ftTouchNone,
  ftTouchPress,
  ftTouchLongPress
} FT_TouchState;

typedef enum {
  ftTrackNone,
  ftTrackCapture
} FT_TrackState;

typedef struct
{
  FT_TouchState state;
  uint32_t      time;
  uint16_t      tagPress;
  uint16_t      tag;
  int16_t       x;
  int16_t       y;
} FT_Touch;

typedef struct
{
  FT_TrackState state;
  uint16_t      tagCapture;
  uint16_t      tag;
  uint16_t      oldVal;
  uint16_t      val;
} FT_Track;

typedef enum {
  ftGestureUnknown = 0,
  ftGestureLeft = 1,
  ftSGestureRight = 1 << 1,
  ftGestureTop = 1 << 2,
  ftGestureTopRight = 1 << 3,
  ftGestureBottom = 1 << 4,
  ftGestureSwipe = 1 << 5,
  ftGesturePinch = 1 << 6,
  ftGestureLock = 1 << 7
} FT_GestureState;

typedef struct
{
  Bool            lock;   //< Данное касание используется треком
  FT_GestureState state;
  FT_GestureState ready;  //< Готовый к отправке жест
  int16_t         x;
  int16_t         y;
  int16_t         val;
} FT_GestureTouch;

typedef struct
{
  FT_GestureState gesture;
  int16_t         x;
  int16_t         y;
  int16_t         dx;
  int16_t         dy;
} FT_GestureSendContainer;

typedef struct
{
  uint8_t       gestures;
  QueueHandle_t qu;
} FT_GestureQueue;

typedef struct
{
  FT_GestureTouch touch[FT_NUMBER_OF_TOUCHES];
  Bool            sendFail;
  Vector          queueConnect;
  Bool            draw;
} FT_Gesture;

typedef struct
{
  uint8_t     number;                       //< Поддерживаемое количество касаний
  FT_Touch    touches[FT_NUMBER_OF_TOUCHES];
  FT_Track    tracks[FT_NUMBER_OF_TOUCHES];
  FT_Gesture  gesture;
} FT_TouchScreen;

#endif /* FRAMEWORK_INC_FT_TOUCH_IMPL_H_ */
