#ifndef WIDGETS_INC_FT_WIDGET_IMAGE_H_
#define WIDGETS_INC_FT_WIDGET_IMAGE_H_

#include "ft_screen_impl.h"
#include "ft_widget_impl.h"
#include "ft_image_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  FT_Image  image;
} FT_WidgetImage;

FT_WidgetImage* ft_widget_image_create(FT_Widget *owner);
FT_Image* ft_widget_image_get_image(FT_WidgetImage *widget);

#endif /* WIDGETS_INC_FT_WIDGET_IMAGE_H_ */
