#ifndef WIDGETS_INC_FT_BUTTON_WIDGET_H_
#define WIDGETS_INC_FT_BUTTON_WIDGET_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  int16_t font;
  uint8_t touched;
  String  text;
} FT_WidgetButton;

FT_WidgetButton* ft_widget_button_create(FT_Widget *widget);
void ft_widget_button_set_font(FT_WidgetButton *widget, int16_t font);
void ft_widget_button_set_text(FT_WidgetButton *widget, char const *text);
void ft_widget_button_append_text(FT_WidgetButton *widget, char const *text);

#endif /* WIDGETS_INC_FT_BUTTON_WIDGET_H_ */
