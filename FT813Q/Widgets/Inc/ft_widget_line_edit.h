#ifndef WIDGETS_INC_FT_WIDGET_LINE_EDIT_H_
#define WIDGETS_INC_FT_WIDGET_LINE_EDIT_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "util_string.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct _FT_WidgetLineEdit
{
  FT_WIDGET_HEADER
  struct
  {
    TimerHandle_t cursorBlinking;
    TimerHandle_t outOfBonds;
  } timers;
  Bool          enable;
  Bool          placeholderText;
  int16_t       font;
  String        text;
  struct
  {
    Bool        visible;
    uint16_t    startPos;
    uint16_t    endPos;
    uint16_t    position;
  } cursor;
} FT_WidgetLineEdit;

FT_WidgetLineEdit* ft_widget_line_edit_create(FT_Widget *widget);
void ft_widget_line_edit_set_font(FT_WidgetLineEdit *widget, int16_t font);
void ft_widget_line_edit_set_text(FT_WidgetLineEdit *widget, char const *text);
void ft_widget_line_edit_set_enable(FT_WidgetLineEdit *widget, Bool enable);
void ft_widget_line_edit_inser_char(FT_WidgetLineEdit *widget, char ch);
void ft_widget_line_edit_erase_char(FT_WidgetLineEdit *widget);
char const* ft_widget_line_edit_text(FT_WidgetLineEdit *widget);

#endif /* WIDGETS_INC_FT_WIDGET_LINE_EDIT_H_ */
