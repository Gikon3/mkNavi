#include <string.h>
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_touch.h"
#include "ft_screen.h"
#include "ft_keyboard.h"
#include "ft_widget_impl.h"
#include "ft_widget_line_edit.h"

#define TOUCH_RELEASE_VAL -32768  /*< Значение, означающее отсутствие касания  */
#define LONG_TOUCH_TIME   1000    /*< Время, после которого касание считается долгим */

#define GEST_LEFT_X1      20
#define GEST_TOPR_X0      (param->h.size - 180)
#define GEST_TOPR_Y1      20
#define GEST_TOP_X0       GEST_LEFT_X1
#define GEST_TOP_X1       GEST_TOPR_X0
#define GEST_TOP_Y1       GEST_TOPR_Y1
#define GEST_RIGHT_X0     (param->h.size - 20)
#define GEST_RIGHT_Y0     GEST_TOP_Y1
#define GEST_BOTTOM_X0    GEST_LEFT_X1
#define GEST_BOTTOM_X1    GEST_RIGHT_X0
#define GEST_BOTTOM_Y0    (param->v.size - 20)
#define GEST_THR_RDY      80

#define GEST_RADIUS(l)    ((4 * GEST_THR_RDY * GEST_THR_RDY + (l) * (l)) / (8 * GEST_THR_RDY) - GEST_THR_RDY)
/* Радиус окружности, чтобы высота кругового сегмента (GEST_THR_RDY) находилась в видимой области
 *     4 * h^2 + l^2
 * R = -------------
 *         8 * h
 * где h - высота сегмента, l - длина хорды
 */

typedef enum {
  touchPress,
  touchLongPress,
  touchRelease,
  touchReset
} TouchSlotType;

typedef struct
{
  uint8_t         number;
  struct
  {
    TouchSlotType type;
    uint16_t      tag;
    int16_t       x;
    int16_t       y;
  } touch[FT_NUMBER_OF_TOUCHES];
} CalcTouches;

typedef enum {
  trackCapture,
  trackRelease
} TrackSlotType;

typedef struct
{
  uint8_t         number;
  struct
  {
    uint8_t       index;    //< Порядковый номер необработанного касания
    TrackSlotType type;
    uint16_t      tag;
    uint16_t      val;
  } track[FT_NUMBER_OF_TOUCHES];
} CalcTracks;

void ft_touch_touchscreen_init(FT_TouchScreen *touchScreen)
{
  touchScreen->number = FT_NUMBER_OF_TOUCHES;
  memset(touchScreen->touches, 0, sizeof(touchScreen->touches));
  memset(touchScreen->tracks, 0, sizeof(touchScreen->tracks));

  FT_Gesture *gesture = &touchScreen->gesture;
  memset(gesture->touch, 0, sizeof(gesture->touch));
  gesture->sendFail = bFalse;
  gesture->queueConnect = make_vect_null(sizeof(FT_GestureQueue));
  gesture->draw = bFalse;
}

void ft_touch_touchscreen_destroy(FT_TouchScreen *touchScreen)
{
  vect_clear(&touchScreen->gesture.queueConnect);
}

void ft_touch_touchscreen_load(FT_Core *core)
{
  FT_TouchScreen *touchScreen = &core->screen->touchScreen;
  ft_hal_touches(&core->host, touchScreen->number, touchScreen->touches);
  ft_hal_tracks(&core->host, touchScreen->number, touchScreen->tracks);
}

static inline void calc_touch_fill(FT_Touch const *touch, CalcTouches *calcTouches)
{
  calcTouches->touch[calcTouches->number].x = touch->x;
  calcTouches->touch[calcTouches->number].y = touch->y;
  ++calcTouches->number;
}

static void touch_press(FT_Touch const *touch, CalcTouches *calcTouches)
{
  calcTouches->touch[calcTouches->number].type = touchPress;
  calcTouches->touch[calcTouches->number].tag = touch->tag;
  calc_touch_fill(touch, calcTouches);
}

static void touch_long_press(FT_Touch const *touch, CalcTouches *calcTouches)
{
  calcTouches->touch[calcTouches->number].type = touchLongPress;
  calcTouches->touch[calcTouches->number].tag = touch->tag;
  calc_touch_fill(touch, calcTouches);
}

static void touch_reset(FT_Touch const *touch, CalcTouches *calcTouches)
{
  calcTouches->touch[calcTouches->number].type = touchReset;
  calcTouches->touch[calcTouches->number].tag = touch->tagPress;
  calc_touch_fill(touch, calcTouches);
}

static void touch_release(FT_Touch const *touch, CalcTouches *calcTouches)
{
  calcTouches->touch[calcTouches->number].type = touchRelease;
  calcTouches->touch[calcTouches->number].tag = touch->tagPress;
  calc_touch_fill(touch, calcTouches);
}

static Bool touch_is_released(FT_Touch const *touch)
{
  return touch->x == TOUCH_RELEASE_VAL && touch->y == TOUCH_RELEASE_VAL ? bTrue : bFalse;
}

static void touch_process(FT_TouchScreen *touchScreen, CalcTouches *calcTouches)
{
  const uint32_t timeMs = xTaskGetTickCount() / portTICK_PERIOD_MS;
  for(uint8_t i = 0; i < touchScreen->number; ++i) {
    FT_Touch *touch = &touchScreen->touches[i];
    FT_GestureTouch const *gTouch = &touchScreen->gesture.touch[i];
    const Bool allowedGestures = gTouch->state == ftGestureUnknown || gTouch->state == ftGestureSwipe ||
        gTouch->state == ftGesturePinch || gTouch->state == ftGestureLock ? bTrue : bFalse;
    switch(touch->state) {
      case ftTouchNone:
        touch->time = timeMs;
        if(touch->tag > 0 && allowedGestures == bTrue) {
          touch_press(touch, calcTouches);
          touch->tagPress = touch->tag;
          touch->state = ftTouchPress;
        }
        break;
      case ftTouchPress:
        if(touch->tag == touch->tagPress && (xTaskGetTickCount()/portTICK_PERIOD_MS-touch->time) > LONG_TOUCH_TIME) {
          touch_long_press(touch, calcTouches);
          touch->state = ftTouchLongPress;
          break;
        }
      case ftTouchLongPress:
        if(touch->tag != touch->tagPress) {
          touch->time = timeMs;
          if(touch_is_released(touch) == bTrue)
            touch_release(touch, calcTouches);
          else
            touch_reset(touch, calcTouches);
          touch->tagPress = touch->tag;
          if(touch->tag > 0) {
            touch_press(touch, calcTouches);
            touch->state = ftTouchPress;
          }
          else
            touch->state = ftTouchNone;
        }
        break;
    }
  }
}

static inline void calc_track_fill(FT_Track const *track, uint8_t index, CalcTracks *calcTracks)
{
  calcTracks->track[calcTracks->number].index = index;
  calcTracks->track[calcTracks->number].val = track->val;
  ++calcTracks->number;
}

static void track_capture(FT_Track const *track, uint8_t index, CalcTracks *calcTracks)
{
  calcTracks->track[calcTracks->number].type = trackCapture;
  calcTracks->track[calcTracks->number].tag = track->tag;
  calc_track_fill(track, index, calcTracks);
}

static void track_release(FT_Track const *track, uint8_t index, CalcTracks *calcTracks)
{
  calcTracks->track[calcTracks->number].type = trackRelease;
  calcTracks->track[calcTracks->number].tag = track->tagCapture;
  calc_track_fill(track, index, calcTracks);
}

static void track_process(FT_TouchScreen *touchScreen, CalcTracks *calcTracks)
{
  for(uint8_t i = 0; i < touchScreen->number; ++i) {
    FT_Track *track = &touchScreen->tracks[i];
    switch(track->state) {
      case ftTrackNone:
        if(track->tag > 0 && touchScreen->gesture.touch[i].state == ftGestureUnknown) {
          track_capture(track, i, calcTracks);
          track->tagCapture = track->tag;
          track->oldVal = track->val;
          track->state = ftTrackCapture;
        }
        break;
      case ftTrackCapture:
        if(track->tag == track->tagCapture && track->val != track->oldVal) {
          track_capture(track, i, calcTracks);
          track->oldVal = track->val;
        }
        else if(track->tag != track->tagCapture) {
          track_release(track, i, calcTracks);
          if(track->tag > 0)
            track_capture(track, i, calcTracks);
          else
            track->state = ftTrackNone;
          track->tagCapture = track->tag;
          track->oldVal = track->val;
        }
        break;
    }
  }
}

static void define_slots(TouchSlotType type, FT_Widget *widget, FT_WidgetSlot *common, FT_WidgetUserSlot *user)
{
  switch(type) {
    case touchPress:
      *common = widget->touch.slot.common.press;
      *user = widget->touch.slot.user.press;
      break;
    case touchLongPress:
      *common = widget->touch.slot.common.long_press;
      *user = widget->touch.slot.user.long_press;
      break;
    case touchRelease:
      *common = widget->touch.slot.common.release;
      *user = widget->touch.slot.user.release;
      break;
    case touchReset:
      *common = widget->touch.slot.common.reset;
      *user = widget->touch.slot.user.reset;
      break;
    }
}

static void send_touch_slot(xQueueHandle qSlotItem, FT_Widget *widget, CalcTouches const *calcTouches)
{
  for(uint8_t i = 0; i < calcTouches->number; ++i) {
    if(calcTouches->touch[i].tag == widget->touch.tag) {
      FT_WidgetSlot selectedCommonSlot = NULL;
      FT_WidgetUserSlot selectedUserSlot = NULL;
      define_slots(calcTouches->touch[i].type, widget, &selectedCommonSlot, &selectedUserSlot);

      FT_SlotItem slotItem = {
          .type = ftWidgetSlotInternal,
          .slot = NULL,
          .widget = widget,
          .param.x = calcTouches->touch[i].x,
          .param.y = calcTouches->touch[i].y,
          .arg = NULL
      };
      if(selectedCommonSlot) {
        slotItem.slot = selectedCommonSlot;
        xQueueSendToBack(qSlotItem, &slotItem, portMAX_DELAY);
      }
      if(selectedUserSlot) {
        slotItem.type = ftWidgetSlotUser;
        slotItem.userSlot = selectedUserSlot;
        slotItem.arg = widget->touch.slot.user.arg;
        xQueueSendToBack(qSlotItem, &slotItem, portMAX_DELAY);
      }
    }
  }
}

static void send_track_slot(xQueueHandle qSlotItem, FT_Gesture *gesture, FT_Widget *widget, CalcTracks const *calcTracks)
{
  for(uint8_t i = 0; i < calcTracks->number; ++i) {
    if(calcTracks->track[i].tag == widget->touch.tag) {
      FT_WidgetSlot selectedSlot = NULL;
      FT_WidgetUserSlot selectedUserSlot = NULL;
      switch(calcTracks->track[i].type) {
        case trackCapture:
          selectedSlot = widget->touch.slot.common.press;
          selectedUserSlot = widget->touch.slot.user.press;
          break;
        case trackRelease:
          selectedSlot = widget->touch.slot.common.release;
          selectedUserSlot = widget->touch.slot.user.release;
          break;
        }

      FT_SlotItem slotItem = {
          .type = ftWidgetSlotInternal,
          .slot = NULL,
          .widget = widget,
          .param.val = calcTracks->track[i].val,
          .arg = NULL
      };
      if(selectedSlot) {
        slotItem.slot = selectedSlot;
        xQueueSendToBack(qSlotItem, &slotItem, portMAX_DELAY);
      }
      if(selectedUserSlot) {
        slotItem.type = ftWidgetSlotUser;
        slotItem.userSlot = selectedUserSlot;
        slotItem.arg = widget->touch.slot.user.arg;
        xQueueSendToBack(qSlotItem, &slotItem, portMAX_DELAY);
      }

      // Заблокировать касание при обработке жестов
      FT_GestureTouch *gTouch = &gesture->touch[calcTracks->track[i].index];
      if(gTouch->state == ftGestureUnknown)
        gTouch->lock = bTrue;
    }
  }
}

static void slot_selection(xQueueHandle qSlotItem, FT_Gesture *gesture, FT_Widget *widget,
    CalcTouches const *calcTouches, CalcTracks const *calcTracks)
{
  switch(widget->touch.type) {
    case ftWidgetTouchNone:
      break;
    case ftWidgetTouchUsual:
      send_touch_slot(qSlotItem, widget, calcTouches);
      break;
    case ftWidgetTouchTrack:
      send_track_slot(qSlotItem, gesture, widget, calcTracks);
      break;
  }
}

static void find_slots_in_widgets(xQueueHandle qSlotItem, FT_Gesture *gesture, FT_Widget *widget,
    CalcTouches const *calcTouches, CalcTracks const *calcTracks)
{
  slot_selection(qSlotItem, gesture, widget, calcTouches, calcTracks);

  DListContainer *container = dlist_front(&widget->widgets);
  while(container) {
    FT_Widget *nested = *(FT_Widget**)dlist_item(container);
    find_slots_in_widgets(qSlotItem, gesture, nested, calcTouches, calcTracks);
    container = dlist_next(container);
  }
}

static void keyboard_process(FT_Keyboard *keyboard, CalcTouches const *calcTouches)
{
  for(uint8_t i = 0; i < calcTouches->number; ++i) {
    const uint16_t tag = calcTouches->touch[i].tag;
    if(tag <= 0x7F) {
      switch(calcTouches->touch[i].type) {
        case touchPress: ft_keyboard_press(keyboard, tag); break;
        case touchLongPress: ft_keyboard_long_press(keyboard, tag); break;
        case touchRelease: ft_keyboard_release(keyboard, tag); break;
        case touchReset: ft_keyboard_reset(keyboard, tag); break;
        }
    }
  }
}

static FT_GestureState define_gesture_state(FT_Display const *param, FT_Touch const *touch)
{
  const int16_t x = touch->x;
  const int16_t y = touch->y;
  if(touch_is_released(touch) == bTrue)
    return ftGestureUnknown;
  if(x <= GEST_LEFT_X1)
    return ftGestureLeft;
  if(x >= GEST_RIGHT_X0 && y > GEST_RIGHT_Y0)
    return ftSGestureRight;
  if(x > GEST_TOP_X0 && x < GEST_TOP_X1 && y <= GEST_TOP_Y1)
    return ftGestureTop;
  if(x >= GEST_TOPR_X0 && y <= GEST_TOPR_Y1)
    return ftGestureTopRight;
  if(x > GEST_BOTTOM_X0 && x < GEST_BOTTOM_X1 && y >= GEST_BOTTOM_Y0)
    return ftGestureBottom;
  return ftGestureSwipe;
}

static void gesture_fsm(FT_TouchScreen *touchScreen, FT_Display const *param)
{
  FT_Gesture *gesture = &touchScreen->gesture;
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    FT_GestureTouch *gTouch = &gesture->touch[i];
    FT_Touch const *touch = &touchScreen->touches[i];
    int16_t gestureVal = 0;
    switch(gTouch->state) {
      case ftGestureUnknown:
        break;
      case ftGestureLeft:
        gestureVal = touch->x;
        break;
      case ftSGestureRight:
        gestureVal = param->h.size - touch->x;
        break;
      case ftGestureTop:
      case ftGestureTopRight:
        gestureVal = touch->y;
        break;
      case ftGestureBottom:
        gestureVal = param->v.size - touch->y;
        break;
      case ftGestureSwipe:
      case ftGesturePinch:
        if(gTouch->lock == bTrue) {
          gTouch->val = 0;
          gTouch->state = ftGestureLock;
        }
        break;
      case ftGestureLock:
        break;
    }
    if(gTouch->state != ftGestureUnknown && touch_is_released(touch) == bTrue) {
      if(gTouch->val == GEST_THR_RDY)
        gTouch->ready = gTouch->state;
      gestureVal = 0;
      gTouch->x = TOUCH_RELEASE_VAL;
      gTouch->y = TOUCH_RELEASE_VAL;
      gTouch->val = 0;
      gTouch->state = ftGestureUnknown;
      gesture->draw = bTrue;
    }
    gestureVal = gestureVal > GEST_THR_RDY ? GEST_THR_RDY : gestureVal;
    if(gestureVal != gTouch->val) {
      gTouch->val = gestureVal;
      gesture->draw = bTrue;
    }
  }
  if(gesture->touch[0].state == ftGestureSwipe && gesture->touch[1].state == ftGestureSwipe) {
    gesture->touch[0].state = ftGesturePinch;
    gesture->touch[1].state = ftGesturePinch;
  }
  else if(gesture->touch[0].state == ftGesturePinch && gesture->touch[1].state != ftGesturePinch)
    gesture->touch[0].state = ftGestureSwipe;
  else if(gesture->touch[0].state != ftGesturePinch && gesture->touch[1].state == ftGesturePinch)
    gesture->touch[1].state = ftGestureSwipe;
}

static void send_in_queue(FT_Gesture *gesture, FT_GestureSendContainer *container)
{
  for(size_t i = 0; i < vect_size(&gesture->queueConnect); ++i) {
    FT_GestureQueue *quConnect = vect_at(&gesture->queueConnect, i, NULL);
    if((container->gesture | quConnect->gestures) && xQueueSend(quConnect->qu, container, 0) == pdFALSE)
      gesture->sendFail = bTrue;
  }
}

static void send_gesture(FT_Gesture *gesture, FT_GestureState state)
{
  FT_GestureSendContainer container = {
    container.gesture = state,
    container.x = TOUCH_RELEASE_VAL,
    container.y = TOUCH_RELEASE_VAL,
    container.dx = 0,
    container.dy = 0
  };
  send_in_queue(gesture, &container);
}

static void send_gesture_swipe(FT_Gesture *gesture, FT_GestureState state, FT_Touch const *touch)
{
  FT_GestureSendContainer container = {
    container.gesture = state,
    container.x = touch->x,
    container.y = touch->y,
    container.dx = 0,
    container.dy = 0
  };
  send_in_queue(gesture, &container);
}

static void send_gesture_pinch(FT_Gesture *gesture, FT_GestureState state, FT_Touch const *touches)
{
  FT_GestureSendContainer container = {
    container.gesture = state,
    container.x = touches[0].x,
    container.y = touches[0].y,
    container.dx = touches[1].x - touches[0].x,
    container.dy = touches[1].y - touches[0].y
  };
  send_in_queue(gesture, &container);
}

static void gesture_process(FT_Core *core)
{
  FT_Display const *param = &core->host.param;
  FT_TouchScreen *touchScreen = &core->screen->touchScreen;
  FT_Touch const *touches = touchScreen->touches;
  FT_Gesture *gesture = &touchScreen->gesture;
  for(uint8_t i = 0; i < touchScreen->number; ++i) {
    FT_Touch const *touch = &touches[i];
    if(gesture->touch[i].state == ftGestureUnknown)
      gesture->touch[i].state = define_gesture_state(param, touch);
  }

  gesture_fsm(&core->screen->touchScreen, param);

  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    FT_GestureTouch *gTouch = &gesture->touch[i];
    FT_Touch const *touch = &touchScreen->touches[i];
    if(gTouch->state == ftGestureUnknown && gTouch->ready != ftGestureUnknown) {
      send_gesture(gesture, gTouch->ready);
      gTouch->ready = ftGestureUnknown;
    }
    else if(gTouch->state == ftGestureSwipe && touch->x != gTouch->x && touch->y != gTouch->y) {
      send_gesture_swipe(gesture, ftGestureSwipe, touch);
      gTouch->x = touch->x;
      gTouch->y = touch->y;
    }
  }
  if(gesture->touch[0].state == ftGesturePinch && gesture->touch[1].state == ftGesturePinch &&
      gesture->touch[0].x != touches[0].x && gesture->touch[0].y != touches[0].y &&
      gesture->touch[1].x != touches[1].x && gesture->touch[1].y != touches[1].y) {
    send_gesture_pinch(gesture, ftGesturePinch, touches);
    gesture->touch[0].x = touches[0].x;
    gesture->touch[0].y = touches[0].y;
    gesture->touch[1].x = touches[1].x;
    gesture->touch[1].y = touches[1].y;
  }
}

static void keyboard_hide(FT_Keyboard *keyboard, FT_TouchScreen const *touchScreen)
{
  if(keyboard->visible == bTrue) {
    Bool hideKeyboard = bFalse;
    for(uint8_t i = 0; i < touchScreen->number; ++i) {
      const Bool isReleased = touch_is_released(&touchScreen->touches[i]);
      const Bool isCaptured = touchScreen->touches[i].tag == keyboard->capture->parent.touch.tag ? bTrue : bFalse;
      const Bool isMoreStartTag = touchScreen->touches[i].tagPress >= FT_SCREEN_TAGS_START ? bTrue : bFalse;
      if(isReleased == bTrue && (isCaptured == bFalse && isMoreStartTag == bTrue))
        hideKeyboard = bTrue;
    }
    if(hideKeyboard == bTrue)
      ft_keyboard_detach(keyboard);
  }
}

void ft_touch_touchscreen_process(FT_Core *core)
{
  FT_TouchScreen *touchScreen = &core->screen->touchScreen;
  for(uint8_t i = 0; i < touchScreen->number; ++i)
    touchScreen->gesture.touch[i].lock = bFalse;

  keyboard_hide(&core->screen->keyboard, touchScreen);

  CalcTouches calcTouches = {
      .number = 0
  };
  touch_process(touchScreen, &calcTouches);
  CalcTracks calcTracks = {
      .number = 0
  };
  track_process(touchScreen, &calcTracks);

  if(calcTouches.number > 0 || calcTracks.number > 0)
    find_slots_in_widgets(core->qSlotItem, &touchScreen->gesture, core->screen->widget, &calcTouches, &calcTracks);
  if(calcTouches.number > 0)
    keyboard_process(&core->screen->keyboard, &calcTouches);
  gesture_process(core);
}

Bool ft_touch_gesture_queue_connect(FT_Core *core, QueueHandle_t qu, uint8_t gestures)
{
  Vector *quConnect = &core->screen->touchScreen.gesture.queueConnect;
  // Проверка на повтор
  for(size_t i = 0; i < vect_size(quConnect); ++i) {
    FT_GestureQueue const *item = vect_at(quConnect, i, NULL);
    if(item->qu == qu)
      return bFalse;
  }
  FT_GestureQueue item = {
      .gestures = gestures,
      .qu = qu
  };
  vect_push_back(quConnect, &item);
  return bTrue;
}

Bool ft_touch_gesture_queue_disconnect(FT_Core *core, QueueHandle_t qu)
{
  Vector *quConnect = &core->screen->touchScreen.gesture.queueConnect;
  for(size_t i = 0; i < vect_size(quConnect); ++i) {
    FT_GestureQueue const *item = vect_at(quConnect, i, NULL);
    if(item->qu == qu) {
      vect_erase(quConnect, i, NULL);
      return bTrue;
    }
  }
  return bFalse;
}

void ft_touch_gesture_show(FT_Core *core, Vector *cmds)
{
  FT_Display const *param = &core->host.param;
  FT_Gesture *gesture = &core->screen->touchScreen.gesture;

  FT_Coord coord[FT_NUMBER_OF_TOUCHES];
  int32_t radius[FT_NUMBER_OF_TOUCHES];
  Bool draw[FT_NUMBER_OF_TOUCHES];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    FT_GestureTouch const *gTouch = &gesture->touch[i];
    switch(gTouch->state) {
      case ftGestureLeft:
        if(gTouch->val == 0) {
          draw[i] = bFalse;
          break;
        }
        radius[i] = GEST_RADIUS(param->v.size/2);
        coord[i].x = -radius[i];
        coord[i].y = param->v.size / 2;
        draw[i] = bTrue;
        break;
      case ftSGestureRight:
        if(gTouch->val == 0) {
          draw[i] = bFalse;
          break;
        }
        radius[i] = GEST_RADIUS(param->v.size/2);
        coord[i].x = param->h.size + radius[i];
        coord[i].y = param->v.size / 2;
        draw[i] = bTrue;
        break;
      case ftGestureTop:
        if(gTouch->val == 0) {
          draw[i] = bFalse;
          break;
        }
        radius[i] = GEST_RADIUS((GEST_TOP_X1-GEST_TOP_X0)/2);
        coord[i].x = GEST_TOP_X0 + (GEST_TOP_X1 - GEST_TOP_X0) / 2;
        coord[i].y = -radius[i];
        draw[i] = bTrue;
        break;
      case ftGestureTopRight:
        if(gTouch->val == 0) {
          draw[i] = bFalse;
          break;
        }
        radius[i] = GEST_RADIUS(param->h.size-GEST_TOPR_X0);
        coord[i].x = GEST_TOPR_X0 + (param->h.size - GEST_TOPR_X0) / 2;
        coord[i].y = -radius[i];
        draw[i] = bTrue;
        break;
      case ftGestureBottom:
        if(gTouch->val == 0) {
          draw[i] = bFalse;
          break;
        }
        radius[i] = GEST_RADIUS(param->h.size/2);
        coord[i].x = param->h.size / 2;
        coord[i].y = param->v.size + radius[i];
        draw[i] = bTrue;
        break;
      default:
        draw[i] = bFalse;
    }
  }

  Bool generalDraw = bFalse;
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    if(draw[i] == bTrue) {
      generalDraw = bTrue;
      break;
    }
  }

  if(generalDraw == bTrue) {
    ft_cmd_dl_set(cmds, ft_dl_save_context());
    ft_cmd_dl_set(cmds, ft_dl_color_a(80));
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(0x7E, 0x7E, 0x7E));
    ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimPoints));

    for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
      if(draw[i] == bFalse)
        continue;
      ft_cmd_dl_set(cmds, ft_dl_point_size((radius[i]+gesture->touch[i].val)*16));
      ft_cmd_dl_set(cmds, ft_dl_vertex2f(coord[i].x*16, coord[i].y*16));
    }

    ft_cmd_dl_set(cmds, ft_dl_end());
    ft_cmd_dl_set(cmds, ft_dl_restore_context());
  }

  gesture->draw = bFalse;
}

Bool ft_touch_gesture_draw(FT_Gesture *gesture)
{
  return gesture->draw;
}
