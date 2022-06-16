#ifndef FRAMEWORK_INC_FT_WIDGET_IMPL_H_
#define FRAMEWORK_INC_FT_WIDGET_IMPL_H_

#include "stm32f4xx.h"
#include "util_types.h"
#include "util_vector.h"
#include "util_dlist.h"
#include "ft_utils_impl.h"
#include "ft_heap_impl.h"

#define FT_WIDGET_HEADER \
  union \
  { \
    struct \
    { \
      FT_Widget *owner; \
      FT_Screen *screen; \
    }; \
    FT_Widget   parent; \
  };

struct _FT_Screen;
struct _FT_Widget;

typedef union
{
  struct
  {
    int16_t x;
    int16_t y;
  };
  uint16_t  val;
} FT_WidgetSlotParam;

typedef enum {
  ftWidgetSlotInternal,
  ftWidgetSlotUser
} FT_WidgetSlotType;

typedef void (*FT_WidgetSlot)(struct _FT_Widget*, FT_WidgetSlotParam*);
typedef void (*FT_WidgetUserSlot)(struct _FT_Widget const*, FT_WidgetSlotParam const*, void*);

typedef struct
{
  FT_WidgetSlot press;
  FT_WidgetSlot long_press;
  FT_WidgetSlot release;
  FT_WidgetSlot reset;
} FT_Slots;

typedef struct
{
  FT_WidgetUserSlot press;
  FT_WidgetUserSlot long_press;
  FT_WidgetUserSlot release;
  FT_WidgetUserSlot reset;
  void              *arg;
} FT_UserSlots;

typedef enum {
  ftWidgetTouchNone,
  ftWidgetTouchUsual,
  ftWidgetTouchTrack
} FT_WidgetTouchType;

typedef enum {
  ftContextNotCompile,
  ftContextHeapLoad
} FT_ContextState;

typedef struct _FT_Widget
{
  struct _FT_Widget     *owner;
  struct _FT_Screen     *screen;
  DList                 widgets;
  struct
  {
    struct
    {
      FT_Slots          common;
      FT_UserSlots      user;
    } slot;
    FT_WidgetTouchType  type;
    uint16_t            tag;
  } touch;
  struct
  {
    Bool                enable;
    FT_HeapParam        property;
  } heap;
  FT_ContextState       state;
  Bool                  visible;
  Bool                  defaultColor;
  Bool                  defaultAlpha;
  FT_Color              color;
  FT_Coord              coord;
  FT_Coord              absoluteCoord;
  FT_Dimensions         dimensions;
  struct
  {
    void (*compile)(struct _FT_Widget*, Vector*);
    void (*clear)(struct _FT_Widget*);
  } action;
} FT_Widget;

typedef struct
{
  FT_WidgetSlotType   type;
  union
  {
    FT_WidgetSlot     slot;
    FT_WidgetUserSlot userSlot;
  };
  FT_Widget           *widget;
  FT_WidgetSlotParam  param;
  void                *arg;
} FT_SlotItem;

#endif /* FRAMEWORK_INC_FT_WIDGET_IMPL_H_ */
