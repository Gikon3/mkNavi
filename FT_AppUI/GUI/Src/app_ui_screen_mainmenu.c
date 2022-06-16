#include <stdio.h>
#include "ft_co_pro_cmds_options.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"
#include "station.h"

#define BTN_STAT_BODY_FONT  28

static void lsn_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_lsn);
}

static void gnss_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_gnss);
}

static void log_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_log);
}

static void terminal_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_terminal);
}

static void bluetooth_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_bluetooth);
}

static void debug_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_debug);
}

static FT_WidgetButtonStatistic* btn_stat_config(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h,
    char const *text)
{
  FT_WidgetButtonStatistic *widget = ft_widget_button_statistic_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_button_statistic_set_text_color(widget, 255, 128, 0, bFalse);
  ft_widget_button_statistic_set_font(widget, 30);
  ft_widget_button_statistic_set_text(widget, text);
  return widget;
}

static void btn_stat_lsn_body_create(FT_Widget *owner, AppUI_MainMenu *uiScreen)
{
  FT_WidgetLabel *latitude = ft_widget_label_create(owner);
  ft_widget_set_color(&latitude->parent, 255, 128, 0, bFalse);
  ft_widget_label_set_font(latitude, BTN_STAT_BODY_FONT);

  FT_WidgetLabel *longitude = ft_widget_label_create(owner);
  ft_widget_set_color(&longitude->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&longitude->parent, 0, 30);
  ft_widget_label_set_font(longitude, BTN_STAT_BODY_FONT);

  FT_WidgetLabel *altitude = ft_widget_label_create(owner);
  ft_widget_set_color(&altitude->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&altitude->parent, 0, 60);
  ft_widget_label_set_font(altitude, BTN_STAT_BODY_FONT);

  uiScreen->lsn.lat = latitude;
  uiScreen->lsn.lng = longitude;
  uiScreen->lsn.alt = altitude;
}

static void btn_stat_gnss_body_create(FT_Widget *owner, AppUI_MainMenu *uiScreen)
{
  FT_WidgetLabel *latitude = ft_widget_label_create(owner);
  ft_widget_set_color(&latitude->parent, 255, 128, 0, bFalse);
  ft_widget_label_set_font(latitude, BTN_STAT_BODY_FONT);

  FT_WidgetLabel *longitude = ft_widget_label_create(owner);
  ft_widget_set_color(&longitude->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&longitude->parent, 0, 30);
  ft_widget_label_set_font(longitude, BTN_STAT_BODY_FONT);

  FT_WidgetLabel *altitude = ft_widget_label_create(owner);
  ft_widget_set_color(&altitude->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&altitude->parent, 0, 60);
  ft_widget_label_set_font(altitude, BTN_STAT_BODY_FONT);

  uiScreen->gnss.lat = latitude;
  uiScreen->gnss.lng = longitude;
  uiScreen->gnss.alt = altitude;
}

static FT_WidgetButton* btn_config(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h,
    char const *text)
{
  FT_WidgetButton *widget = ft_widget_button_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_button_set_font(widget, 28);
  ft_widget_button_set_text(widget, text);
  return widget;
}

FT_Screen *app_ui_screen_mainmenu(FT_Core *core, AppUI *ui)
{
  AppUI_MainMenu *uiScreen = &ui->screen.mainMenu;

  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *btns = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(btns, 20, APP_UI_STATUSBAR_HEIGHT+25);
  ft_widget_set_dimensions(btns, 760, 410);
  ft_widget_set_heap_en(btns, bFalse);

  const int16_t btnStatW = 400;
  const int16_t btnStatH = 140;

  FT_WidgetButtonStatistic *lsnBtn = btn_stat_config(btns, 0, 50, btnStatW, btnStatH, "LSN");
  ft_widget_slots_connect(&lsnBtn->parent, NULL, NULL, lsn_clicked, NULL, ui);
  FT_Widget *lsnStat = ft_widget_button_statistic_body(lsnBtn);
  ft_widget_set_heap_en(lsnStat, bFalse);
  btn_stat_lsn_body_create(lsnStat, uiScreen);

  FT_WidgetButtonStatistic *gnssBtn = btn_stat_config(btns, 0, 240, btnStatW, btnStatH, "GNSS");
  ft_widget_slots_connect(&gnssBtn->parent, NULL, NULL, gnss_clicked, NULL, ui);
  FT_Widget *gnssStat = ft_widget_button_statistic_body(gnssBtn);
  ft_widget_set_heap_en(gnssStat, bFalse);
  btn_stat_gnss_body_create(gnssStat, uiScreen);

  const int16_t bntX = 640;
  const int16_t btnW = 120;
  const int16_t btnH = 87;
  const int16_t btnsDx = btnW + 20;
  const int16_t btnsDy = btnH + 20;

  FT_WidgetButton *sensBtn = btn_config(btns, bntX+btnsDx*0, btnsDy*0, btnW, btnH, "log");
  ft_widget_slots_connect(&sensBtn->parent, NULL, NULL, log_clicked, NULL, ui);

  FT_WidgetButton *terminalBtn = btn_config(btns, bntX+btnsDx*0, btnsDy*1, btnW, btnH, "terminal");
  ft_widget_slots_connect(&terminalBtn->parent, NULL, NULL, terminal_clicked, NULL, ui);

  FT_WidgetButton *bluetoothBtn = btn_config(btns, bntX+btnsDx*0, btnsDy*2, btnW, btnH, "bluetooth");
  ft_widget_slots_connect(&bluetoothBtn->parent, NULL, NULL, bluetooth_clicked, NULL, ui);

  FT_WidgetButton *debugBtn = btn_config(btns, bntX+btnsDx*0, btnsDy*3, btnW, btnH, "debug");
  ft_widget_slots_connect(&debugBtn->parent, NULL, NULL, debug_clicked, NULL, ui);

  uiScreen->time = 0;

  return screen;
}

Bool app_ui_screen_mainmenu_refresh(AppUI *ui)
{
  Bool draw = bFalse;
  AppUI_MainMenu *uiScreen = &ui->screen.mainMenu;
  const uint32_t time = xTaskGetTickCount();
  if((time - uiScreen->time) > pdMS_TO_TICKS(300)) {
    uiScreen->time = time;
    char text[20];
    // ЛСН
    snprintf(text, sizeof(text), "%12.8f", station.roverPosition.lat);
    ft_widget_label_set_text(uiScreen->lsn.lat, text);

    snprintf(text, sizeof(text), "%12.8f", station.roverPosition.lng);
    ft_widget_label_set_text(uiScreen->lsn.lng, text);

    snprintf(text, sizeof(text), "%12.8f", station.roverPosition.height);
    ft_widget_label_set_text(uiScreen->lsn.alt, text);

    // ГНСС
    snprintf(text, sizeof(text), "%12.8f", station.gnss.coord.lat);
    ft_widget_label_set_text(uiScreen->gnss.lat, text);

    snprintf(text, sizeof(text), "%12.8f", station.gnss.coord.lng);
    ft_widget_label_set_text(uiScreen->gnss.lng, text);

    snprintf(text, sizeof(text), "%12.8f", station.gnss.altitude);
    ft_widget_label_set_text(uiScreen->gnss.alt, text);

    draw = bTrue;
  }
  return draw;
}
