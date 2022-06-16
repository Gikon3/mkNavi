#ifndef WIDGETS_INC_FT_WIDGET_TEXT_H_
#define WIDGETS_INC_FT_WIDGET_TEXT_H_

#include "stm32f4xx.h"
#include "util_list.h"
#include "ft_utils_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  List        lines;
  int16_t     font;
  uint16_t    maxLines;
  uint16_t    cursor;
  Bool        touched;
  FT_TextCut  cut;
} FT_WidgetText;

FT_WidgetText* ft_widget_text_create(FT_Widget *owner);
void ft_widget_text_set_font(FT_WidgetText *widget, int16_t font);
void ft_widget_text_set_max_lines(FT_WidgetText *widget, uint16_t maxLines);
void ft_widget_text_append_text(FT_WidgetText *widget, char const *text, uint32_t color);
void ft_widget_text_clear_text(FT_WidgetText *widget);
void ft_widget_text_set_cut(FT_WidgetText *widget, FT_TextCut cut);

#endif /* WIDGETS_INC_FT_WIDGET_TEXT_H_ */
