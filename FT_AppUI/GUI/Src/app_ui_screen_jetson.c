#include <stdio.h>
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_screens.h"
#include "station.h"

static const int16_t font = 28;
static const int16_t lineH = 25;

static char const *const solutionPattern = "%llu\n""%-17.8f\n""%-17.8f\n""\n""%1u\n""%1u\n""%1u\n""%1u\n""%1u\n""%1u";
static char const *const bluetoothPattern = "%llu\n""%lu\n""%lu\n""%08lX\n""%08lX\n""%08lX\n""%08lX\n""%i";
static char const *const wifiPattern = "%llu\n""%02X-%02X-%02X-%02X-%02X-%02X\n""%f";
static char const *const gpsPattern = "%llu\n""%-17.8f\n""%-17.8f";

static FT_WidgetParagraph* paragraph_create(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h, char const *text)
{
  FT_WidgetParagraph *widget = ft_widget_paragraph_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_paragraph_set_font(widget, font);
  if(text)
    ft_widget_paragraph_set_text(widget, text);
  return widget;
}

FT_Screen* app_ui_screen_jetson(FT_Core *core, AppUI *ui)
{
  AppUI_Jetson *uiScreen = &ui->screen.jetson;
  FT_Screen *screen = ft_screen_create(ui->core);

  FT_Widget *mainWidget = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(mainWidget, 20, APP_UI_STATUSBAR_HEIGHT+10);
  ft_widget_set_dimensions(mainWidget, 760, 425);
  ft_widget_set_heap_en(mainWidget, bFalse);

  paragraph_create(mainWidget, 0, 0, 110, lineH*11, "Solution\n""- time\n""- lat\n""- long\n""- system\n"
      "-- LSN\n""-- GNSS\n""-- INS\n""-- Wi-Fi\n""-- Blu\n""-- GSM");
  paragraph_create(mainWidget, 380, 0, 110, lineH*9, "Bluetooth\n""- time\n""- major\n""- minor\n"
      "- UUID 0\n""- UUID 1\n""- UUID 2\n""- UUID 3\n""- RSSI");
  paragraph_create(mainWidget, 0, lineH*12, 110, lineH*4, "Wi-Fi\n""- time\n""- MAC\n""- distance");
  paragraph_create(mainWidget, 380, lineH*10, 110, lineH*4, "GPS\n""- time\n""- lat\n""- long");

  uiScreen->time = 0;
  uiScreen->solution = paragraph_create(mainWidget, 120, lineH, 200, lineH*11, NULL);
  uiScreen->bluetooth = paragraph_create(mainWidget, 500, lineH, 200, lineH*9, NULL);
  uiScreen->wifi = paragraph_create(mainWidget, 120, lineH*13, 200, lineH*4, NULL);
  uiScreen->gps = paragraph_create(mainWidget, 500, lineH*11, 200, lineH*4, NULL);

  return screen;
}

Bool app_ui_screen_jetson_refresh(AppUI *ui)
{
  AppUI_Jetson *uiScreen = &ui->screen.jetson;
  Bool draw = bFalse;

  const uint32_t time = xTaskGetTickCount();
  if((uiScreen->time-time) > pdMS_TO_TICKS(100)) {
    uiScreen->time = time;
    char text[200];
    snprintf(text, sizeof(text), solutionPattern,
        station.jet.solution.time, station.jet.solution.coord.lat, station.jet.solution.coord.lng,
        station.jet.solution.system.lsn, station.jet.solution.system.gnss, station.jet.solution.system.ins,
        station.jet.solution.system.wifi, station.jet.solution.system.blu, station.jet.solution.system.gsm);
    ft_widget_paragraph_set_text(uiScreen->solution, text);

    const uint32_t uuid[4] = {
        (uint32_t)station.jet.blu.uuid[0] << 24 | (uint32_t)station.jet.blu.uuid[1] << 16 |
        (uint32_t)station.jet.blu.uuid[2] << 8 | (uint32_t)station.jet.blu.uuid[3],
        (uint32_t)station.jet.blu.uuid[4] << 24 | (uint32_t)station.jet.blu.uuid[5] << 16 |
        (uint32_t)station.jet.blu.uuid[6] << 8 | (uint32_t)station.jet.blu.uuid[7],
        (uint32_t)station.jet.blu.uuid[8] << 24 | (uint32_t)station.jet.blu.uuid[9] << 16 |
        (uint32_t)station.jet.blu.uuid[10] << 8 | (uint32_t)station.jet.blu.uuid[11],
        (uint32_t)station.jet.blu.uuid[12] << 24 | (uint32_t)station.jet.blu.uuid[13] << 16 |
        (uint32_t)station.jet.blu.uuid[14] << 8 | (uint32_t)station.jet.blu.uuid[15]
    };
    snprintf(text, sizeof(text), bluetoothPattern, station.jet.blu.time, station.jet.blu.major, station.jet.blu.minor,
        station.jet.blu.rssi, uuid[0], uuid[1], uuid[2], uuid[3], station.jet.blu.rssi);
    ft_widget_paragraph_set_text(uiScreen->bluetooth, text);

    snprintf(text, sizeof(text), wifiPattern, station.jet.wifi.time, station.jet.wifi.mac[0], station.jet.wifi.mac[1],
        station.jet.wifi.mac[2], station.jet.wifi.mac[3], station.jet.wifi.mac[4], station.jet.wifi.mac[5],
        station.jet.wifi.distance);
    ft_widget_paragraph_set_text(uiScreen->wifi, text);

    snprintf(text, sizeof(text), gpsPattern, station.jet.gps.time, station.jet.gps.coord.lat, station.jet.gps.coord.lng);
    ft_widget_paragraph_set_text(uiScreen->gps, text);
  }

  return draw;
}
