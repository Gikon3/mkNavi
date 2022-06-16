#ifndef WIDGETS_INC_FT_WIDGET_RECTANGLE_H_
#define WIDGETS_INC_FT_WIDGET_RECTANGLE_H_

#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
} FT_WidgetRectangle;

FT_WidgetRectangle* ft_widget_rectangle_create(FT_Widget *owner);

#endif /* WIDGETS_INC_FT_WIDGET_RECTANGLE_H_ */
