#include <stdio.h>
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_screens.h"
#include "clk_osc.h"
#include "usb_port_ctrl.h"
#include "bat_ctrl.h"
#include "a_lis331xx.h"
#include "b_bmp280.h"
#include "g_l3gxx.h"
#include "m_hmc5883l.h"
#include "m_lis3mdl.h"
#include "mt25q.h"
#include "sd_card.h"

static char const *const devicesList = {
    "Clock oscillator\n"
    "USB-Port Control\n"
    "Battery Control\n"
    "a LIS331HH\n"
    "a LIS331DLH\n"
    "g L3GD20H\n"
    "g L3G4250D\n"
    "m LIS3MDL\n"
    "m HMC5883L\n"
    "b BMP280\n"
    "TFT-Display\n"
    "Flash\n"
    "SD-Card"
};

FT_Screen* app_ui_screen_devices(FT_Core *core, AppUI *ui)
{
  AppUI_Devices *uiScreen = &ui->screen.devices;
  FT_Screen *screen = ft_screen_create(ui->core);

  FT_Widget *mainWidget = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(mainWidget, 20, APP_UI_STATUSBAR_HEIGHT+10);
  ft_widget_set_dimensions(mainWidget, 760, 425);
  ft_widget_set_heap_en(mainWidget, bFalse);

  const int16_t font = 28;
  const int16_t lineH = 25;

  FT_WidgetParagraph *paragraph = ft_widget_paragraph_create(mainWidget);
  ft_widget_set_dimensions(&paragraph->parent, 170, lineH*13);
  ft_widget_paragraph_set_font(paragraph, font);
  ft_widget_paragraph_set_text(paragraph, devicesList);

  FT_WidgetText *text = ft_widget_text_create(mainWidget);
  ft_widget_set_coord(&text->parent, 190, 0);
  ft_widget_set_dimensions(&text->parent, 140, lineH*13);
  ft_widget_text_set_font(text, font);
  ft_widget_text_set_max_lines(text, 13);

  uiScreen->time = 0;
  uiScreen->states = text;

  return screen;
}

Bool app_ui_screen_devices_refresh(AppUI *ui)
{
  AppUI_Devices *uiScreen = &ui->screen.devices;
  Bool draw = bFalse;

  const uint32_t time = xTaskGetTickCount();
  if((time-uiScreen->time) > pdMS_TO_TICKS(1000)) {
    uiScreen->time = time;

    Bool isReady[] = {
        clk_osc_is_ready() == HAL_OK ? bTrue : bFalse,
        usbp_ctrl_is_ready() == HAL_OK ? bTrue : bFalse,
        bat_is_ready() == HAL_OK ? bTrue : bFalse,
        a_lis331xx_is_ready(&lis331hh),
        a_lis331xx_is_ready(&lis331dlh),
        g_l3gxx_is_ready(&l3gd20h),
        g_l3gxx_is_ready(&l3g4250d),
        m_lis3mdl_is_ready(&lis3mdl),
        m_hmc5883l_is_ready(&hmc5883l),
        b_bmp230_is_ready(&bmp230),
        bTrue,
        mt25q_is_ready(mt25q_instance()) == HAL_OK ? bTrue : bFalse,
        sd_is_ready() == HAL_OK ? bTrue : bFalse
    };

    for(uint8_t i = 0; i < sizeof(isReady)/sizeof(isReady[0]); ++i) {
      if(isReady[i] == bTrue)
        ft_widget_text_append_text(uiScreen->states, "OK", 0xFF00FF);
      else
        ft_widget_text_append_text(uiScreen->states, "ERROR", 0xFF0000FF);
    }

    draw = bTrue;
  }

  return draw;
}
