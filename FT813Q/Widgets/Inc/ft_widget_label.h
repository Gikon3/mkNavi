#ifndef WIDGETS_INC_FT_WIDGET_LABEL_H_
#define WIDGETS_INC_FT_WIDGET_LABEL_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_utils_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  int16_t     font;
  uint16_t    options;
  String      text;
  FT_TextCut  cut;
} FT_WidgetLabel;

FT_WidgetLabel* ft_widget_label_create(FT_Widget *owner);
void ft_widget_label_set_font(FT_WidgetLabel *widget, int16_t font);
void ft_widget_label_append_text(FT_WidgetLabel *widget, char const *text);
void ft_widget_label_set_text(FT_WidgetLabel *widget, char const *text);
void ft_widget_label_set_options(FT_WidgetLabel *widget, uint16_t options);
void ft_widget_label_set_cut(FT_WidgetLabel *widget, FT_TextCut cut);

#endif /* WIDGETS_INC_FT_WIDGET_LABEL_H_ */
