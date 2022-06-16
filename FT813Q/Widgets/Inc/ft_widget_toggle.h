#ifndef WIDGETS_INC_FT_WIDGET_TOGGLE_H_
#define WIDGETS_INC_FT_WIDGET_TOGGLE_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  int16_t   font;
  Bool      state;
  uint8_t   touched;
  String    text;
} FT_WidgetToggle;

FT_WidgetToggle* ft_widget_toggle_create(FT_Widget *widget);
void ft_widget_toggle_set_font(FT_WidgetToggle *widget, int16_t font);
void ft_widget_toggle_set_state(FT_WidgetToggle *widget, Bool state);
void ft_widget_toggle_set_text(FT_WidgetToggle *widget, char const *text);

#endif /* WIDGETS_INC_FT_WIDGET_TOGGLE_H_ */
