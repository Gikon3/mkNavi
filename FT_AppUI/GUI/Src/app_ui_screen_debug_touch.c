#include <stdio.h>
#include "ft_screen.h"
#include "ft_widgets.h"
#include "app_ui_screens.h"

static char const *touchFieldNames[] = {
    "state",
    "time",
    "tag press",
    "tag",
    "x",
    "y"
};

static char const *trackFieldNames[] = {
    "state",
    "tag capture",
    "tag",
    "old value",
    "value"
};

static FT_WidgetLabel* label_create(FT_Widget *owner, char const *text, int16_t x, int16_t y)
{
  FT_WidgetLabel *label = ft_widget_label_create(owner);
  ft_widget_label_set_text(label, text);
  ft_widget_set_coord(&label->parent, x, y);
  return label;
}

static void field_names_create(FT_Widget *owner, AppUI_DebugTouch *widgets)
{
  const int16_t fieldsDy = 16;
  for(uint8_t i = 0; i < sizeof(widgets->touch[0])/sizeof(widgets->touch[0].abstractField[0]); ++i)
    label_create(owner, touchFieldNames[i], 0, fieldsDy*i);

  for(uint8_t i = 0; i < sizeof(widgets->track[0])/sizeof(widgets->track[0].abstractField[0]); ++i)
    label_create(owner, trackFieldNames[i], 430, fieldsDy*i);
}

static char const* touch_state_name(FT_TouchState state)
{
  switch(state) {
    case ftTouchNone: return "none";
    case ftTouchPress: return "press";
    case ftTouchLongPress: return "long press";
  }
  return NULL;
}

static void touch_field_values_create(FT_Widget *owner, AppUI_DebugTouch *widgets)
{
  const int16_t dx = 70;
  const int16_t dy = 16;
  FT_TouchScreen const *touchScreen = &widgets->core->screen->touchScreen;
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", touch_state_name(touchScreen->touches[i].state));
    widgets->touch[i].state = label_create(owner, text, dx*i, dy*0);

    snprintf(text, sizeof(text), "%lu", touchScreen->touches[i].time);
    widgets->touch[i].time = label_create(owner, text, dx*i, dy*1);

    snprintf(text, sizeof(text), "%03u", touchScreen->touches[i].tagPress);
    widgets->touch[i].tagPress = label_create(owner, text, dx*i, dy*2);

    snprintf(text, sizeof(text), "%03u", touchScreen->touches[i].tag);
    widgets->touch[i].tag = label_create(owner, text, dx*i, dy*3);

    snprintf(text, sizeof(text), "%06d", touchScreen->touches[i].x);
    widgets->touch[i].x = label_create(owner, text, dx*i, dy*4);

    snprintf(text, sizeof(text), "%06d", touchScreen->touches[i].y);
    widgets->touch[i].y = label_create(owner, text, dx*i, dy*5);
  }
}

static char const* track_state_name(FT_TrackState state)
{
  switch(state) {
    case ftTrackNone: return "none";
    case ftTrackCapture: return "capture";
  }
  return NULL;
}

static void track_field_values_create(FT_Widget *owner, AppUI_DebugTouch *widgets)
{
  const int16_t dx = 60;
  const int16_t dy = 16;
  FT_TouchScreen const *touchScreen = &widgets->core->screen->touchScreen;
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", track_state_name(touchScreen->tracks[i].state));
    widgets->track[i].state = label_create(owner, text, dx*i, dy*0);

    snprintf(text, sizeof(text), "%03u", touchScreen->tracks[i].tagCapture);
    widgets->track[i].tagCapture = label_create(owner, text, dx*i, dy*1);

    snprintf(text, sizeof(text), "%03u", touchScreen->tracks[i].tag);
    widgets->track[i].tag = label_create(owner, text, dx*i, dy*2);

    snprintf(text, sizeof(text), "%05u", touchScreen->tracks[i].oldVal);
    widgets->track[i].oldVal = label_create(owner, text, dx*i, dy*3);

    snprintf(text, sizeof(text), "%05u", touchScreen->tracks[i].val);
    widgets->track[i].value = label_create(owner, text, dx*i, dy*4);
  }
}

static void touch_field_values_set(AppUI_DebugTouch *widgets, FT_TouchScreen const *touchScreen)
{
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", touch_state_name(touchScreen->touches[i].state));
    ft_widget_label_set_text(widgets->touch[i].state, text);

    snprintf(text, sizeof(text), "%lu", touchScreen->touches[i].time);
    ft_widget_label_set_text(widgets->touch[i].time, text);

    snprintf(text, sizeof(text), "%03u", touchScreen->touches[i].tagPress);
    ft_widget_label_set_text(widgets->touch[i].tagPress, text);

    snprintf(text, sizeof(text), "%03u", touchScreen->touches[i].tag);
    ft_widget_label_set_text(widgets->touch[i].tag, text);

    snprintf(text, sizeof(text), "%06d", touchScreen->touches[i].x);
    ft_widget_label_set_text(widgets->touch[i].x, text);

    snprintf(text, sizeof(text), "%06d", touchScreen->touches[i].y);
    ft_widget_label_set_text(widgets->touch[i].y, text);
  }
}

static void track_field_values_set(AppUI_DebugTouch *widgets, FT_TouchScreen const *touchScreen)
{
  char text[16];
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    snprintf(text, sizeof(text), "%s", track_state_name(touchScreen->tracks[i].state));
    ft_widget_label_set_text(widgets->track[i].state, text);

    snprintf(text, sizeof(text), "%03u", touchScreen->tracks[i].tagCapture);
    ft_widget_label_set_text(widgets->track[i].tagCapture, text);

    snprintf(text, sizeof(text), "%03u", touchScreen->tracks[i].tag);
    ft_widget_label_set_text(widgets->track[i].tag, text);

    snprintf(text, sizeof(text), "%05u", touchScreen->tracks[i].oldVal);
    ft_widget_label_set_text(widgets->track[i].oldVal, text);

    snprintf(text, sizeof(text), "%05u", touchScreen->tracks[i].val);
    ft_widget_label_set_text(widgets->track[i].value, text);
  }
}

FT_Screen* app_ui_screen_debug_touch(FT_Core *core, AppUI *ui)
{
  AppUI_DebugTouch *uiScreen = &ui->screen.debugTouch;
  uiScreen->core = core;
  FT_Screen *screen = ft_screen_create(core);

  FT_Widget *mainWidget = ft_widget_create(ft_screen_widget(screen), 0);
  ft_widget_set_y(mainWidget, APP_UI_STATUSBAR_HEIGHT+10);
  ft_widget_set_heap_en(mainWidget, bFalse);

  FT_Widget *fieldNames = ft_widget_create(mainWidget, 0);
  ft_widget_set_heap_en(fieldNames, bFalse);
  field_names_create(fieldNames, uiScreen);

  FT_Widget *touchFields = ft_widget_create(mainWidget, 0);
  ft_widget_set_x(touchFields, 70);
  touch_field_values_create(touchFields, uiScreen);
  ft_widget_set_heap_en_recursive(touchFields, bFalse);

  FT_Widget *trackFields = ft_widget_create(mainWidget, 0);
  ft_widget_set_coord(trackFields, 510, 10);
  track_field_values_create(trackFields, uiScreen);
  ft_widget_set_heap_en_recursive(trackFields, bFalse);

  FT_Widget *areas = ft_widget_create(mainWidget, 0);
  ft_widget_set_heap_en(areas, bFalse);
  ft_widget_set_coord(areas, 20, 180);
  const int16_t touchAreaDx = 50;
  const int16_t touchAreaW = 112;
  const int16_t touchAreaH = 50;
  for(uint8_t i = 0; i < FT_NUMBER_OF_TOUCHES; ++i) {
    const int16_t x = (touchAreaW + touchAreaDx) * i;
    FT_WidgetRectangle *touchArea = ft_widget_rectangle_create(areas);
    ft_widget_set_color(&touchArea->parent, 24, 48, 127, bFalse);
    ft_widget_set_coord(&touchArea->parent, x, 0);
    ft_widget_set_dimensions(&touchArea->parent, touchAreaW, touchAreaH);
    ft_widget_set_touch_type(&touchArea->parent, ftWidgetTouchUsual);
  }

  const int16_t trackAreaY0 = touchAreaH + 50;
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

Bool app_ui_screen_debug_touch_refresh(AppUI *ui)
{
  AppUI_DebugTouch *widgets = &ui->screen.debugTouch;
  FT_TouchScreen const *touchScreen = &widgets->core->screen->touchScreen;
  touch_field_values_set(widgets, touchScreen);
  track_field_values_set(widgets, touchScreen);

  return bTrue;
}
