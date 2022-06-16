#include <stdio.h>
#include "app_ui_screens.h"
#include "app_ui_manager.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "sd_card.h"

static void cross_tab_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  AppUI_LogTab *uiLogTab = arg;
  ft_widget_tab_delete_tab(uiLogTab->log->window, uiLogTab->body);
  uiLogTab->body = NULL;
  const int8_t handle = uiLogTab->fileHandle;
  if(handle >= 0) {
    sd_stop(sd_instance(), handle);
    uiLogTab->fileHandle = -1;
  }
}

static void set_categories(FT_WidgetCheckbox const *checkbox, AppUI_LogTab *uiLogTab, SD_CardCategory category)
{
  if(ft_widget_checkbox_enabled(checkbox) == bTrue)
    uiLogTab->categories |= category;
  else
    uiLogTab->categories &= ~category;
  sd_set_categories(sd_instance(), uiLogTab->fileHandle, uiLogTab->categories);
}

static void checkbox_sensor_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  FT_WidgetCheckbox const *checkbox = (FT_WidgetCheckbox const*)widget;
  AppUI_LogTab *uiLogTab = arg;
  set_categories(checkbox, uiLogTab, sdCategorySensors);
}

static void checkbox_jetson_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  FT_WidgetCheckbox const *checkbox = (FT_WidgetCheckbox const*)widget;
  AppUI_LogTab *uiLogTab = arg;
  set_categories(checkbox, uiLogTab, sdCategoryJetson);
}

static void checkbox_ppm_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  FT_WidgetCheckbox const *checkbox = (FT_WidgetCheckbox const*)widget;
  AppUI_LogTab *uiLogTab = arg;
  set_categories(checkbox, uiLogTab, sdCategoryPPM);
}

static void checkbox_gnss_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  FT_WidgetCheckbox const *checkbox = (FT_WidgetCheckbox const*)widget;
  AppUI_LogTab *uiLogTab = arg;
  set_categories(checkbox, uiLogTab, sdCategoryGNSS);
}

static void toggle_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *param, void *arg)
{
  FT_WidgetToggle const *toggle = (FT_WidgetToggle const*)widget;
  AppUI_LogTab *uiLogTab = arg;
  if(toggle->state == bTrue) {
    char const *filename = ft_widget_line_edit_text(uiLogTab->lineEdit);
    uiLogTab->fileHandle = sd_start(sd_instance(), filename, uiLogTab->categories);
    if(uiLogTab->fileHandle >= 0)
      ft_widget_tab_set_tab_name(uiLogTab->log->window, uiLogTab->body, filename);
    else
      ft_widget_toggle_set_state(uiLogTab->toggle, bFalse);
    ft_screen_draw(uiLogTab->log->screen);
  }
  else {
    sd_stop(sd_instance(), uiLogTab->fileHandle);
    uiLogTab->fileHandle = -1;
  }
}

static FT_WidgetCheckbox* checkbox_create(FT_Widget *owner, uint8_t numberStr, int16_t width, char const *text)
{
  const int16_t height = 25;
  const int16_t y = 80 + (height + 10) * numberStr;
  FT_WidgetCheckbox *checkbox = ft_widget_checkbox_create(owner);
  ft_widget_set_coord(&checkbox->parent, 10, y);
  ft_widget_set_dimensions(&checkbox->parent, width, height);
  ft_widget_checkbox_set_font(checkbox, 28);
  ft_widget_checkbox_set_text(checkbox, text);
  return checkbox;
}

static FT_WidgetLabel* label_create(FT_Widget *owner, int16_t y, char const *text)
{
  FT_WidgetLabel *widget = ft_widget_label_create(owner);
  ft_widget_set_coord(&widget->parent, 300, y);
  ft_widget_set_dimensions(&widget->parent, 300, 25);
  ft_widget_label_set_cut(widget, ftTextCutFront);
  ft_widget_label_set_font(widget, 28);
  if(text)
    ft_widget_label_set_text(widget, text);
  return widget;
}

static FT_Widget* tab_make_and_config(AppUI_Log *uiScreen, uint8_t index)
{
  char text[19];
  snprintf(text, sizeof(text), "Filename %u", index);
  FT_Widget *body = ft_widget_tab_new_tab(uiScreen->window, text);
  if(!body)
    return NULL;
  ft_widget_tab_delete_slot_connect(uiScreen->window, body, cross_tab_clicked, &uiScreen->tab[index]);

  FT_WidgetLineEdit *lineEdit = ft_widget_line_edit_create(body);
  ft_widget_set_coord(&lineEdit->parent, 10, 10);
  ft_widget_set_dimensions(&lineEdit->parent, 250, 40);
  ft_widget_line_edit_set_font(lineEdit, 28);
  ft_widget_line_edit_set_text(lineEdit, text);
  uiScreen->tab[index].lineEdit = lineEdit;

  FT_WidgetCheckbox *checkbox = checkbox_create(body, 0, 120, "Sensors");
  ft_widget_slots_connect(&checkbox->parent, NULL, NULL, checkbox_sensor_clicked, NULL, &uiScreen->tab[index]);
  uiScreen->tab[index].checkbox.sensor = checkbox;

  checkbox = checkbox_create(body, 1, 120, "Jetson");
  ft_widget_slots_connect(&checkbox->parent, NULL, NULL, checkbox_jetson_clicked, NULL, &uiScreen->tab[index]);
  uiScreen->tab[index].checkbox.jetson = checkbox;

  checkbox = checkbox_create(body, 2, 120, "PPM");
  ft_widget_slots_connect(&checkbox->parent, NULL, NULL, checkbox_ppm_clicked, NULL, &uiScreen->tab[index]);
  uiScreen->tab[index].checkbox.ppm = checkbox;

  checkbox = checkbox_create(body, 3, 120, "GNSS");
  ft_widget_slots_connect(&checkbox->parent, NULL, NULL, checkbox_gnss_clicked, NULL, &uiScreen->tab[index]);
  uiScreen->tab[index].checkbox.gnss = checkbox;

  uiScreen->tab[index].fileSize = label_create(body, 80, "Size:");
  uiScreen->tab[index].currentFilename = label_create(body, 105, "Filename:");

  FT_WidgetToggle *toggle = ft_widget_toggle_create(body);
  ft_widget_set_color(&toggle->parent, 255, 255, 255, bFalse);
  ft_widget_set_coord(&toggle->parent, 670, 30);
  ft_widget_set_dimensions(&toggle->parent, 60, 30);
  ft_widget_slots_connect(&toggle->parent, NULL, NULL, toggle_clicked, NULL, &uiScreen->tab[index]);
  ft_widget_toggle_set_font(toggle, 28);
  uiScreen->tab[index].toggle = toggle;

  return body;
}

static void tab_create(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;

  const size_t maxCount = sizeof(ui->screen.log.tab) / sizeof(ui->screen.log.tab[0]);
  for(uint8_t i = 0; i < maxCount; ++i) {
    if(!ui->screen.log.tab[i].body) {
      FT_Widget *tabBody = tab_make_and_config(&ui->screen.log, i);
      ui->screen.log.tab[i].body = tabBody;
      ui->screen.log.tab[i].categories = sdCategoryNone;
      ui->screen.log.tab[i].fileHandle = -1;
      ft_screen_draw(ui->core->screen);
      break;
    }
  }
}

static void window_create(FT_Widget *owner, AppUI *ui)
{
  AppUI_Log *uiScreen = &ui->screen.log;

  FT_WidgetTab *window = ft_widget_tab_create(owner);
  ft_widget_set_color(&window->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&window->parent, 20, 40);
  ft_widget_set_dimensions(&window->parent, 760, 400);
  ft_widget_slots_connect(&window->parent, NULL, NULL, tab_create, NULL, ui);
  ft_widget_tab_set_font(window, 28);
  ft_widget_tab_set_max_tabs(window, 3);
  uiScreen->window = window;

  uint8_t tabNo = 0;
  SD_Card const *sd = sd_instance();
  const uint8_t maxStreams = sizeof(sd->stream) / sizeof(sd->stream[0]);
  for(uint8_t i = 0; i < maxStreams; ++i) {
    if(sd->stream[i].inWork == bTrue) {
      uiScreen->tab[tabNo].log = uiScreen;
      uiScreen->tab[tabNo].body = tab_make_and_config(uiScreen, tabNo);
      uiScreen->tab[tabNo].categories = sd->stream[i].categories;
      uiScreen->tab[tabNo].fileHandle = i;

      char const *filename = sd_filename(sd, i);
      ft_widget_tab_set_tab_name(uiScreen->window, uiScreen->tab[tabNo].body, filename);
      ft_widget_line_edit_set_text(uiScreen->tab[tabNo].lineEdit, filename);

      Bool enabled = (sd->stream[i].categories & sdCategorySensors) == sdCategorySensors ? bTrue : bFalse;
      ft_widget_checkbox_set_enabled(uiScreen->tab[tabNo].checkbox.sensor, enabled);

      enabled = (sd->stream[i].categories & sdCategoryJetson) == sdCategoryJetson ? bTrue : bFalse;
      ft_widget_checkbox_set_enabled(uiScreen->tab[tabNo].checkbox.jetson, enabled);

      enabled = (sd->stream[i].categories & sdCategoryPPM) == sdCategoryPPM ? bTrue : bFalse;
      ft_widget_checkbox_set_enabled(uiScreen->tab[tabNo].checkbox.ppm, enabled);

      enabled = (sd->stream[i].categories & sdCategoryGNSS) == sdCategoryGNSS ? bTrue : bFalse;
      ft_widget_checkbox_set_enabled(uiScreen->tab[tabNo].checkbox.gnss, enabled);

      ft_widget_label_set_text(uiScreen->tab[tabNo].currentFilename, filename);
      char text[32];
      const size_t sizeLen = snprintf(text, sizeof(text), "Size: ");
      sd_file_size_in_text(sd_instance(), i, &text[sizeLen], sizeof(text)-sizeof("Size: ")+1);
      ft_widget_label_set_text(uiScreen->tab[tabNo].fileSize, text);

      ft_widget_toggle_set_state(uiScreen->tab[tabNo].toggle, bTrue);

      ++tabNo;
    }
  }

  const size_t maxCount = sizeof(ui->screen.log.tab)/sizeof(ui->screen.log.tab[0]);
    for(uint8_t i = tabNo; i < maxCount; ++i) {
      ui->screen.log.tab[i].log = &ui->screen.log;
      ui->screen.log.tab[i].body = NULL;
      ui->screen.log.tab[i].categories = sdCategoryNone;
      ui->screen.log.tab[i].fileHandle = -1;
    }
}

FT_Screen* app_ui_screen_log(FT_Core *core, AppUI *ui)
{
  AppUI_Log *uiScreen = &ui->screen.log;
  FT_Screen *screen = ft_screen_create(core);
  window_create(ft_screen_widget(screen), ui);
  uiScreen->time = 0;
  uiScreen->screen = screen;
  return screen;
}

Bool app_ui_screen_log_refresh(AppUI *ui)
{
  AppUI_Log *uiScreen = &ui->screen.log;
  Bool draw = bFalse;
  const uint32_t time = xTaskGetTickCount();
  if((time-uiScreen->time) > pdMS_TO_TICKS(1000)) {
    uiScreen->time = time;
    const uint8_t tabNumber = sizeof(uiScreen->tab) / sizeof(uiScreen->tab[0]);
    for(uint8_t i = 0; i < tabNumber; ++i) {
      const int8_t handle = uiScreen->tab[i].fileHandle;
      if(handle >= 0) {
        char text[64];
        const size_t sizeLen = snprintf(text, sizeof(text), "Size: ");
        sd_file_size_in_text(sd_instance(), handle, &text[sizeLen], sizeof(text)-sizeof("Size: ")+1);
        ft_widget_label_set_text(uiScreen->tab[i].fileSize, text);

        snprintf(text, sizeof(text), "Filename: %s", sd_filename(sd_instance(), handle));
        ft_widget_label_set_text(uiScreen->tab[i].currentFilename, text);
      }
    }
  }
  return draw;
}
