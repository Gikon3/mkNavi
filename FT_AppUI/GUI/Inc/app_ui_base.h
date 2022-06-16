#ifndef INC_APP_UI_BASE_H_
#define INC_APP_UI_BASE_H_

#include "util_vector.h"
#include "ft_core_impl.h"
#include "ft_screen_impl.h"
#include "ft_widgets.h"
#include "app_ui_widget_battery.h"
#include "bluetooth.h"

#define APP_UI_STATUSBAR_HEIGHT 25

typedef struct
{
  uint32_t              ticks;
  FT_WidgetImage        *ppmIcon;
  struct
  {
    FT_WidgetLabel      *widget;
    uint8_t             sec;
    uint8_t             min;
    uint8_t             hour;
  } time;
  struct
  {
    AppUI_WidgetBattery *widget;
    uint8_t             val;
    Bool                charge;
  } battery;
  FT_WidgetLabel        *freeHeapSize;
} AppUI_StatusBar;

typedef struct
{
  uint32_t          time;
  struct
  {
    FT_WidgetLabel  *lat;
    FT_WidgetLabel  *lng;
    FT_WidgetLabel  *alt;
  } lsn;
  struct
  {
    FT_WidgetLabel  *lat;
    FT_WidgetLabel  *lng;
    FT_WidgetLabel  *alt;
  } gnss;
} AppUI_MainMenu;

typedef struct
{
  uint32_t            time;
  FT_WidgetToggle     *antenna;
  FT_WidgetParagraph  *bs[3];
} AppUI_LSN;

typedef struct
{
  uint32_t            time;
  FT_WidgetParagraph  *coord;
  FT_WidgetParagraph  *param;
  FT_WidgetToggle     *rtkSwitch;
  FT_WidgetToggle     *pppSwitch;
} AppUI_GNSS;

struct _AppUI_Log;
typedef struct
{
  struct _AppUI_Log *log;
  FT_Widget         *body;
  FT_WidgetLineEdit *lineEdit;
  struct
  {
    FT_WidgetCheckbox *sensor;
    FT_WidgetCheckbox *jetson;
    FT_WidgetCheckbox *ppm;
    FT_WidgetCheckbox *gnss;
  } checkbox;
  FT_WidgetLabel    *fileSize;
  FT_WidgetLabel    *currentFilename;
  FT_WidgetToggle   *toggle;
  uint8_t           categories;
  int8_t            fileHandle;
} AppUI_LogTab;

typedef struct _AppUI_Log
{
  uint32_t      time;
  FT_Screen     *screen;
  FT_WidgetTab  *window;
  AppUI_LogTab  tab[3];
} AppUI_Log;

struct _AppUI_Terminal;
typedef struct
{
  struct _AppUI_Terminal  *terminal;
  FT_WidgetText           *text;
} AppUI_TerminalTab;

typedef struct _AppUI_Terminal
{
  FT_WidgetTab        *window;
  union
  {
    struct
    {
      AppUI_TerminalTab ppm;
      AppUI_TerminalTab gnss;
    };
    AppUI_TerminalTab   field[2];
  } tab;
} AppUI_Terminal;

typedef struct
{
  uint32_t        time;
  BluetoothState  state;
  FT_Widget       *ownerWidget;
  FT_Widget       *mainWidget;
  FT_WidgetText   *devices;
} AppUI_Bluetooth;

typedef struct
{
  FT_Core const               *core;
  union
  {
    struct
    {
      FT_WidgetLabel          *state;
      FT_WidgetLabel          *time;
      FT_WidgetLabel          *tagPress;
      FT_WidgetLabel          *tag;
      FT_WidgetLabel          *x;
      FT_WidgetLabel          *y;
    };
    FT_WidgetLabel            *abstractField[6];
  } touch[FT_NUMBER_OF_TOUCHES];
  union
  {
    struct
    {
      FT_WidgetLabel          *state;
      FT_WidgetLabel          *tagCapture;
      FT_WidgetLabel          *tag;
      FT_WidgetLabel          *oldVal;
      FT_WidgetLabel          *value;
    };
    FT_WidgetLabel            *abstractField[5];
  } track[FT_NUMBER_OF_TOUCHES];
} AppUI_DebugTouch;

typedef struct
{
  FT_Core const                   *core;
  union
  {
    struct
    {
      FT_WidgetLabel              *lock;
      FT_WidgetLabel              *state;
      FT_WidgetLabel              *ready;
      FT_WidgetLabel              *x;
      FT_WidgetLabel              *y;
      FT_WidgetLabel              *val;
    };
    FT_WidgetLabel                *abstractField[6];
  } values[FT_NUMBER_OF_TOUCHES];
} AppUI_DebugGesture;

typedef struct
{
  union
  {
    struct
    {
      struct _AppUI_Sensors3D
      {
        FT_WidgetLabel        *x;
        FT_WidgetLabel        *y;
        FT_WidgetLabel        *z;
      } acc;
      struct _AppUI_Sensors3D gyr;
      struct _AppUI_Sensors3D mag;
      FT_WidgetLabel          *bar;
      struct
      {
        FT_WidgetLabel        *gyr;
        FT_WidgetLabel        *mag;
        FT_WidgetLabel        *bar;
      } temperature;
      struct
      {
        struct
        {
          FT_WidgetLabel      *charge;
          FT_WidgetLabel      *powerGood;
          FT_WidgetLabel      *vbusDetect;
          FT_WidgetLabel      *ico;
        } status;
        struct
        {
          FT_WidgetLabel      *ibus;
          FT_WidgetLabel      *ichrg;
          FT_WidgetLabel      *vbus;
          FT_WidgetLabel      *vbut;
          FT_WidgetLabel      *vsys;
          FT_WidgetLabel      *ts;
          FT_WidgetLabel      *tdie;
        } adc;
      } battery;
    };
    FT_WidgetLabel            *field[24];
  };
} AppUI_Sensors;

typedef struct
{
  uint32_t            time;
  FT_WidgetParagraph  *solution;
  FT_WidgetParagraph  *bluetooth;
  FT_WidgetParagraph  *wifi;
  FT_WidgetParagraph  *gps;
} AppUI_Jetson;

typedef struct
{
  uint32_t      time;
  FT_WidgetText *states;
} AppUI_Devices;

struct _AppUI;
typedef FT_Screen* (*app_ui_screen_id_t)(FT_Core*, struct _AppUI*);
typedef Bool (*app_ui_refresh_t)(struct _AppUI*);
typedef void (*app_ui_clear_t)(struct _AppUI*);

typedef struct _AppUI
{
  FT_Core               *core;
  QueueHandle_t         qGestureContainer;
  QueueHandle_t         qNextScreen;
  app_ui_refresh_t      refresh;
  app_ui_clear_t        clear;
  struct
  {
    uint8_t             index;
    Vector              screenStack;
  } navigation;
  AppUI_StatusBar       statusBar;
  union
  {
    AppUI_MainMenu      mainMenu;
    AppUI_LSN           lsn;
    AppUI_GNSS          gnss;
    AppUI_Terminal      terminal;
    AppUI_Bluetooth     bluetooth;
    AppUI_Log           log;
    AppUI_DebugTouch    debugTouch;
    AppUI_DebugGesture  debugGesture;
    AppUI_Sensors       sensors;
    AppUI_Jetson        jetson;
    AppUI_Devices       devices;
  } screen;
} AppUI;

#endif /* INC_APP_UI_BASE_H_ */
