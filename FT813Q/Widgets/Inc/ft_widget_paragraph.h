#ifndef WIDGETS_INC_FT_WIDGET_PARAGRAPH_H_
#define WIDGETS_INC_FT_WIDGET_PARAGRAPH_H_

#include "stm32f4xx.h"
#include "ft_utils_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  char      *text;
  uint16_t  lines;
  int16_t   font;
} FT_WidgetParagraph;

FT_WidgetParagraph* ft_widget_paragraph_create(FT_Widget *owner);
void ft_widget_paragraph_set_font(FT_WidgetParagraph *widget, int16_t font);
void ft_widget_paragraph_set_text(FT_WidgetParagraph *widget, char const *text);

#endif /* WIDGETS_INC_FT_WIDGET_PARAGRAPH_H_ */
