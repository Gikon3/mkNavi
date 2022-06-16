#ifndef WIDGETS_INC_FT_WIDGET_CHECKBOX_H_
#define WIDGETS_INC_FT_WIDGET_CHECKBOX_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  int16_t font;
  Bool    enabled;
  uint8_t touched;
  String  text;
} FT_WidgetCheckbox;

FT_WidgetCheckbox* ft_widget_checkbox_create(FT_Widget *widget);
void ft_widget_checkbox_set_font(FT_WidgetCheckbox *widget, int16_t font);
void ft_widget_checkbox_set_text(FT_WidgetCheckbox *widget, char const *text);
void ft_widget_checkbox_set_enabled(FT_WidgetCheckbox *widget, Bool enabled);
Bool ft_widget_checkbox_enabled(FT_WidgetCheckbox const *widget);

#endif /* WIDGETS_INC_FT_WIDGET_CHECKBOX_H_ */
