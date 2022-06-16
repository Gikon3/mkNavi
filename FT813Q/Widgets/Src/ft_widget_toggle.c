#include <string.h>
#include "util_vector.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_toggle.h"

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetToggle *toggle = (FT_WidgetToggle*)widget;
  const Bool redraw = toggle->touched == 0 ? bTrue : bFalse;
  ++toggle->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void relese(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetToggle *toggle = (FT_WidgetToggle*)widget;
  --toggle->touched;
  if(toggle->touched == 0) {
    toggle->state = toggle->state == bTrue ? bFalse : bTrue;
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetToggle *toggle = (FT_WidgetToggle*)widget;
  --toggle->touched;
  if(toggle->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetToggle *toggle = (FT_WidgetToggle*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  uint16_t options = FT_OPT_3D;
  if(toggle->touched > 0)
    options = FT_OPT_FLAT;
  const uint16_t state = toggle->state == bFalse ? 0 : 0xFFFF;
  ft_cmd_toggle(cmds, widget->absoluteCoord.x, widget->absoluteCoord.y, widget->dimensions.w,
      toggle->font, options, state, str_cdata(&toggle->text));
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetToggle *context = (FT_WidgetToggle*)widget;
  str_clear(&context->text);
}

FT_WidgetToggle* ft_widget_toggle_create(FT_Widget *owner)
{
  FT_WidgetToggle *widget = (FT_WidgetToggle*)ft_widget_create(owner, sizeof(FT_WidgetToggle));
  widget->parent.touch.type = ftWidgetTouchUsual;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese;
  slots->reset = reset;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->state = bFalse;
  widget->touched = 0;
  widget->text = make_str("off\xFFon");
  return widget;
}

void ft_widget_toggle_set_font(FT_WidgetToggle *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_toggle_set_state(FT_WidgetToggle *widget, Bool state)
{
  if(widget->state != state) {
    ft_widget_semphr_take(&widget->parent);
    widget->state = state;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_toggle_set_text(FT_WidgetToggle *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}
