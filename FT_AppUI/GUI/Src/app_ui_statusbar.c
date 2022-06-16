#include <stdio.h>
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_screens.h"
#include "app_ui_widget_battery.h"
#include "rtc_if.h"
#include "bat_ctrl.h"
#include "ppm_white_18x18_argb2.h"
#include "station.h"

void app_ui_statusbar_create(FT_Core *core, FT_Screen *screen, AppUI_StatusBar *uiStatusbar)
{
  FT_WidgetRectangle *layout = ft_widget_rectangle_create(ft_screen_widget(screen));
  ft_widget_set_color(&layout->parent, 80, 80, 80, bFalse);
  ft_widget_set_alpha(&layout->parent, 200, bFalse);
  ft_widget_set_h(&layout->parent, APP_UI_STATUSBAR_HEIGHT);

  FT_WidgetImage *ppmIcon = NULL;
  if(station.ppm.init.state != ppmInitStateOk) {
    ppmIcon = ft_widget_image_create(&layout->parent);
    FT_Image *image = ft_widget_image_get_image(ppmIcon);
    ft_image_set_picture(image, &ppmWhite18x18ARGB2);
    FT_Color color = { .value = 0xFFFF00FF };
    ft_image_set_color(image, color);
    ft_widget_set_coord(&ppmIcon->parent, 5, 4);
  }

  const int16_t batX = 670;
  const int16_t batY = 4;
  const int16_t batW = 30;
  const int16_t batH = 15;
  AppUI_WidgetBattery *battery = app_ui_widget_battery_create(&layout->parent);
  ft_widget_set_color(&battery->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&battery->parent, batX, batY);
  ft_widget_set_dimensions(&battery->parent, batW, batH);
  app_ui_widget_battery_set_charge(battery, bFalse);
  app_ui_widget_battery_set_charge_level(battery, 0);

  const int16_t timeFont = 28;
  const int16_t timeX = batX + batW + 10;
  const int16_t timeY = batY - 5;
  const int16_t timeW = 800 - timeX;
  FT_WidgetLabel *time = ft_widget_label_create(&layout->parent);
  ft_widget_set_color(&time->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&time->parent, timeX, timeY);
  ft_widget_set_w(&time->parent, timeW);
  char timeText[9];
  snprintf(timeText, sizeof(timeText), "%02u:%02u:%02u", 0, 0, 0);
  ft_widget_label_set_font(time, timeFont);
  ft_widget_label_set_text(time, timeText);

  FT_WidgetLabel *freeHeapSize = ft_widget_label_create(&layout->parent);
  ft_widget_set_color(&freeHeapSize->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&freeHeapSize->parent, batX-100, timeY);
  ft_widget_set_w(&freeHeapSize->parent, 90);
  ft_widget_label_set_font(freeHeapSize, timeFont);

  uiStatusbar->ticks = 0;
  uiStatusbar->ppmIcon = ppmIcon;
  uiStatusbar->battery.widget = battery;
  uiStatusbar->battery.charge = bFalse;
  uiStatusbar->battery.val = 0;
  uiStatusbar->time.widget = time;
  uiStatusbar->time.hour = 0;
  uiStatusbar->time.min = 0;
  uiStatusbar->time.sec = 0;
  uiStatusbar->freeHeapSize = freeHeapSize;
}

Bool app_ui_statusbar_refresh(AppUI_StatusBar *uiStatusbar)
{
  Bool draw = bFalse;
  rtc_datetime(&rtcIF);
  if(uiStatusbar->time.sec != rtcIF.time.sec) {
    uiStatusbar->time.sec = rtcIF.time.sec;
    uiStatusbar->time.min = rtcIF.time.min;
    uiStatusbar->time.hour = rtcIF.time.hour;
    char time[9];
    snprintf(time, sizeof(time), "%02u:%02u:%02u", uiStatusbar->time.hour, uiStatusbar->time.min, uiStatusbar->time.sec);
    ft_widget_label_set_text(uiStatusbar->time.widget, time);

    const uint16_t range = 8400 - 7800;
    const uint8_t chargeLevel = (range - (8400 - bat_get_vbat())) * 100 / range;
    if(uiStatusbar->battery.val != chargeLevel) {
      uiStatusbar->battery.val = chargeLevel;
      app_ui_widget_battery_set_charge_level(uiStatusbar->battery.widget, uiStatusbar->battery.val);
    }

    if(uiStatusbar->ppmIcon) {
      if(station.ppm.init.state == ppmInitStateOk) {
        DListContainer *container = dlist_front(&uiStatusbar->ppmIcon->owner->widgets);
        while(container) {
          FT_Widget *widget = *(FT_Widget**)dlist_item(container);
          if(widget == &uiStatusbar->ppmIcon->parent) {
            dlist_erase(&uiStatusbar->ppmIcon->owner->widgets, container, NULL);
            break;
          }
          container = dlist_next(container);
        }
        ft_widget_destroy(&uiStatusbar->ppmIcon->parent);
        uiStatusbar->ppmIcon = NULL;
      }
      else if(station.ppm.init.state == ppmInitStateFail) {
        FT_Color color = { .value = 0xFF0000FF };
        ft_image_set_color(ft_widget_image_get_image(uiStatusbar->ppmIcon), color);
      }
    }

    draw = bTrue;
  }

  const Bool charge = bat_get_chrg_stat() == chrgNotCharging ? bFalse : bTrue;
  if(uiStatusbar->battery.charge != charge) {
    uiStatusbar->battery.charge = charge;
    app_ui_widget_battery_set_charge(uiStatusbar->battery.widget, uiStatusbar->battery.charge);
    draw = bTrue;
  }

  const uint32_t ticks = xTaskGetTickCount();
  if((ticks-uiStatusbar->ticks) > pdMS_TO_TICKS(250)) {
    uiStatusbar->ticks = ticks;
    char text[12];
    snprintf(text, sizeof(text), "%u", xPortGetFreeHeapSize());
    ft_widget_label_set_text(uiStatusbar->freeHeapSize, text);
    draw = bTrue;
  }

  return draw;
}
