#include <string.h>
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_core.h"
#include "ft_heap.h"
#include "ft_keyboard.h"
#include "ft_screen.h"
#include "ft_widget.h"

void ft_core_init(FT_Core *core, SPI_IF *spiIF, FT_Display const *param, GPIO_TypeDef *port, uint16_t pin, uint32_t delay)
{
  ft_hal_init(&core->host, spiIF, param, port, pin);
  ft_heap_init(&core->heap, &core->host);
  core->delayMS = delay;
  core->tskMain = NULL;
  core->tskSlotProcess = NULL;
  core->qSlotItem = NULL;
  core->screen = NULL;
}

void ft_core_screen_connect(FT_Core *core, FT_Screen *screen)
{
  if(core->screen != screen) {
    FT_Screen *oldScreen = core->screen;
    if(oldScreen)
      xSemaphoreTakeRecursive(oldScreen->mut, portMAX_DELAY);
    screen->core = core;
    core->screen = screen;
    if(oldScreen)
      xSemaphoreGiveRecursive(oldScreen->mut);
  }
}

static inline void loop(FT_Core *core)
{
  if(!core->screen)
    return;

  ft_touch_touchscreen_load(core);

  xSemaphoreTakeRecursive(core->screen->mut, portMAX_DELAY);
  ft_touch_touchscreen_process(core);
  const Bool gestureDraw = ft_touch_gesture_draw(&core->screen->touchScreen.gesture);
  if(core->screen->draw == bTrue || gestureDraw == bTrue) {
    Vector commands = make_vect(64, sizeof(uint32_t));
    ft_widget_compile(core->screen->widget, &commands);
    ft_keyboard_compile(&core->screen->keyboard, &commands);
    if(vect_size(&commands) > 0)
      ft_screen_load(core->screen, &commands);
    else
      vect_clear(&commands);
  }
  xSemaphoreGiveRecursive(core->screen->mut);
}

static void main_loop(void *arg)
{
  FT_Core *core = (FT_Core *)arg;
  for(;;) {
    const uint32_t start = xTaskGetTickCount();
    loop(core);
    osDelayUntil(start+pdMS_TO_TICKS(core->delayMS));
  }
}

static void slot_process(void *arg)
{
  FT_Core *core = (FT_Core *)arg;
  for(;;) {
    FT_SlotItem slotItem;
    xQueueReceive(core->qSlotItem, &slotItem, portMAX_DELAY);
    ft_widget_semphr_take(core->screen->widget);
    switch(slotItem.type) {
      case ftWidgetSlotInternal:
        slotItem.slot(slotItem.widget, &slotItem.param);
        break;
      case ftWidgetSlotUser:
        slotItem.userSlot(slotItem.widget, &slotItem.param,  slotItem.arg);
        break;
    }
    ft_widget_semphr_give(core->screen->widget);
  }
}

void ft_core_config(FT_Core *core, uint32_t stackSize, uint32_t queueSize, FT_Theme const *theme)
{
  ft_hal_config(&core->host);

  xTaskCreate(main_loop, "ftMain", 224, core, osPriorityNormal, &core->tskMain);
  xTaskCreate(slot_process, "ftSlotProcess", stackSize, core, osPriorityNormal, &core->tskSlotProcess);
  core->qSlotItem = xQueueCreate(queueSize, sizeof(FT_SlotItem));

  // Выгрузка размеров шрифтов
  const uint32_t romFontAddr = ft_hal_rd32(&core->host, FT_ROM_FONT_ADDR);
  const uint32_t fontTableOffset = 148;
  for(uint32_t i = 0; i < 19; ++i) {
    uint32_t param[2];  //< Максимальная ширина и высота символов
    const uint32_t addr = romFontAddr + fontTableOffset * i + 136;
    ft_hal_rd(&core->host, addr, (uint8_t *)param, sizeof(param));
    core->fontTable.widthMax[i] = param[0];
    core->fontTable.height[i] = param[1];
  }
  for(uint32_t i = 10; i < 19; ++i) {
    const uint32_t addr = romFontAddr + fontTableOffset * i + ' ';  //< Чтение начинается с пробела
    ft_hal_rd(&core->host, addr, core->fontTable.width[i-10], 95);
  }

  // Загрузка начального фрейма в кучу дисплея
  Vector startFrame = make_vect(5, sizeof(uint32_t));
  ft_cmd_dl_set(&startFrame, ft_dl_clear_color_rgb(theme->clearColor.red, theme->clearColor.blue, theme->clearColor.green));
  ft_cmd_dl_set(&startFrame, ft_dl_clear_color_a(theme->clearColor.alpha));
  ft_cmd_dl_set(&startFrame, ft_dl_clear(bTrue, bTrue, bTrue));
  ft_cmd_dl_set(&startFrame, ft_dl_color_rgb(theme->color.red, theme->color.blue, theme->color.green));
  ft_cmd_dl_set(&startFrame, ft_dl_color_a(theme->color.alpha));
  ft_heap_append_own(&core->heap, &startFrame, &core->startFrame);

  Vector colors = make_vect(4, sizeof(uint32_t));
  ft_cmd_bgcolor(&colors, theme->bgColor.value);
  ft_cmd_fgcolor(&colors, theme->fgColor.value);
  ft_hal_wr_into_cmd(&core->host, &colors);
}
