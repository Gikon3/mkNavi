#include <string.h>
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_heap.h"
#include "ft_screen.h"
#include "ft_widget.h"

FT_Widget *ft_widget_create(FT_Widget *owner, size_t size)
{
  if(size < sizeof(FT_Widget))
    size = sizeof(FT_Widget);
  FT_Widget *widget = ft_util_malloc(size);
  ft_widget_set_dependent_param(widget, owner);
  if(owner)
    dlist_push_back(&owner->widgets, &widget);
  widget->widgets = make_dlist(sizeof(FT_Widget*));
  widget->visible = bTrue;
  memset(&widget->touch.slot, 0, sizeof(widget->touch.slot));
  widget->touch.type = ftWidgetTouchNone;
  widget->touch.tag = 0;
  widget->heap.enable = bTrue;
  widget->heap.property.addr = 0;
  widget->heap.property.size = 0;
  widget->state = ftContextNotCompile;
  widget->defaultColor = bTrue;
  widget->defaultAlpha = bTrue;
  widget->color.value = 0xFF;
  widget->coord.x = 0;
  widget->coord.y = 0;
  widget->absoluteCoord.x = 0;
  widget->absoluteCoord.y = 0;
  widget->action.compile = NULL;
  widget->action.clear = NULL;
  if(owner)
    ft_widget_calculate_absolute_coord_recursive(widget);
  return widget;
}

void ft_widget_destroy(FT_Widget *widget)
{
  ft_widget_semphr_take(widget);

  while(dlist_size(&widget->widgets) > 0) {
    FT_Widget *nested;
    dlist_pop_front(&widget->widgets, &nested);
    ft_widget_destroy(nested);
  }

  ft_screen_clear_tag(widget->screen, widget->touch.tag);
  if(widget->heap.property.addr > 0)
    ft_heap_remove(&widget->screen->core->heap, widget->heap.property.addr);
  if(widget->action.clear)
    widget->action.clear(widget);

  if(widget->owner) {
    DListContainer *container = dlist_front(&widget->owner->widgets);
    while(container) {
      FT_Widget *widgetInOwner = *(FT_Widget**)dlist_item(container);
      if(widgetInOwner == widget) {
        dlist_erase(&widget->owner->widgets, container, NULL);
        break;
      }
      container = dlist_next(container);
    }
  }

  ft_widget_semphr_give(widget);
  ft_util_free(widget);
}

void ft_widget_semphr_take(FT_Widget *widget)
{
  xSemaphoreTakeRecursive(widget->screen->mut, portMAX_DELAY);
}

void ft_widget_semphr_give(FT_Widget *widget)
{
  xSemaphoreGiveRecursive(widget->screen->mut);
}

void ft_widget_calculate_dimensions(FT_Widget *widget)
{
  if(!widget->owner)
    return;

  const FT_Coord ownerEdge = {
      .x =  widget->owner->absoluteCoord.x + widget->owner->dimensions.w,
      .y =  widget->owner->absoluteCoord.y + widget->owner->dimensions.h
  };
  const FT_Coord widgetEdge = {
      .x =  widget->absoluteCoord.x + widget->dimensions.w,
      .y =  widget->absoluteCoord.y + widget->dimensions.h
  };
  if(widgetEdge.x > ownerEdge.x)
    widget->dimensions.w -= widgetEdge.x - ownerEdge.x;
  if(widgetEdge.y > ownerEdge.y)
    widget->dimensions.h -= widgetEdge.y - ownerEdge.y;
}

void ft_widget_calculate_absolute_coord(FT_Widget *widget)
{
  widget->absoluteCoord.x = widget->owner->absoluteCoord.x + widget->coord.x;
  widget->absoluteCoord.y = widget->owner->absoluteCoord.y + widget->coord.y;
  ft_widget_set_modified(widget);
}

void ft_widget_calculate_absolute_coord_recursive(FT_Widget *widget)
{
  ft_widget_calculate_absolute_coord(widget);

  DListContainer *container = dlist_front(&widget->widgets);
  while(container) {
    FT_Widget *nested = *(FT_Widget**)dlist_item(container);
    ft_widget_calculate_absolute_coord_recursive(nested);
    container = dlist_next(container);
  }
}

void ft_widget_set_modified(FT_Widget *widget)
{
  if(widget->state == ftContextHeapLoad) {
    ft_heap_remove(&widget->screen->core->heap, widget->heap.property.addr);
    widget->heap.property.addr = 0;
    widget->heap.property.size = 0;
  }
  widget->state = ftContextNotCompile;
}

void ft_widget_set_dependent_param(FT_Widget *widget, FT_Widget *owner)
{
  widget->owner = owner;
  if(owner) {
    widget->screen = owner->screen;
    widget->dimensions.w = owner->dimensions.w;
    widget->dimensions.h = owner->dimensions.h;
  }
  else {
    widget->screen = NULL;
    widget->dimensions.w = 0;
    widget->dimensions.h = 0;
  }
}

void ft_widget_insert_color_cmds(FT_Widget *widget, Vector *cmds)
{
  if(widget->defaultColor == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(widget->color.red, widget->color.green, widget->color.blue));
  if(widget->defaultAlpha == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_a(widget->color.alpha));
}

void ft_widget_insert_touch_cmds(FT_Widget *widget, Vector *cmds)
{
  if(widget->touch.type != ftWidgetTouchNone) {
    if(widget->touch.tag == 0)
      widget->touch.tag = ft_screen_tag(widget->screen);
    if(widget->touch.type == ftWidgetTouchTrack)
      ft_cmd_track(cmds, widget->coord.x, widget->coord.y, widget->dimensions.w, widget->dimensions.h, widget->touch.tag);
    ft_cmd_dl_set(cmds, ft_dl_tag(widget->touch.tag));
  }
}

void ft_widget_insert_touch_color_cmds(FT_Widget *widget, Vector *cmds)
{
  if(widget->touch.type != ftWidgetTouchNone) {
    if(widget->touch.tag == 0)
      widget->touch.tag = ft_screen_tag(widget->screen);
    if(widget->touch.type == ftWidgetTouchTrack)
      ft_cmd_track(cmds, widget->coord.x, widget->coord.y, widget->dimensions.w, widget->dimensions.h, widget->touch.tag);
    ft_cmd_dl_set(cmds, ft_dl_tag(widget->touch.tag));
  }
  if(widget->defaultColor == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(widget->color.red, widget->color.green, widget->color.blue));
  if(widget->defaultAlpha == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_a(widget->color.alpha));
}

FT_Font const* ft_widget_font_table(FT_Widget *widget)
{
  return &widget->screen->core->fontTable;
}

void ft_widget_compile(FT_Widget *widget, Vector *cmds)
{
  ft_widget_calculate_dimensions(widget);
  if(widget->action.compile && widget->visible == bTrue) {
    switch(widget->state) {
      case ftContextNotCompile: {
        if(widget->heap.enable == bFalse) {
          widget->action.compile(widget, cmds);
          break;
        }
        Vector heapCmds = make_vect(32, sizeof(uint32_t));
        widget->action.compile(widget, &heapCmds);
        if(vect_size(&heapCmds) == 0) {
          vect_clear(&heapCmds);
          break;
        }

        // Выделение памяти и запись в диспелей
        FT_Heap *heap = &widget->screen->core->heap;
        if(widget->heap.property.addr != 0)
          ft_heap_remove(heap, widget->heap.property.addr);
        ft_heap_append_own(heap, &heapCmds, &widget->heap.property);
      }
      case ftContextHeapLoad:
        // Добавление в общий вектор команды append
        ft_cmd_append(cmds, widget->heap.property.addr, widget->heap.property.size);
        widget->state = ftContextHeapLoad;
        break;
    }
  }

  // Обработка вложенных виджетов
  DListContainer *container = dlist_front(&widget->widgets);
  while(container) {
    FT_Widget *nested = *(FT_Widget**)dlist_item(container);
    ft_widget_compile(nested, cmds);
    container = dlist_next(container);
  }
}

void ft_widget_set_color(FT_Widget *widget, uint8_t red, uint8_t green, uint8_t blue, Bool defaultFl)
{
  if(widget->color.red != red || widget->color.green != green || widget->color.blue != blue ||
      widget->defaultColor != defaultFl) {
    ft_widget_semphr_take(widget);
    widget->color.red = red;
    widget->color.green = green;
    widget->color.blue = blue;
    widget->defaultColor = defaultFl;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_alpha(FT_Widget *widget, uint8_t alpha, Bool defaultFl)
{
  if(widget->color.alpha != alpha || widget->defaultAlpha != defaultFl) {
    ft_widget_semphr_take(widget);
    widget->color.alpha = alpha;
    widget->defaultAlpha = defaultFl;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_x(FT_Widget *widget, int16_t x)
{
  if(widget->coord.x != x) {
    ft_widget_semphr_take(widget);
    widget->coord.x = x;
    ft_widget_calculate_absolute_coord_recursive(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_y(FT_Widget *widget, int16_t y)
{
  if(widget->coord.y != y) {
    ft_widget_semphr_take(widget);
    widget->coord.y = y;
    ft_widget_calculate_absolute_coord_recursive(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_coord(FT_Widget *widget, int16_t x, int16_t y)
{
  if(widget->coord.x != x || widget->coord.y != y) {
    ft_widget_semphr_take(widget);
    widget->coord.x = x;
    widget->coord.y = y;
    ft_widget_calculate_absolute_coord_recursive(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_w(FT_Widget *widget, int16_t w)
{
  if(widget->dimensions.w != w) {
    ft_widget_semphr_take(widget);
    widget->dimensions.w = w;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_h(FT_Widget *widget, int16_t h)
{
  if(widget->dimensions.h != h) {
    ft_widget_semphr_take(widget);
    widget->dimensions.h = h;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_dimensions(FT_Widget *widget, int16_t w, int16_t h)
{
  if(widget->dimensions.w != w || widget->dimensions.h != h) {
    ft_widget_semphr_take(widget);
    widget->dimensions.w = w;
    widget->dimensions.h = h;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_set_touch_type(FT_Widget *widget, FT_WidgetTouchType type)
{
  if(widget->touch.type != type) {
    ft_widget_semphr_take(widget);
    widget->touch.type = type;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

void ft_widget_slots_connect(FT_Widget *widget, FT_WidgetUserSlot press, FT_WidgetUserSlot long_press,
                             FT_WidgetUserSlot release, FT_WidgetUserSlot reset, void *arg)
{
  ft_widget_semphr_take(widget);
  widget->touch.slot.user.press = press;
  widget->touch.slot.user.long_press = long_press;
  widget->touch.slot.user.release = release;
  widget->touch.slot.user.reset = reset;
  widget->touch.slot.user.arg = arg;
  ft_widget_semphr_give(widget);
}

void ft_widget_set_heap_en(FT_Widget *widget, Bool enable)
{
  if(widget->heap.enable != enable) {
    ft_widget_semphr_take(widget);
    widget->heap.enable = enable;
    ft_widget_set_modified(widget);
    ft_widget_semphr_give(widget);
  }
}

static void heap_en_recursive(FT_Widget *widget, Bool enable)
{
  if(widget->heap.enable != enable) {
    widget->heap.enable = enable;
    ft_widget_set_modified(widget);
  }

  DListContainer *container = dlist_front(&widget->widgets);
  while(container) {
    FT_Widget *nested = *(FT_Widget**)dlist_item(container);
    heap_en_recursive(nested, enable);
    container = dlist_next(container);
  }
}

void ft_widget_set_heap_en_recursive(FT_Widget *widget, Bool enable)
{
  ft_widget_semphr_take(widget);
  heap_en_recursive(widget, enable);
  ft_widget_semphr_give(widget);
}
