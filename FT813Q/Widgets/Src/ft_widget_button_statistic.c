#include "util_vector.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_button_statistic.h"

#define MARGIN  5

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetButtonStatistic *btn = (FT_WidgetButtonStatistic*)widget;
  const Bool redraw = btn->touched == 0 ? bTrue : bFalse;
  ++btn->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void relese_reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetButtonStatistic *btn = (FT_WidgetButtonStatistic*)widget;
  --btn->touched;
  if(btn->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetButtonStatistic *btn = (FT_WidgetButtonStatistic*)widget;

  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  if(btn->touched > 0)
    ft_cmd_dl_set(cmds, ft_dl_color_a(150));
  ft_shape_rectangle(cmds, widget->absoluteCoord.x, widget->absoluteCoord.y, widget->dimensions.w, widget->dimensions.h);
  ft_cmd_dl_set(cmds, ft_dl_restore_context());

  ft_cmd_dl_set(cmds, ft_dl_save_context());
  if(btn->defaultTextColor == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(btn->textColor.red, btn->textColor.green, btn->textColor.blue));
  ft_cmd_text(cmds, widget->absoluteCoord.x+MARGIN, widget->absoluteCoord.y+MARGIN, btn->font, 0, str_cdata(&btn->text));
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetButtonStatistic *context = (FT_WidgetButtonStatistic*)widget;
  str_clear(&context->text);
}

FT_WidgetButtonStatistic* ft_widget_button_statistic_create(FT_Widget *owner)
{
  FT_WidgetButtonStatistic *widget = (FT_WidgetButtonStatistic*)ft_widget_create(owner, sizeof(FT_WidgetButtonStatistic));
  widget->parent.touch.type = ftWidgetTouchUsual;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese_reset;
  slots->reset = relese_reset;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->touched = 0;
  widget->defaultTextColor = bTrue;
  widget->textColor.value = 0xFF;
  widget->text = make_str_null();

  return widget;
}

void ft_widget_button_statistic_set_font(FT_WidgetButtonStatistic *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_button_statistic_set_text_color(FT_WidgetButtonStatistic *widget,
    uint8_t red, uint8_t green, uint8_t blue, Bool defaultFl)
{
  if(widget->textColor.red != red || widget->textColor.green != green ||
      widget->textColor.blue != blue || widget->defaultTextColor != defaultFl) {
    ft_widget_semphr_take(&widget->parent);
    widget->textColor.red = red;
    widget->textColor.green = green;
    widget->textColor.blue = blue;
    widget->defaultTextColor = defaultFl;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_button_statistic_set_text(FT_WidgetButtonStatistic *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

FT_Widget* ft_widget_button_statistic_body(FT_WidgetButtonStatistic *widget)
{
  ft_widget_semphr_take(&widget->parent);
  FT_Widget *body = ft_widget_create(&widget->parent, 0);
  FT_Font const *fontTable = ft_widget_font_table(&widget->parent);
  const uint8_t height = fontTable->height[widget->font-16];
  ft_widget_set_coord(body, widget->parent.coord.x+MARGIN, height+MARGIN);
  const int16_t w = widget->parent.dimensions.w - MARGIN * 2;
  const int16_t h = widget->parent.dimensions.h - height - MARGIN * 3;
  ft_widget_set_dimensions(body, w, h);
  ft_widget_semphr_give(&widget->parent);
  return body;
}
