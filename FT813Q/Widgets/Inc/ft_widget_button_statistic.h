#ifndef WIDGETS_INC_FT_WIDGET_BUTTON_STATISTIC_H_
#define WIDGETS_INC_FT_WIDGET_BUTTON_STATISTIC_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  int16_t   font;
  uint8_t   touched;
  Bool      defaultTextColor;
  FT_Color  textColor;
  String    text;
} FT_WidgetButtonStatistic;

FT_WidgetButtonStatistic* ft_widget_button_statistic_create(FT_Widget *owner);
void ft_widget_button_statistic_set_font(FT_WidgetButtonStatistic *widget, int16_t font);
void ft_widget_button_statistic_set_text_color(FT_WidgetButtonStatistic *widget,
    uint8_t red, uint8_t green, uint8_t blue, Bool defaultFl);
void ft_widget_button_statistic_set_text(FT_WidgetButtonStatistic *widget, char const *text);
FT_Widget* ft_widget_button_statistic_body(FT_WidgetButtonStatistic *widget);

#endif /* WIDGETS_INC_FT_WIDGET_BUTTON_STATISTIC_H_ */
