#include "util_vector.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_checkbox.h"

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetCheckbox *checkbox = (FT_WidgetCheckbox*)widget;
  ++checkbox->touched;
}

static void relese(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetCheckbox *checkbox = (FT_WidgetCheckbox*)widget;
  --checkbox->touched;
  if(checkbox->touched == 0) {
    checkbox->enabled = checkbox->enabled == bTrue ? bFalse : bTrue;
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetCheckbox *checkbox = (FT_WidgetCheckbox*)widget;
  --checkbox->touched;
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetCheckbox *checkbox = (FT_WidgetCheckbox*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);

  const int16_t x = widget->absoluteCoord.x;
  const int16_t y = widget->absoluteCoord.y;
  const int16_t squareW = widget->dimensions.h;
  const int16_t gap = squareW / 5;
  ft_shape_rectangle_outline(cmds, x, y, squareW, squareW);
  if(checkbox->enabled == bTrue) {
    ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimLineStrip));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+gap)*16, (y+squareW/2)*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+squareW/3)*16, (y+squareW-gap)*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+squareW-gap)*16, (y+gap)*16));
    ft_cmd_dl_set(cmds, ft_dl_end());
  }

  FT_Font const *fontTable = ft_widget_font_table(widget);
  const int16_t textX = x + squareW + gap;
  const int16_t textY = y + (squareW - fontTable->height[checkbox->font-16]) / 2;
  const int16_t textW = widget->dimensions.w - squareW - gap;
  char *cutText = ft_util_text_cut(fontTable, checkbox->font, &checkbox->text, ftTextCutBack, textW, widget->dimensions.h);
  ft_cmd_text(cmds, textX, textY, checkbox->font, 0, cutText);
  ft_util_free(cutText);

  ft_cmd_dl_set(cmds, ft_dl_color_a(0));
  ft_shape_rectangle(cmds, x, y, squareW, squareW);

  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetCheckbox *context = (FT_WidgetCheckbox*)widget;
  str_clear(&context->text);
}

FT_WidgetCheckbox* ft_widget_checkbox_create(FT_Widget *owner)
{
  FT_WidgetCheckbox *widget = (FT_WidgetCheckbox*)ft_widget_create(owner, sizeof(FT_WidgetCheckbox));
  widget->parent.touch.type = ftWidgetTouchUsual;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese;
  slots->reset = reset;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->text = make_str_null();
  widget->enabled = bFalse;
  widget->touched = 0;
  return widget;
}

void ft_widget_checkbox_set_font(FT_WidgetCheckbox *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_checkbox_set_text(FT_WidgetCheckbox *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_checkbox_set_enabled(FT_WidgetCheckbox *widget, Bool enabled)
{
  if(widget->enabled != enabled) {
    ft_widget_semphr_take(&widget->parent);
    widget->enabled = enabled;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

Bool ft_widget_checkbox_enabled(FT_WidgetCheckbox const *widget)
{
  return widget->enabled;
}
