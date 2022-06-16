#include "util_vector.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_widget.h"
#include "ft_widget_rectangle.h"

static void compile(FT_Widget *widget, Vector *cmds)
{
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+widget->dimensions.w)*16,
      (widget->absoluteCoord.y+widget->dimensions.h)*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

FT_WidgetRectangle* ft_widget_rectangle_create(FT_Widget *owner)
{
  FT_WidgetRectangle *widget = (FT_WidgetRectangle*)ft_widget_create(owner, sizeof(FT_WidgetRectangle));
  widget->parent.action.compile = compile;
  return widget;
}
