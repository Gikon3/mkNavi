#include "util_vector.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_button.h"

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetButton *btn = (FT_WidgetButton*)widget;
  const Bool redraw = btn->touched == 0 ? bTrue : bFalse;
  ++btn->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void relese_reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetButton *btn = (FT_WidgetButton*)widget;
  --btn->touched;
  if(btn->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetButton *btn = (FT_WidgetButton*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  uint16_t options = FT_OPT_3D;
  if(btn->touched > 0)
    options = FT_OPT_FLAT;
  ft_cmd_button(cmds, widget->absoluteCoord.x, widget->absoluteCoord.y, widget->dimensions.w, widget->dimensions.h,
                btn->font, options, str_data(&btn->text));
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetButton *context = (FT_WidgetButton*)widget;
  str_clear(&context->text);
}

FT_WidgetButton* ft_widget_button_create(FT_Widget *owner)
{
  FT_WidgetButton *widget = (FT_WidgetButton*)ft_widget_create(owner, sizeof(FT_WidgetButton));
  widget->parent.touch.type = ftWidgetTouchUsual;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese_reset;
  slots->reset = relese_reset;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->text = make_str_null();
  widget->touched = 0;
  return widget;
}

void ft_widget_button_set_font(FT_WidgetButton *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_button_set_text(FT_WidgetButton *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_button_append_text(FT_WidgetButton *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_append(&widget->text, text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}
