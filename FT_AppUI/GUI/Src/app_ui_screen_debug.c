#include "app_ui_screens.h"
#include "app_ui_manager.h"
#include "ft_screen.h"
#include "ft_widgets.h"

static void touch_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_debug_touch);
}

static void gesture_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_debug_gesture);
}

static void sensors_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_sensors);
}

static void jetson_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_jetson);
}

static void devices_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_devices);
}

static FT_WidgetButton* btn_config(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h,
                       int16_t font, char const *text)
{
  FT_WidgetButton *widget = ft_widget_button_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_button_set_font(widget, font);
  ft_widget_button_set_text(widget, text);
  return widget;
}

FT_Screen* app_ui_screen_debug(FT_Core *core, AppUI *ui)
{
  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *btns = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(btns, 20, APP_UI_STATUSBAR_HEIGHT+25);
  ft_widget_set_heap_en(btns, bFalse);

  const int16_t btnW = 120;
  const int16_t btnH = 80;
  const int16_t btnFont = 28;
  const int16_t btnsDx = btnW + 20;
  const int16_t btnsDy = btnH + 20;

  FT_WidgetButton *touchBtn = btn_config(btns, btnsDx*0, btnsDy*0, btnW, btnH, btnFont, "touch");
  ft_widget_slots_connect(&touchBtn->parent, NULL, NULL, touch_clicked, NULL, ui);

  FT_WidgetButton *gestureBtn = btn_config(btns, btnsDx*0, btnsDy*1, btnW, btnH, btnFont, "gesture");
  ft_widget_slots_connect(&gestureBtn->parent, NULL, NULL, gesture_clicked, NULL, ui);

  FT_WidgetButton *sensorsBtn = btn_config(btns, btnsDx*1, btnsDy*0, btnW, btnH, btnFont, "sensors");
  ft_widget_slots_connect(&sensorsBtn->parent, NULL, NULL, sensors_clicked, NULL, ui);

  FT_WidgetButton *jetsonBtn = btn_config(btns, btnsDx*1, btnsDy*1, btnW, btnH, btnFont, "jetson");
  ft_widget_slots_connect(&jetsonBtn->parent, NULL, NULL, jetson_clicked, NULL, ui);

  FT_WidgetButton *devicesBtn = btn_config(btns, btnsDx*2, btnsDy*0, btnW, btnH, btnFont, "devices");
  ft_widget_slots_connect(&devicesBtn->parent, NULL, NULL, devices_clicked, NULL, ui);

  return screen;
}

Bool app_ui_screen_debug_refresh(AppUI *ui)
{
  return bFalse;
}
