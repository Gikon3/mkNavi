#include "util_vector.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"
#include "ft_keyboard.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_line_edit.h"

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetLineEdit *lineEdit = (FT_WidgetLineEdit*)widget;
  if(lineEdit->placeholderText == bTrue) {
    str_clear(&lineEdit->text);
    lineEdit->placeholderText = bFalse;
  }

  if(str_length(&lineEdit->text) > 0) {
    const uint32_t wPixels = arg->val * widget->dimensions.w / 0xFFFF;
    const uint32_t wEdges = widget->dimensions.w / 10;  //< 10 % от краев
    if(wPixels < wEdges || wPixels > (widget->dimensions.w-wEdges)) {
      if(xTimerIsTimerActive(lineEdit->timers.outOfBonds) == pdFALSE)
        xTimerStart(lineEdit->timers.outOfBonds, portMAX_DELAY);
    }
    else {
      if(xTimerIsTimerActive(lineEdit->timers.outOfBonds) == pdTRUE)
        xTimerStop(lineEdit->timers.outOfBonds, portMAX_DELAY);
      FT_Font const *fontTable = ft_widget_font_table(widget);
      char const *str = &str_cdata(&lineEdit->text)[lineEdit->cursor.startPos];
      const uint16_t oldPosition = lineEdit->cursor.position;
      lineEdit->cursor.position = lineEdit->cursor.startPos + ft_util_text_find_pos(fontTable, lineEdit->font, str, wPixels);
      if(oldPosition != lineEdit->cursor.position) {
        ft_widget_set_modified(widget);
        ft_screen_draw(widget->screen);
      }
    }
  }

  if(xTimerIsTimerActive(lineEdit->timers.cursorBlinking) == pdTRUE) {
    lineEdit->cursor.visible = bTrue;
    xTimerStop(lineEdit->timers.cursorBlinking, portMAX_DELAY);
  }
}

static void relese(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetLineEdit *lineEdit = (FT_WidgetLineEdit*)widget;
  if(lineEdit->enable == bFalse)
    ft_keyboard_attach(&lineEdit->screen->keyboard, lineEdit);
  if(xTimerIsTimerActive(lineEdit->timers.outOfBonds) == pdTRUE)
    xTimerStop(lineEdit->timers.outOfBonds, portMAX_DELAY);
  if(xTimerIsTimerActive(lineEdit->timers.cursorBlinking) == pdFALSE)
    xTimerStart(lineEdit->timers.cursorBlinking, portMAX_DELAY);
  ft_widget_set_modified(widget);
  ft_screen_draw(widget->screen);
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetLineEdit *lineEdit = (FT_WidgetLineEdit*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_color_cmds(widget, cmds);

  const int16_t x = widget->absoluteCoord.x;
  const int16_t y = widget->absoluteCoord.y;
  const int16_t gap = widget->dimensions.h / 5;
  ft_shape_rectangle_outline(cmds, x, y, widget->dimensions.w, widget->dimensions.h);

  FT_Font const *fontTable = ft_widget_font_table(widget);
  const uint8_t height = fontTable->height[lineEdit->font-16];
  const int16_t textX = x + gap;
  const int16_t textY = y + (widget->dimensions.h - height) / 2;

  char *cutStr = NULL;
  if(str_length(&lineEdit->text) > 0) {
    const uint32_t pos = lineEdit->cursor.position < str_length(&lineEdit->text) ? lineEdit->cursor.position :
        lineEdit->cursor.position - 1;
    uint32_t endPos = 0;
    cutStr = ft_util_text_cut_edges(fontTable, lineEdit->font, str_cdata(&lineEdit->text),
        &lineEdit->cursor.startPos, pos, &endPos, widget->dimensions.w-gap*2, widget->dimensions.h);
    lineEdit->cursor.endPos = endPos;
    if(cutStr)
      ft_cmd_text(cmds, textX, textY, lineEdit->font, 0, cutStr);
  }
  if(lineEdit->cursor.visible == bTrue) {
    const uint32_t pos = lineEdit->cursor.position - lineEdit->cursor.startPos;
    const int16_t cursorX = textX + ft_util_text_width(fontTable, lineEdit->font, cutStr, pos);
    ft_shape_line(cmds, cursorX, textY, cursorX, textY+height);
  }
  ft_util_free(cutStr);

  if(widget->touch.tag == 0)
    widget->touch.tag = ft_screen_tag(widget->screen);
  if(widget->touch.type == ftWidgetTouchTrack)
    ft_cmd_track(cmds, x+gap, y, widget->dimensions.w-gap*2, widget->dimensions.h, widget->touch.tag);
  ft_cmd_dl_set(cmds, ft_dl_tag(widget->touch.tag));
  ft_cmd_dl_set(cmds, ft_dl_color_a(0));
  ft_shape_rectangle(cmds, x+gap, y,  widget->dimensions.w-gap*2, widget->dimensions.h);

  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetLineEdit *context = (FT_WidgetLineEdit*)widget;
  if(context->enable == bTrue)
    ft_keyboard_detach(&context->screen->keyboard);
  str_clear(&context->text);
  xTimerDelete(context->timers.cursorBlinking, portMAX_DELAY);
  xTimerDelete(context->timers.outOfBonds, portMAX_DELAY);
}

static void tim_cursor_blinking(TimerHandle_t tim)
{
  FT_WidgetLineEdit *lineEdit = pvTimerGetTimerID(tim);
  lineEdit->cursor.visible = lineEdit->cursor.visible == bFalse ? bTrue : bFalse;
  ft_widget_set_modified(&lineEdit->parent);
  ft_screen_draw(lineEdit->screen);
}

static void tim_out_of_bonds(TimerHandle_t tim)
{
  FT_WidgetLineEdit *lineEdit = pvTimerGetTimerID(tim);
  const uint32_t left = lineEdit->cursor.position - lineEdit->cursor.startPos;
  const uint32_t right = lineEdit->cursor.endPos - lineEdit->cursor.position;
  if(left < right && lineEdit->cursor.position > 0)
    --lineEdit->cursor.position;
  else if(right < left && lineEdit->cursor.position < str_length(&lineEdit->text))
    ++lineEdit->cursor.position;
  ft_widget_set_modified(&lineEdit->parent);
  ft_screen_draw(lineEdit->screen);
}

FT_WidgetLineEdit* ft_widget_line_edit_create(FT_Widget *owner)
{
  FT_WidgetLineEdit *widget = (FT_WidgetLineEdit*)ft_widget_create(owner, sizeof(FT_WidgetLineEdit));
  widget->parent.touch.type = ftWidgetTouchTrack;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->timers.cursorBlinking = xTimerCreate("ftCursorBlinking", pdMS_TO_TICKS(500),
      pdTRUE, widget, tim_cursor_blinking);
  widget->timers.outOfBonds = xTimerCreate("ftOutOfBonds", pdMS_TO_TICKS(100),
      pdTRUE, widget, tim_out_of_bonds);
  widget->enable = bFalse;
  widget->placeholderText = bTrue;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->text = make_str_null();
  widget->cursor.visible = bFalse;
  widget->cursor.startPos = 0;
  widget->cursor.endPos = 0;
  widget->cursor.position = 0;
  return widget;
}

void ft_widget_line_edit_set_font(FT_WidgetLineEdit *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_line_edit_set_text(FT_WidgetLineEdit *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_line_edit_set_enable(FT_WidgetLineEdit *widget, Bool enable)
{
  ft_widget_semphr_take(&widget->parent);
  if(enable == bFalse) {
    widget->cursor.position = str_length(&widget->text);
    if(xTimerIsTimerActive(widget->timers.cursorBlinking) == pdTRUE)
      xTimerStop(widget->timers.cursorBlinking, portMAX_DELAY);
    if(xTimerIsTimerActive(widget->timers.outOfBonds) == pdTRUE)
      xTimerStop(widget->timers.outOfBonds, portMAX_DELAY);
  }
  else
    xTimerStart(widget->timers.cursorBlinking, portMAX_DELAY);
  widget->cursor.visible = enable;
  widget->enable = enable;
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_line_edit_inser_char(FT_WidgetLineEdit *widget, char ch)
{
  ft_widget_semphr_take(&widget->parent);
  str_insert_char(&widget->text, widget->cursor.position, ch);
  ++widget->cursor.position;
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_line_edit_erase_char(FT_WidgetLineEdit *widget)
{
  ft_widget_semphr_take(&widget->parent);
  if(widget->cursor.position > 0) {
    str_erase_char(&widget->text, widget->cursor.position-1);
    --widget->cursor.position;
    if(widget->cursor.startPos > 0)
      --widget->cursor.startPos;
    ft_widget_set_modified(&widget->parent);
  }
  ft_widget_semphr_give(&widget->parent);
}

char const* ft_widget_line_edit_text(FT_WidgetLineEdit *widget)
{
  ft_widget_semphr_take(&widget->parent);
  char const *text = str_cdata(&widget->text);
  ft_widget_semphr_give(&widget->parent);
  return text;
}
