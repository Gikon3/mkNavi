#ifndef INC_APP_UI_WIDGET_BATTERY_H_
#define INC_APP_UI_WIDGET_BATTERY_H_

#include "stm32f4xx.h"
#include "util_types.h"
#include "ft_image_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  uint8_t   chargeLevel;  //< В процентах
  Bool      charge;       //< Состояние зарядки
  FT_Image  lighting;
} AppUI_WidgetBattery;

AppUI_WidgetBattery* app_ui_widget_battery_create(FT_Widget *owner);
void app_ui_widget_battery_set_charge_level(AppUI_WidgetBattery *widget, uint8_t charge);
void app_ui_widget_battery_set_charge(AppUI_WidgetBattery *widget, Bool charge);

#endif /* INC_APP_UI_WIDGET_BATTERY_H_ */
