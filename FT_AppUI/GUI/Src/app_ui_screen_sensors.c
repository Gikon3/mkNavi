#include <stdio.h>
#include "ft_co_pro_cmds_options.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_screens.h"
#include "sensor.h"
#include "bat_ctrl.h"


static FT_WidgetLabel* label_create(FT_Widget *owner, char const *text, int16_t x, int16_t y, uint16_t options)
{
  FT_WidgetLabel *label = ft_widget_label_create(owner);
  if(text)
    ft_widget_label_set_text(label, text);
  ft_widget_label_set_font(label, 18);
  if(options != 0)
    ft_widget_label_set_options(label, options);
  ft_widget_set_coord(&label->parent, x, y);
  return label;
}

static void table_create(FT_Widget *owner, AppUI_Sensors *uiScreen)
{
  const int16_t dx = 200;
  const int16_t dy = 24;
  const int16_t adcDx = 114;
  label_create(owner, "acc, g", dx*0, dy*0, 0);
  label_create(owner, "gyr, dps", dx*1, dy*0, 0);
  label_create(owner, "mag, G", dx*2,dy*0, 0);
  label_create(owner, "bar, Pa", dx*3, dy*0, 0);
  label_create(owner, "temperatue, C", 400, dy*4, FT_OPT_CENTERX);
  label_create(owner, "battery", 400, dy*6, FT_OPT_CENTERX);
  label_create(owner, "statuses", 400, dy*7, FT_OPT_CENTERX);
  label_create(owner, "charge", 0, dy*8, 0);
  label_create(owner, "power good", 0, dy*9, 0);
  label_create(owner, "VBUS detection", 0, dy*10, 0);
  label_create(owner, "ICO", 0, dy*11, 0);
  label_create(owner, "ADC", 400, dy*12, FT_OPT_CENTERX);
  label_create(owner, "IBUS, mA", adcDx*0, dy*13, 0);
  label_create(owner, "ICHG, mA", adcDx*1, dy*13, 0);
  label_create(owner, "VBUS, mV", adcDx*2, dy*13, 0);
  label_create(owner, "VBUT, mV", adcDx*3, dy*13, 0);
  label_create(owner, "VSYS, mV", adcDx*4, dy*13, 0);
  label_create(owner, "TS, %", adcDx*5, dy*13, 0);
  label_create(owner, "TSTDIE, C", adcDx*6, dy*13, 0);
}

static void table_fill(FT_Widget *owner, AppUI_Sensors *uiScreen)
{
  const int16_t dx = 200;
  const int16_t dy = 24;
  const int16_t adcDx = 114;

  for(uint8_t i = 0; i < 9; ++i) {
    const uint8_t columnNumber = i / 3;
    const uint8_t lineNumber = i < 3 ? i : i % 3;
    uiScreen->field[i] = label_create(owner, NULL, dx*columnNumber, dy*lineNumber, 0);
  }
  uiScreen->bar = label_create(owner, NULL, dx*3, dy*1, 0);

  // Значения температуры
  for(uint8_t i = 10; i < 13; ++i) {
    const uint8_t columnNumber = i - 10 + 1;
    const uint8_t lineNumber = 4;
    uiScreen->field[i] = label_create(owner, NULL, dx*columnNumber, dy*lineNumber, 0);
  }

  // Статусы батареи
  for(uint8_t i = 13; i < 17; ++i) {
    const uint8_t lineNumber = 9 + i - 15;
    uiScreen->field[i] = label_create(owner, NULL, 135, dy*lineNumber, 0);
  }
  // Значения АЦП батареи
  for(uint8_t i = 17; i < 24; ++i) {
    const uint8_t columnNumber = i - 17;
    uiScreen->field[i] = label_create(owner, NULL, adcDx*columnNumber, dy*13, 0);
  }
}

static char const* charge_stat()
{
  switch(bat_get_chrg_stat()) {
    case chrgNotCharging: return "not charging";
    case chrgTrickleCharge: return "Trickle charge";
    case chrgPreCharge: return "Pre-charge";
    case chrgFastCharge: return "Fast-charge";
    case chrgTaperCharge: return "Taper charge";
    case chrgTopOffTimerCharging: return "Top-off timer charging";
    case chrgChargeTerminationDone: return "Charge termination done";
  }
  return "Invalid value";
}

static char const* power_good_stat()
{
  switch(bat_get_pg_stat()) {
    case pgNotPowerGood: return "False";
    case pgPowerGood: return "True";
  }
  return "Invalid value";
}

static char const* vbus_stat()
{
  switch(bat_get_vbus_stat()){
    case vbusNoInput: return "No input";
    case vbusUsbHostSdp: return "USB host SDP -> PSEL high";
    case vbusUsbCdp: return "USB CDP (1.5 A)";
    case vbusAdapter: return "Adapter (3.0A) -> PSEL low";
    case vbusPoorSrc: return "POORSRC detected 7 consecutive times";
    case vbusUnknownAdapter: return "Unknown adapter (500 mA)";
    case vbusNonStandardAdapter: return "Non-standard Adapter (1 A/2 A/2.1 A/2.4 A)";
    case vbusOtg: return "OTG";
  }
  return "Invalid value";
}

static char const* ico_stat()
{
  switch(bat_get_ico_stat()) {
    case icoDisabled: return "ICO Disabled";
    case icoOptimization: return "ICO Optimization is in progress";
    case icoMaximumInputCurrent: return "Maximum input current detected";
  }
  return "Invalid value";
}

static void table_refill(AppUI_Sensors *uiScreen)
{
  char text[12];
  snprintf(text, sizeof(text), "%8.4f", sens.acc.ripe.x);
  ft_widget_label_set_text(uiScreen->acc.x, text);
  snprintf(text, sizeof(text), "%8.4f", sens.acc.ripe.y);
  ft_widget_label_set_text(uiScreen->acc.y, text);
  snprintf(text, sizeof(text), "%8.4f", sens.acc.ripe.z);
  ft_widget_label_set_text(uiScreen->acc.z, text);

  snprintf(text, sizeof(text), "%10.4f", sens.gyr.ripe.x);
  ft_widget_label_set_text(uiScreen->gyr.x, text);
  snprintf(text, sizeof(text), "%10.4f", sens.gyr.ripe.y);
  ft_widget_label_set_text(uiScreen->gyr.y, text);
  snprintf(text, sizeof(text), "%10.4f", sens.gyr.ripe.z);
  ft_widget_label_set_text(uiScreen->gyr.z, text);

  snprintf(text, sizeof(text), "%8.4f", sens.mag.ripe.x);
  ft_widget_label_set_text(uiScreen->mag.x, text);
  snprintf(text, sizeof(text), "%8.4f", sens.mag.ripe.y);
  ft_widget_label_set_text(uiScreen->mag.y, text);
  snprintf(text, sizeof(text), "%8.4f", sens.mag.ripe.z);
  ft_widget_label_set_text(uiScreen->mag.z, text);

  snprintf(text, sizeof(text), "%8.1f", sens.bar.ripe.val);
  ft_widget_label_set_text(uiScreen->bar, text);

  snprintf(text, sizeof(text), "%5.1f", sens.gyr.ripe.temp);
  ft_widget_label_set_text(uiScreen->temperature.gyr, text);
  snprintf(text, sizeof(text), "%5.1f", sens.mag.ripe.temp);
  ft_widget_label_set_text(uiScreen->temperature.mag, text);
  snprintf(text, sizeof(text), "%5.1f", sens.bar.ripe.temp);
  ft_widget_label_set_text(uiScreen->temperature.bar, text);

  ft_widget_label_set_text(uiScreen->battery.status.charge, charge_stat());
  ft_widget_label_set_text(uiScreen->battery.status.powerGood, power_good_stat());
  ft_widget_label_set_text(uiScreen->battery.status.vbusDetect, vbus_stat());
  ft_widget_label_set_text(uiScreen->battery.status.ico, ico_stat());

  snprintf(text, sizeof(text), "%5d", bat_get_ibus());
  ft_widget_label_set_text(uiScreen->battery.adc.ibus, text);
  snprintf(text, sizeof(text), "%5d", bat_get_ichrg());
  ft_widget_label_set_text(uiScreen->battery.adc.ichrg, text);
  snprintf(text, sizeof(text), "%5d", bat_get_vbus());
  ft_widget_label_set_text(uiScreen->battery.adc.vbus, text);
  snprintf(text, sizeof(text), "%5d", bat_get_vbat());
  ft_widget_label_set_text(uiScreen->battery.adc.vbut, text);
  snprintf(text, sizeof(text), "%5d", bat_get_vsys());
  ft_widget_label_set_text(uiScreen->battery.adc.vsys, text);
  snprintf(text, sizeof(text), "%2.3f", bat_get_ts());
  ft_widget_label_set_text(uiScreen->battery.adc.ts, text);
  snprintf(text, sizeof(text), "%3.1f", bat_get_tdie());
  ft_widget_label_set_text(uiScreen->battery.adc.tdie, text);
}


FT_Screen* app_ui_screen_sensors(FT_Core *core, AppUI *ui)
{
  AppUI_Sensors *uiScreen = &ui->screen.sensors;
  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *mainWidget = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(mainWidget, 5, APP_UI_STATUSBAR_HEIGHT+10);

  FT_Widget *namesLayout = ft_widget_create(mainWidget, 0);
  table_create(namesLayout, uiScreen);

  FT_Widget *valuesLayout = ft_widget_create(mainWidget, 0);
  ft_widget_set_y(valuesLayout, 24);
  table_fill(valuesLayout, uiScreen);
  ft_widget_set_heap_en_recursive(valuesLayout, bFalse);

  return screen;
}

Bool app_ui_screen_sensors_refresh(AppUI *ui)
{
  AppUI_Sensors *uiScreen = &ui->screen.sensors;
  table_refill(uiScreen);

  return bTrue;
}
