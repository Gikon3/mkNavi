#include <stdio.h>
#include "util_types.h"
#include "ft_utils.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"

extern Bluetooth* blth_instanse();

static void scanning_start_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  blth_scanning_start(blth_instanse());
}

static void scanning_stop_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI_Bluetooth *uiScreen = arg;
  blth_scanning_stop(blth_instanse());
  blth_get_devices(blth_instanse());
  uiScreen->time = xTaskGetTickCount();
}

static void disconnect_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  blth_disconnect(blth_instanse());
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

static FT_WidgetButton* btn_config(FT_Widget *owner, int16_t x, int16_t y, int16_t w, int16_t h, char const *text)
{
  FT_WidgetButton *widget = ft_widget_button_create(owner);
  ft_widget_set_coord(&widget->parent, x, y);
  ft_widget_set_dimensions(&widget->parent, w, h);
  ft_widget_button_set_font(widget, 28);
  ft_widget_button_set_text(widget, text);
  return widget;
}

static void initialize(AppUI_Bluetooth *uiScreen, BluetoothState state, Bluetooth *blth)
{
  if(uiScreen->mainWidget)
    ft_widget_destroy(uiScreen->mainWidget);

  FT_Widget *main = ft_widget_create(uiScreen->ownerWidget, 0);
  ft_widget_set_coord(main, 20, APP_UI_STATUSBAR_HEIGHT+25);
  ft_widget_set_dimensions(main, 760, 410);
  ft_widget_set_heap_en(main, bFalse);

  switch(state) {
    case blthStateIdle: {
      char passkeyText[16];
      snprintf(passkeyText, sizeof(passkeyText), "Passkey: %s", blth_passkey(blth));
      label_create(main, 420, 0, passkeyText);
      FT_WidgetButton *btnScanning = btn_config(main, 420, 40, 160, 60, "scanning start");
      ft_widget_slots_connect(&btnScanning->parent, NULL, NULL, scanning_start_clicked, NULL, uiScreen);

      FT_WidgetText *text = ft_widget_text_create(main);
      ft_widget_set_heap_en(&text->parent, bFalse);
      ft_widget_set_w(&text->parent, 400);
      ft_widget_text_set_font(text, 28);
      ft_widget_text_set_max_lines(text, 40);
      uiScreen->devices = text;
      break;
    }
    case blthStateScanning: {
      FT_WidgetButton *btnScanning = btn_config(main, 320, 195, 160, 60, "scanning stop");
      ft_widget_slots_connect(&btnScanning->parent, NULL, NULL, scanning_stop_clicked, NULL, uiScreen);
      break;
    }
    case blthStateConnected: {
      uint8_t btmac[6];
      blth_connect_state(blth, btmac);
      char connectDevice[29];
      snprintf(connectDevice, sizeof(connectDevice), "Connected: %02X:%02X:%02X:%02X:%02X:%02X",
          btmac[0], btmac[1], btmac[2], btmac[3], btmac[4], btmac[5]);
      label_create(main, 0, 0, connectDevice);
      FT_WidgetButton *btn = btn_config(main, 0, 100, 120, 60, "disconnect");
      ft_widget_slots_connect(&btn->parent, NULL, NULL, disconnect_clicked, NULL, uiScreen);
    }
    default: ;
  }

  uiScreen->mainWidget = main;
}

FT_Screen *app_ui_screen_bluetooth(FT_Core *core, AppUI *ui)
{
  AppUI_Bluetooth *uiScreen = &ui->screen.bluetooth;

  FT_Screen *screen = ft_screen_create(core);

  uiScreen->time = portMAX_DELAY;
  uiScreen->ownerWidget = ft_screen_widget(screen);
  uiScreen->mainWidget = NULL;

  Bluetooth *blth = blth_instanse();
  const BluetoothState state = blth_state(blth);
  initialize(uiScreen, state, blth);
  uiScreen->state = state;

  return screen;
}

Bool app_ui_screen_bluetooth_refresh(AppUI *ui)
{
  Bool draw = bFalse;
  AppUI_Bluetooth *uiScreen = &ui->screen.bluetooth;

  Bluetooth *blth = blth_instanse();
  const BluetoothState state = blth_state(blth);
  if(uiScreen->state != state) {
    uiScreen->state = state;
    initialize(uiScreen, state, blth);
    draw = bTrue;
  }
  const uint32_t time = xTaskGetTickCount();
  if(state == blthStateIdle && uiScreen->time != portMAX_DELAY && time > (uiScreen->time + pdMS_TO_TICKS(3000))) {
    uiScreen->time = portMAX_DELAY;
    ft_widget_text_clear_text(uiScreen->devices);
    BluetoothDevices const *devices = blth_get_devices(blth);
    for(uint8_t i = 0; i < devices->number; ++i) {
      BluetoothDevice const *device = &devices->device[i];
      char str[32];
      snprintf(str, sizeof(str), "%d dB, %d dB, %s", device->rssi, device->txPower, device->name);
      ft_widget_text_append_text(uiScreen->devices, str, 0xFF);
    }
  }

  return draw;
}
