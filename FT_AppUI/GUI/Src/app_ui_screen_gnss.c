#include <stdio.h>
#include "ft_utils.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"
#include "flow_ctrl.h"
#include "station.h"
#include "mi_cmd.h"
#include "javad_cmd.h"
#include "util_types.h"

static void rtk_fix_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModeRTK_Fix);
#endif
}

static void rtk_float_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModeRTK_Float);
#endif
}

static void dgps_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModeDGPS);
#endif
}

static void wdgps_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModeWDGPS);
#endif
}

static void sp_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModeSP);
#endif
}

static void ppp_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
#ifdef DEVICE_TYPE_ATV
  javad_switch_solution(&station, flowGNSS1, gnssNavModePPP);
#endif
}

static void rtk_toggle_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  FT_WidgetToggle const *toggle = (FT_WidgetToggle const*)widget;
  mi_cmd_set_coord(&station, &flowCtrl);
  mi_cmd_rtk_subscription(&station, &flowCtrl, toggle->state);
}

static void ppp_toggle_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  FT_WidgetToggle const *toggle = (FT_WidgetToggle const*)widget;
  mi_cmd_set_coord(&station, &flowCtrl);
  mi_cmd_ppp_subscription(&station, &flowCtrl, toggle->state);
}

static FT_WidgetButton* btn_config(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h, char const *text)
{
  FT_WidgetButton *widget = ft_widget_button_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_button_set_font(widget, 28);
  ft_widget_button_set_text(widget, text);
  return widget;
}

static FT_WidgetParagraph *paragraph_create(FT_Widget *owner, int16_t x, int16_t y, char const *str)
{
  const uint8_t lineH = 25;
  FT_WidgetParagraph *widget = ft_widget_paragraph_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, 120, lineH*3);
  ft_widget_paragraph_set_font(widget, 28);
  if(str)
    ft_widget_paragraph_set_text(widget, str);
  return widget;
}

static FT_WidgetLabel* label_create(FT_Widget *owner, int16_t x, int16_t y, char const *text)
{
  FT_WidgetLabel *widget = ft_widget_label_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_label_set_font(widget, 28);
  if(text)
    ft_widget_label_set_text(widget, text);
  return widget;
}

static FT_WidgetToggle *toggle_create(FT_Widget *owner, int16_t y)
{
  FT_WidgetToggle *widget = ft_widget_toggle_create(owner);
  ft_widget_set_color(&widget->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&widget->parent, 170, y);
  ft_widget_set_dimensions(&widget->parent, 50, 25);
  ft_widget_toggle_set_font(widget, 28);
  ft_widget_toggle_set_text(widget, "off""\xFF""on");
  return widget;
}

FT_Screen *app_ui_screen_gnss(FT_Core *core, AppUI *ui)
{
  AppUI_GNSS *uiScreen = &ui->screen.gnss;

  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *main = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(main, 20, APP_UI_STATUSBAR_HEIGHT+25);
  ft_widget_set_dimensions(main, 760, 410);
  ft_widget_set_heap_en(main, bFalse);

  const int16_t font = 28;
  const uint8_t lineH = ft_util_font_height(&core->fontTable, font);
  paragraph_create(main, 0, 0, "latitude\n""longitude\n""altitude");
  uiScreen->coord = paragraph_create(main, 140, 0, NULL);

  const int16_t btnW = (760 - 20 * 5) / 6;
  const int16_t dxBtn = btnW + 20;

  FT_WidgetButton *btnRTK_Fix = btn_config(main, 0, 100, btnW, lineH*3, "RTK Fix");
  ft_widget_slots_connect(&btnRTK_Fix->parent, NULL, NULL, rtk_fix_clicked, NULL, ui);

  FT_WidgetButton *btnRTK_Float = btn_config(main, dxBtn, 100, btnW, lineH*3, "RTK Float");
  ft_widget_slots_connect(&btnRTK_Float->parent, NULL, NULL, rtk_float_clicked, NULL, ui);

  FT_WidgetButton *btnDGPS = btn_config(main, dxBtn*2, 100, btnW, lineH*3, "DGPS");
  ft_widget_slots_connect(&btnDGPS->parent, NULL, NULL, dgps_clicked, NULL, ui);

  FT_WidgetButton *btnWDGPS = btn_config(main, dxBtn*3, 100, btnW, lineH*3, "WDGPS");
  ft_widget_slots_connect(&btnWDGPS->parent, NULL, NULL, wdgps_clicked, NULL, ui);

  FT_WidgetButton *btnSP = btn_config(main, dxBtn*4, 100, btnW, lineH*3, "SP");
  ft_widget_slots_connect(&btnSP->parent, NULL, NULL, sp_clicked, NULL, ui);

  FT_WidgetButton *btnPPP = btn_config(main, dxBtn*5, 100, btnW, lineH*3, "PPP");
  ft_widget_slots_connect(&btnPPP->parent, NULL, NULL, ppp_clicked, NULL, ui);

  paragraph_create(main, 300, 0, "mode\n""solution\n""last RTCM");
  uiScreen->param = paragraph_create(main, 420, 0, NULL);

  label_create(main, 0, 240, "Subscribe RTK");
  uiScreen->rtkSwitch = toggle_create(main, 240);
  ft_widget_slots_connect(&uiScreen->rtkSwitch->parent, NULL, NULL, rtk_toggle_clicked, NULL, ui);

  label_create(main, 0, 300, "Subscribe PPP");
  uiScreen->pppSwitch = toggle_create(main, 300);
  ft_widget_slots_connect(&uiScreen->pppSwitch->parent, NULL, NULL, ppp_toggle_clicked, NULL, ui);

  uiScreen->time = 0;

  return screen;
}

static char const* navi_mode(GNSS_NavMode mode)
{
  switch(mode) {
    case gnssNavModeRTK_Fix: return "RTK Fixed";
    case gnssNavModeRTK_Float: return "RTK Float";
    case gnssNavModeDGPS: return "DGPS";
    case gnssNavModeWDGPS: return "WDFGPS";
    case gnssNavModeSP: return "SP";
    case gnssNavModePPP: return "PPP";
  }
  return NULL;
}

static char const* navi_solution(GNSS_NavType type)
{
  switch(type) {
    case gnssNavTypeNotValied: return "Data not valid";
    case gnssNavTypeAutonomous: return "Autonomous";
    case gnssNavTypeDifferencial: return "Differential";
    case gnssNavTypePPS: return "PPS";
    case gnssNavTypeRTK_Fix: return "RTK Fixed";
    case gnssNavTypeRTK_Float: return "RTK Float";
    case gnssNavTypeEstimated: return "Estimated";
    case gnssNavTypeManual: return "Manual";
    case gnssNavTypeSimulator: return "Simulator";
  }
  return "None";
}

Bool app_ui_screen_gnss_refresh(AppUI *ui)
{
  Bool draw = bFalse;
  AppUI_GNSS *uiScreen = &ui->screen.gnss;
  const uint32_t time = xTaskGetTickCount();
  if((time - uiScreen->time) > pdMS_TO_TICKS(300)) {
    uiScreen->time = time;
    char text[36];
    snprintf(text, sizeof(text), "%.8f\n""%.8f\n""%.2f",
        station.gnss.coord.lat, station.gnss.coord.lng, station.gnss.altitude);
    ft_widget_paragraph_set_text(uiScreen->coord, text);

    snprintf(text, sizeof(text), "%s\n""%s\n""%ld",
        navi_mode(station.gnss.navMode), navi_solution(station.gnss.solution), station.gnss.timegap);
    ft_widget_paragraph_set_text(uiScreen->param, text);

    if(station.mi.subscribe.rtk != uiScreen->rtkSwitch->state)
      ft_widget_toggle_set_state(uiScreen->rtkSwitch, station.mi.subscribe.rtk);

    if(station.mi.subscribe.ppp != uiScreen->pppSwitch->state)
      ft_widget_toggle_set_state(uiScreen->pppSwitch, station.mi.subscribe.ppp);

    draw = bTrue;
  }
  return draw;
}
