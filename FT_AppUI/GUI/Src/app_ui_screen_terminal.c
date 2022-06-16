#include <stdio.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "app_ui_screens.h"
#include "app_ui_manager.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "flow_ctrl.h"

static void tab_make_and_config(AppUI_Terminal *uiScreen, AppUI_TerminalTab *uiTab)
{
  char const *name = NULL;
  if(uiTab == &uiScreen->tab.ppm)
    name = "PPM";
  else if(uiTab == &uiScreen->tab.gnss)
    name = "GNSS";

  FT_Widget *body = ft_widget_tab_new_tab(uiScreen->window, name);
  if(!body)
    return;
  ft_widget_set_heap_en(body, bFalse);

  FT_WidgetText *text = ft_widget_text_create(body);
  ft_widget_set_heap_en(&text->parent, bFalse);
  ft_widget_text_set_font(text, 28);
  ft_widget_text_set_max_lines(text, 40);

  uiTab->terminal = uiScreen;
  uiTab->text = text;
}

FT_Screen* app_ui_screen_terminal(FT_Core *core, AppUI *ui)
{
  AppUI_Terminal *uiScreen = &ui->screen.terminal;
  FT_Screen *screen = ft_screen_create(core);

  FT_WidgetTab *window = ft_widget_tab_create(ft_screen_widget(screen));
  ft_widget_set_color(&window->parent, 255, 128, 0, bFalse);
  ft_widget_set_coord(&window->parent, 20, 40);
  ft_widget_set_dimensions(&window->parent, 760, 400);
  ft_widget_tab_set_font(window, 28);
  ft_widget_tab_set_closeable_tabs(window, bFalse);
  ft_widget_tab_set_max_tabs(window, 2);
  uiScreen->window = window;

  tab_make_and_config(uiScreen, &uiScreen->tab.ppm);
  tab_make_and_config(uiScreen, &uiScreen->tab.gnss);

  flowCtrl.ripe.screen.gnss.queue = xQueueCreate(16, sizeof(String));
  flowCtrl.ripe.screen.gnss.en = bTrue;

  flowCtrl.ripe.screen.ppm.queue = xQueueCreate(16, sizeof(String));
  flowCtrl.ripe.screen.ppm.en = bTrue;

  return screen;
}

Bool app_ui_screen_terminal_refresh(AppUI *ui)
{
  AppUI_Terminal *uiScreen = &ui->screen.terminal;
  Bool draw = bFalse;

  String str;
  while(xQueueReceive(flowCtrl.ripe.screen.gnss.queue, &str, 0) == pdTRUE) {
    ft_widget_text_append_text(uiScreen->tab.gnss.text, str_cdata(&str), 0xFF);
    str_clear(&str);
    draw = bTrue;
  }

  while(xQueueReceive(flowCtrl.ripe.screen.ppm.queue, &str, 0) == pdTRUE) {
    ft_widget_text_append_text(uiScreen->tab.ppm.text, str_cdata(&str), 0xFF);
    str_clear(&str);
    draw = bTrue;
  }
  return draw;
}

void app_ui_screen_terminal_clear(AppUI *ui)
{
  flowCtrl.ripe.screen.gnss.en = bFalse;
  vQueueDelete(flowCtrl.ripe.screen.gnss.queue);
  flowCtrl.ripe.screen.gnss.queue = NULL;

  flowCtrl.ripe.screen.ppm.en = bFalse;
  vQueueDelete(flowCtrl.ripe.screen.ppm.queue);
  flowCtrl.ripe.screen.ppm.queue = NULL;
}
