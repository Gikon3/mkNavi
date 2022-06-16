#include "ft_core.h"
#include "ft_screen.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"

void app_ui_manager_init(AppUI *ui, FT_Core *core)
{
  app_ui_screen_id_t start_screen_id = app_ui_screen_mainmenu;
  ui->refresh = app_ui_screen_mainmenu_refresh;
  ui->clear = NULL;

  ui->core = core;
  ui->qGestureContainer = xQueueCreate(4, sizeof(FT_GestureSendContainer));
  ui->navigation.index = 0;
  ui->navigation.screenStack = make_vect(4, sizeof(app_ui_screen_id_t));
  vect_push_back(&ui->navigation.screenStack, &start_screen_id);
  ui->qNextScreen = xQueueCreate(2, sizeof(app_ui_screen_id_t));

  FT_Screen *startScreen = start_screen_id(ui->core, ui);
  app_ui_statusbar_create(ui->core, startScreen, &ui->statusBar);
  ft_core_screen_connect(core, startScreen);
}

static void step_into_screen(AppUI *ui)
{
  app_ui_screen_id_t screen_id;
  vect_at(&ui->navigation.screenStack, ui->navigation.index, &screen_id);
  xQueueSend(ui->qNextScreen, &screen_id, portMAX_DELAY);
}

static Bool gesture_process(AppUI *ui)
{
  FT_GestureSendContainer gestureContainer;
  if(xQueueReceive(ui->qGestureContainer, &gestureContainer, 0) == pdFALSE)
    return bFalse;

  switch(gestureContainer.gesture) {
    case ftGestureLeft:
      if(ui->navigation.index > 0) {
        --ui->navigation.index;
        step_into_screen(ui);
      }
      return bTrue;
    case ftSGestureRight:
      if((ui->navigation.index+1) < vect_size(&ui->navigation.screenStack)) {
        ++ui->navigation.index;
        step_into_screen(ui);
      }
      return bTrue;
    case ftGestureTop:
      break;
    case ftGestureTopRight:
      break;
    case ftGestureBottom: {
      const size_t numberRemoveItems = vect_size(&ui->navigation.screenStack) - 1;
      for(uint8_t i = 0; i < numberRemoveItems; ++i)
        vect_pop_back(&ui->navigation.screenStack, NULL);
      ui->navigation.index = 0;
      step_into_screen(ui);
      return bTrue;
    }
    case ftGestureSwipe:
    case ftGesturePinch:
      break;
    default: ;
  }
  return bFalse;
}

static void navigation_solve(AppUI *ui, app_ui_screen_id_t screen_id)
{
  if((ui->navigation.index+1) < vect_size(&ui->navigation.screenStack)) {
    app_ui_screen_id_t screen_in_stack;
    vect_at(&ui->navigation.screenStack, ui->navigation.index+1, &screen_in_stack);
    if(screen_id != screen_in_stack) {
      for(uint8_t i = 0; i < (vect_size(&ui->navigation.screenStack)-1-ui->navigation.index); ++i)
        vect_pop_back(&ui->navigation.screenStack, NULL);
      vect_push_back(&ui->navigation.screenStack, &screen_id);
    }
  }
  else
    vect_push_back(&ui->navigation.screenStack, &screen_id);
  ++ui->navigation.index;
}

static void refresh_and_clear_select(app_ui_screen_id_t screen_id, app_ui_refresh_t *refresh, app_ui_clear_t *clear)
{
  *refresh = NULL;
  *clear = NULL;

  if(screen_id == app_ui_screen_mainmenu) {
    *refresh = app_ui_screen_mainmenu_refresh;
  }
  else if(screen_id == app_ui_screen_lsn) {
    *refresh = app_ui_screen_lsn_refresh;
  }
  else if(screen_id == app_ui_screen_gnss) {
    *refresh = app_ui_screen_gnss_refresh;
  }
  else if(screen_id == app_ui_screen_log) {
    *refresh = app_ui_screen_log_refresh;
  }
  else if(screen_id == app_ui_screen_terminal) {
    *refresh = app_ui_screen_terminal_refresh;
    *clear = app_ui_screen_terminal_clear;
  }
  else if(screen_id == app_ui_screen_bluetooth) {
    *refresh = app_ui_screen_bluetooth_refresh;
  }
  else if(screen_id == app_ui_screen_debug) {
    *refresh = app_ui_screen_debug_refresh;
  }
  else if(screen_id == app_ui_screen_debug_touch) {
    *refresh = app_ui_screen_debug_touch_refresh;
  }
  else if(screen_id == app_ui_screen_debug_gesture) {
    *refresh = app_ui_screen_debug_gesture_refresh;
  }
  else if(screen_id == app_ui_screen_sensors) {
    *refresh = app_ui_screen_sensors_refresh;
  }
  else if(screen_id == app_ui_screen_jetson) {
    *refresh = app_ui_screen_jetson_refresh;
  }
  else if(screen_id == app_ui_screen_devices) {
    *refresh = app_ui_screen_devices_refresh;
  }
}

static Bool replace_screen(AppUI *ui)
{
  const Bool gestureWorked = gesture_process(ui);

  app_ui_screen_id_t screen_id = NULL;
  if(xQueueReceive(ui->qNextScreen, &screen_id, 0) == pdFALSE)
    return bFalse;
  xQueueReset(ui->qGestureContainer);

  if(gestureWorked == bFalse)
    navigation_solve(ui, screen_id);

  if(ui->clear)
    ui->clear(ui);

  refresh_and_clear_select(screen_id, &ui->refresh, &ui->clear);
  FT_Screen *nextScreen = screen_id(ui->core, ui);

  app_ui_statusbar_create(ui->core, nextScreen, &ui->statusBar);
  FT_Screen *screen = ui->core->screen;
  ft_core_screen_connect(ui->core, nextScreen);
  ft_screen_destroy(screen);
  if(screen_id != app_ui_screen_debug_gesture) {
    const uint8_t gestures = ftGestureLeft | ftSGestureRight | ftGestureBottom;
    ft_touch_gesture_queue_connect(ui->core, ui->qGestureContainer, gestures);
  }
  return bTrue;
}

void app_ui_manager(AppUI *ui)
{
  const Bool replaceStat = replace_screen(ui);

  xSemaphoreTakeRecursive(ui->core->screen->mut, portMAX_DELAY);
  const Bool statusbarStat = app_ui_statusbar_refresh(&ui->statusBar);
  Bool screenStat = bFalse;
  if(ui->refresh)
    screenStat = ui->refresh(ui);
  if(replaceStat == bTrue || statusbarStat == bTrue || screenStat == bTrue)
    ft_screen_draw(ui->core->screen);
  xSemaphoreGiveRecursive(ui->core->screen->mut);
}

void app_ui_manager_next_screen(AppUI *ui, app_ui_screen_id_t screen_id)
{
  xQueueSend(ui->qNextScreen, &screen_id, portMAX_DELAY);
}
