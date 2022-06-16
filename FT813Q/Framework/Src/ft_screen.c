#include <string.h>
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_heap_impl.h"
#include "ft_touch.h"
#include "ft_keyboard.h"
#include "ft_screen.h"
#include "ft_widget.h"

#define BITMAP_HANDLE_MAX_VAL 14

FT_Screen* ft_screen_create(FT_Core *core)
{
  FT_Screen *screen = ft_util_malloc(sizeof(FT_Screen));
  screen->core = core;
  screen->mut = xSemaphoreCreateRecursiveMutex();
  memset(&screen->tags[0], 0, sizeof(screen->tags));
  screen->bitmapHandle = 0;
  ft_touch_touchscreen_init(&screen->touchScreen);
  ft_keyboard_init(&screen->keyboard, screen);
  screen->widget = ft_widget_create(NULL, 0);
  screen->widget->screen = screen;
  screen->widget->heap.enable = bFalse;
  screen->widget->dimensions.w = core->host.param.h.size;
  screen->widget->dimensions.h = core->host.param.v.size;
  return screen;
}

void ft_screen_destroy(FT_Screen *screen)
{
  xSemaphoreTakeRecursive(screen->mut, portMAX_DELAY);
  ft_widget_destroy(ft_screen_widget(screen));
  ft_keyboard_destroy(&screen->keyboard);
  ft_touch_touchscreen_destroy(&screen->touchScreen);
  xSemaphoreGiveRecursive(screen->mut);
  vSemaphoreDelete(screen->mut);
  ft_util_free(screen);
}

FT_Widget* ft_screen_widget(FT_Screen *screen)
{
  return screen->widget;
}

void ft_screen_load(FT_Screen *screen, Vector *cmds)
{
  FT_Host const *host = &screen->core->host;
  Vector startFrame = make_vect(2, sizeof(uint32_t));
  ft_cmd_dlstart(&startFrame);
  ft_cmd_append(&startFrame, screen->core->startFrame.addr, screen->core->startFrame.size);
  ft_hal_wr_into_cmd(host, &startFrame);

  ft_hal_wr_into_cmd(host, cmds);

  Vector endFrame = make_vect(10, sizeof(uint32_t));
  ft_touch_gesture_show(screen->core, &endFrame);
  ft_cmd_dl_set(&endFrame, ft_dl_display());
  ft_cmd_dlswap(&endFrame);
  ft_hal_wr_into_cmd(host, &endFrame);

  screen->draw = bFalse;
}

void ft_screen_draw(FT_Screen *screen)
{
  xSemaphoreTakeRecursive(screen->mut, portMAX_DELAY);
  screen->draw = bTrue;
  xSemaphoreGiveRecursive(screen->mut);
}

uint16_t ft_screen_tag(FT_Screen *screen)
{
  for(size_t word = 0; word < FT_SCREEN_TAGS_WORDS; ++word) {
    uint32_t wordVal = screen->tags[word];
    if(~wordVal != 0) {
      for(size_t bit = 0; bit < FT_SCREEN_TAGS_BITS; ++bit) {
        if((wordVal & 1) == bFalse) {
          screen->tags[word] |= bTrue << bit;
          return FT_SCREEN_TAGS_START + word * FT_SCREEN_TAGS_BITS + bit;
        }
        wordVal >>= 1;
      }
    }
  }
  return 0;
}

void ft_screen_clear_tag(FT_Screen *screen, uint16_t tag)
{
  if(tag < FT_SCREEN_TAGS_START)
    return;
  const size_t word = (tag - FT_SCREEN_TAGS_START) / FT_SCREEN_TAGS_BITS;
  const size_t bit = tag % FT_SCREEN_TAGS_BITS;
  screen->tags[word] &= ~((uint32_t)1 << bit);
}

int8_t ft_screen_bitmap_handle(FT_Screen *screen)
{
  uint16_t handlePool = screen->bitmapHandle;
  for(size_t bit = 0; bit <= BITMAP_HANDLE_MAX_VAL; ++bit) {
    if((handlePool & 1) == bFalse) {
      screen->bitmapHandle |= bTrue << bit;
      return bit;
    }
    handlePool >>= 1;
  }
  return -1;
}

void ft_screen_bitmap_handle_clear(FT_Screen *screen, int8_t handle)
{
  if(handle < 0 || handle > BITMAP_HANDLE_MAX_VAL)
    return;
  screen->bitmapHandle &= ~((uint16_t)1 << handle);
}
