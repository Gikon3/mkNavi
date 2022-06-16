#include <stdio.h>
#include "ft_touch_impl.h"
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_manager.h"
#include "app_ui_screens.h"

static char const *fieldNames[] = {
    "lock",
    "state",
    "ready",
    "x",
    "y",
    "value"
};

static void back_clicked(FT_Widget const *widget, FT_WidgetSlotParam const *coords, void *arg)
{
  AppUI *ui = arg;
  app_ui_manager_next_screen(ui, app_ui_screen_debug);
}

static FT_WidgetLabel* label_create(FT_Widget *owner, char const *text, int16_t x, int16_t y)
{
  FT_WidgetLabel *label = ft_widget_label_create(owner);
  ft_widget_label_set_text(label, text);
  ft_widget_set_coord(&label->parent, x, y);
  return label;
}

static void field_names_create(FT_Widget *owner, AppUI_DebugGesture *uiScreen)
{
  const int16_t fieldsDy = 16;
  for(uint8_t i = 0; i < sizeof(uiScreen->values[0])/sizeof(uiScreen->values[0].abstractField[0]); ++i)
    label_create(owner, fieldNames[i], 0, fieldsDy*i);
}

static char const* state_name(FT_GestureState state)
{
  switch(state) {
    case ftGestureUnknown: return "none";
    case ftGestureLeft: return "left";
    case ftSGestureRight: return "right";
    case ftGestureTop: return "top";
    case ftGestureTopRight: return "topRight";
    case ftGestureBottom: return "bottom";
    case ftGestureSwipe: return "swipe";
    case ftGesturePinch: return "pinch";
    case ftGestureLock: return "lock";
  }
  return NULL;
}

static void field_values_create(FT_Widget *owner, AppUI_DebugGesture *uiScreen)
{
  const int16_t fieldsDx = 90;
  const int16_t fieldsDy = 16;
  FT_Gesture const *gesture = &uiScreen->core->screen->touchScreen.gesture;
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", gesture->touch[i].lock == bFalse ? "unlock" : "lock");
    uiScreen->values[i].lock = label_create(owner, text, fieldsDx*i, fieldsDy*0);

    snprintf(text, sizeof(text), "%s", state_name(gesture->touch[i].state));
    uiScreen->values[i].state = label_create(owner, text, fieldsDx*i, fieldsDy*1);

    snprintf(text, sizeof(text), "%s", state_name(gesture->touch[i].ready));
    uiScreen->values[i].ready = label_create(owner, text, fieldsDx*i, fieldsDy*2);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].x);
    uiScreen->values[i].x = label_create(owner, text, fieldsDx*i, fieldsDy*3);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].y);
    uiScreen->values[i].y = label_create(owner, text, fieldsDx*i, fieldsDy*4);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].val);
    uiScreen->values[i].val = label_create(owner, text, fieldsDx*i, fieldsDy*5);
  }
}

static void field_values_set(AppUI_DebugGesture *uiScreen, FT_Gesture const *gesture)
{
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", gesture->touch[i].lock == bFalse ? "unlock" : "lock");
    ft_widget_label_set_text(uiScreen->values[i].lock, text);

    snprintf(text, sizeof(text), "%s", state_name(gesture->touch[i].state));
    ft_widget_label_set_text(uiScreen->values[i].state, text);

    snprintf(text, sizeof(text), "%s", state_name(gesture->touch[i].ready));
    ft_widget_label_set_text(uiScreen->values[i].ready, text);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].x);
    ft_widget_label_set_text(uiScreen->values[i].x, text);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].y);
    ft_widget_label_set_text(uiScreen->values[i].y, text);

    snprintf(text, sizeof(text), "%06d", gesture->touch[i].val);
    ft_widget_label_set_text(uiScreen->values[i].val, text);
  }
}

FT_Screen *app_ui_screen_debug_gesture(FT_Core *core, AppUI *ui)
{
  AppUI_DebugGesture *uiScreen = &ui->screen.debugGesture;
  uiScreen->core = core;
  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *mainWindow = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_coord(mainWindow, 20, APP_UI_STATUSBAR_HEIGHT+20);
  ft_widget_set_heap_en(mainWindow, bFalse);
  ft_widget_set_w(mainWindow, 760);

  FT_WidgetButton *btnBack = ft_widget_button_create(mainWindow);
  ft_widget_set_dimensions(&btnBack->parent, 80, 50);
  ft_widget_slots_connect(&btnBack->parent, NULL, NULL, back_clicked, NULL, ui);
  ft_widget_button_set_font(btnBack, 28);
  ft_widget_button_set_text(btnBack, "back");

  FT_Widget *fieldNames = ft_widget_create(mainWindow, 0);
  ft_widget_set_y(fieldNames, 70);
  ft_widget_set_heap_en(fieldNames, bFalse);
  field_names_create(fieldNames, uiScreen);

  FT_Widget *touchFields = ft_widget_create(mainWindow, 0);
  ft_widget_set_coord(touchFields, 70, 70);
  field_values_create(touchFields, uiScreen);
  ft_widget_set_heap_en_recursive(touchFields, bFalse);

  FT_Widget *areas = ft_widget_create(mainWindow, 0);
  ft_widget_set_y(areas, 180);
  ft_widget_set_heap_en(touchFields, bFalse);
  const int16_t touchAreaDx = 80;
  const int16_t touchAreaW = 88;
  const int16_t touchAreaH = 50;
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    const int16_t x = (touchAreaW + touchAreaDx) * i;
    FT_WidgetRectangle *touchArea = ft_widget_rectangle_create(areas);
    ft_widget_set_color(&touchArea->parent, 24, 48, 127, bFalse);
    ft_widget_set_coord(&touchArea->parent, x, 0);
    ft_widget_set_dimensions(&touchArea->parent, touchAreaW, touchAreaH);
    ft_widget_set_touch_type(&touchArea->parent, ftWidgetTouchUsual);
  }

  const int16_t trackAreaY0 = touchAreaH + 20;
  const int16_t trackAreaDx = touchAreaDx;
  const int16_t trackAreaW = touchAreaW;
  const int16_t trackAreaH = touchAreaH;
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    const int16_t x = (trackAreaW + trackAreaDx) * i;
    FT_WidgetRectangle *trackArea = ft_widget_rectangle_create(areas);
    ft_widget_set_color(&trackArea->parent, 125, 125, 20, bFalse);
    ft_widget_set_coord(&trackArea->parent, x, trackAreaY0);
    ft_widget_set_dimensions(&trackArea->parent, trackAreaW, trackAreaH);
    ft_widget_set_touch_type(&trackArea->parent, ftWidgetTouchTrack);
  }

  return screen;
}

Bool app_ui_screen_debug_gesture_refresh(AppUI *ui)
{
  AppUI_DebugGesture *uiScreen = &ui->screen.debugGesture;
  FT_Gesture const *gesture = &uiScreen->core->screen->touchScreen.gesture;
  field_values_set(uiScreen, gesture);

  return bTrue;
}
