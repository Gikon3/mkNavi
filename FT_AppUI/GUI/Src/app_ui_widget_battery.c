#include "util_vector.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "app_ui_widget_battery.h"
#include "lighting_white_11x18_argb2.h"

static void compile(FT_Widget *widget, Vector *cmds)
{
  AppUI_WidgetBattery *battery = (AppUI_WidgetBattery*)widget;
  const int16_t val = (widget->dimensions.w - 2) * battery->chargeLevel / 100;

  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  ft_cmd_dl_set(cmds, ft_dl_line_width(1*16));
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimLineStrip));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+widget->dimensions.w)*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+widget->dimensions.w)*16, (widget->absoluteCoord.y+widget->dimensions.h)*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, (widget->absoluteCoord.y+widget->dimensions.h)*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
  if(val > 0) {
    if(battery->chargeLevel <= 10)
      ft_cmd_dl_set(cmds, ft_dl_color_rgb(255, 0, 0));
    else if(battery->chargeLevel <= 25)
      ft_cmd_dl_set(cmds, ft_dl_color_rgb(255, 255, 0));
    else
      ft_cmd_dl_set(cmds, ft_dl_color_rgb(0, 255, 0));
    ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+1)*16, (widget->absoluteCoord.y+1)*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+1+val)*16, (widget->absoluteCoord.y+widget->dimensions.h-1)*16));
    ft_cmd_dl_set(cmds, ft_dl_end());
  }
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
  if(battery->charge == bTrue) {
    const int16_t lightingX = (widget->dimensions.w - battery->lighting.picture->resolution.w) / 2;
    ft_image_set_coord(&battery->lighting, lightingX, 0);
    ft_image_append(&battery->lighting, cmds);
  }
}

static void clear(FT_Widget *widget)
{
  AppUI_WidgetBattery *context = (AppUI_WidgetBattery*)widget;
  ft_image_destroy(&context->lighting);
  ft_screen_bitmap_handle_clear(widget->screen, context->lighting.handle);
}

AppUI_WidgetBattery* app_ui_widget_battery_create(FT_Widget *owner)
{
  AppUI_WidgetBattery *widget = (AppUI_WidgetBattery*)ft_widget_create(owner, sizeof(AppUI_WidgetBattery));
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->chargeLevel = 0;
  widget->charge = bFalse;
  ft_image_init(&widget->lighting, &widget->parent);
  ft_image_set_picture(&widget->lighting, &lightingWhite11x18ARGB2);
  return widget;
}

void app_ui_widget_battery_set_charge_level(AppUI_WidgetBattery *widget, uint8_t chargeLevel)
{
  if(widget->chargeLevel != chargeLevel && chargeLevel <= 100) {
    ft_widget_semphr_take(&widget->parent);
    widget->chargeLevel = chargeLevel;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void app_ui_widget_battery_set_charge(AppUI_WidgetBattery *widget, Bool charge)
{
  if(widget->charge != charge) {
    ft_widget_semphr_take(&widget->parent);
    widget->charge = charge;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}
