#include <stdio.h>
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"
#include "station.h"
#include "ppm_cmd.h"

char const *bsValuesPattern = "BS%1u\n""%.2f\n""%.8f\n""%.6f\n""%.2f\n""%.3f\n""%.3f";

static void antenna_toggle_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  if(station.ppm.init.state == ppmInitStateOk)
    switch(station.ppm.outConnect) {
      case ppmOutIntAnt:
        ppm_cmd_amp(&station, flowPPM0, ppmOutExtAnt);
        break;
      default:
        ppm_cmd_amp(&station, flowPPM0, ppmOutIntAnt);
    }
}

static FT_WidgetParagraph *paragraph_create(FT_Widget *owner, int16_t x)
{
  const uint8_t lineH = 25;
  FT_WidgetParagraph *widget = ft_widget_paragraph_create(owner);
  ft_widget_set_coord(&widget->parent, x, 0);
  ft_widget_set_dimensions(&widget->parent, 120, lineH*7);
  ft_widget_paragraph_set_font(widget, 28);
  return widget;
}

FT_Screen *app_ui_screen_lsn(FT_Core *core, AppUI *ui)
{
  AppUI_LSN *uiScreen = &ui->screen.lsn;

  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *main = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(main, 20, APP_UI_STATUSBAR_HEIGHT+25);
  ft_widget_set_dimensions(main, 760, 410);
  ft_widget_set_heap_en(main, bFalse);

  const uint8_t lineH = 25;
  FT_WidgetParagraph *names = ft_widget_paragraph_create(main);
  ft_widget_set_coord(&names->parent, 0, lineH);
  ft_widget_set_dimensions(&names->parent, 120, lineH*6);
  ft_widget_paragraph_set_font(names, 28);
  ft_widget_paragraph_set_text(names, "delta\nlatitude\nlongitude\nheight\ndistance\ndist avg");

  uiScreen->time = 0;
  uiScreen->bs[0] = paragraph_create(main, 140);
  uiScreen->bs[1] = paragraph_create(main, 300);
  uiScreen->bs[2] = paragraph_create(main, 460);

  FT_WidgetLabel *antennaName = ft_widget_label_create(main);
  ft_widget_set_coord(&antennaName->parent, 0, 250);
  ft_widget_set_dimensions(&antennaName->parent, 120, lineH);
  ft_widget_label_set_font(antennaName, 28);
  ft_widget_label_set_text(antennaName, "antenna");

  FT_WidgetToggle *antennaToggle = ft_widget_toggle_create(main);
  ft_widget_set_color(&antennaToggle->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&antennaToggle->parent, 140, 260);
  ft_widget_set_dimensions(&antennaToggle->parent, 50, lineH);
  ft_widget_slots_connect(&antennaToggle->parent, NULL, NULL, antenna_toggle_clicked, NULL, ui);
  ft_widget_toggle_set_font(antennaToggle, 28);
  const Bool state = station.ppm.outConnect == ppmOutIntAnt ? bFalse : bTrue;
  ft_widget_toggle_set_state(antennaToggle, state);
  ft_widget_toggle_set_text(antennaToggle, "int""\xFF""ext");

  uiScreen->antenna = antennaToggle;

  return screen;
}

Bool app_ui_screen_lsn_refresh(AppUI *ui)
{
  Bool draw = bFalse;
  AppUI_LSN *uiScreen = &ui->screen.lsn;
  const uint32_t time = xTaskGetTickCount();
  if((time - uiScreen->time) > pdMS_TO_TICKS(300)) {
    uiScreen->time = time;
    char text[96];
    for(uint8_t i = 0; i < 3; ++i) {
      snprintf(text, sizeof(text), bsValuesPattern, i,
          station.bsLocation[i].distanceDelta, station.bsLocation[i].lat, station.bsLocation[i].lng, station.bsLocation[i].height,
          station.bsLocation[i].tempDistance-station.bsLocation[i].distanceDelta, station.bsLocation[i].maTempDistance.maVal);
      ft_widget_paragraph_set_text(uiScreen->bs[i], text);
    }

    const Bool state = station.ppm.outConnect == ppmOutIntAnt ? bFalse : bTrue;
    if(state != uiScreen->antenna->state)
      ft_widget_toggle_set_state(uiScreen->antenna, state);

    draw = bTrue;
  }
  return draw;
}
