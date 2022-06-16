#include "util_vector.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_image.h"
#include "ft_widget_image.h"

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetImage *image = (FT_WidgetImage*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  ft_image_append(&image->image, cmds);
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetImage *context = (FT_WidgetImage*)widget;
  ft_image_destroy(&context->image);
  ft_screen_bitmap_handle_clear(widget->screen, context->image.handle);
}

FT_WidgetImage* ft_widget_image_create(FT_Widget *owner)
{
  FT_WidgetImage *widget = (FT_WidgetImage*)ft_widget_create(owner, sizeof(FT_WidgetImage));
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  ft_image_init(&widget->image, &widget->parent);
  return widget;
}

FT_Image* ft_widget_image_get_image(FT_WidgetImage *widget)
{
  return &widget->image;
}
