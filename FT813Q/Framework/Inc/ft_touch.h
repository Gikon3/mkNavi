#ifndef FRAMEWORK_INC_FT_TOUCH_H_
#define FRAMEWORK_INC_FT_TOUCH_H_

#include "util_types.h"
#include "ft_core_impl.h"
#include "ft_touch_impl.h"

void ft_touch_touchscreen_init(FT_TouchScreen *touchScreen);
void ft_touch_touchscreen_destroy(FT_TouchScreen *touchScreen);
void ft_touch_touchscreen_load(FT_Core *core);
void ft_touch_touchscreen_process(FT_Core *core);
Bool ft_touch_gesture_queue_connect(FT_Core *core, QueueHandle_t qu, uint8_t gestures);
Bool ft_touch_gesture_queue_disconnect(FT_Core *core, QueueHandle_t qu);
void ft_touch_gesture_show(FT_Core *core, Vector *cmds);
Bool ft_touch_gesture_draw(FT_Gesture *gesture);

#endif /* FRAMEWORK_INC_FT_TOUCH_H_ */
