#ifndef FRAMEWORK_INC_FT_KEYBOARD_IMPL_H_
#define FRAMEWORK_INC_FT_KEYBOARD_IMPL_H_

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "util_types.h"
#include "ft_widget_impl.h"

struct _FT_Screen;
struct _FT_WidgetLineEdit;

typedef enum
{
  ftKeyboardLayoutMain,
  ftKeyboardLayoutNumeric,
  ftKeyboardLayoutSymbolic
} FT_KeyboardLayout;

typedef enum
{
  ftKeyboardCaseLower,
  ftKeyboardCaseUpper,
  ftKeyboardCaseAlwaysUpper
} FT_KeyboardCase;

typedef struct
{
  struct _FT_Screen         *screen;
  struct _FT_WidgetLineEdit *capture;
  struct
  {
    TimerHandle_t           repeatBackspace;
    TimerHandle_t           cursorBlinking;
  } timers;
  struct
  {
    Bool                    enable;
    FT_HeapParam            property;
  } heap;
  FT_ContextState           state;
  Bool                      visible;
  Bool                      defaultColor;
  Bool                      defaultAlpha;
  FT_Color                  color;
  FT_Coord                  coord;
  FT_Dimensions             dimensions;
  int16_t                   font;
  FT_KeyboardLayout         layout;
  char                      pressKey;
  char                      nextChar;
  struct
  {
    FT_KeyboardCase         pending;
    FT_KeyboardCase         confirmed;
  } caseManagement;
} FT_Keyboard;

#endif /* FRAMEWORK_INC_FT_KEYBOARD_IMPL_H_ */
