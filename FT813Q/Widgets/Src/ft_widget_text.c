#include "util_vector.h"
#include "util_string.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_text.h"

#define SCROLL_W      30
#define SCROLL_MARGIN 20

typedef struct
{
  FT_Color  color;
  String    line;
} LineInfo;

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetText *text = (FT_WidgetText*)widget;
  text->touched = bTrue;

  FT_Font const *fontTable = ft_widget_font_table(widget);
  const uint8_t lineH = ft_util_font_height(fontTable, text->font);
  const uint16_t visibleLines = widget->dimensions.h / lineH;
  const uint16_t step = 0xFFFF / (list_size(&text->lines) - visibleLines);
  text->cursor = (0xFFFF - arg->val) / step;

  ft_widget_set_modified(widget);
  ft_screen_draw(widget->screen);
}

static void relese(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetText *text = (FT_WidgetText*)widget;
  text->touched = bFalse;
  ft_widget_set_modified(widget);
  ft_screen_draw(widget->screen);
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetText *text = (FT_WidgetText*)widget;
  List *lines = &text->lines;
  if(list_empty(lines) == bFalse) {
    FT_Font const *fontTable = ft_widget_font_table(widget);
    const uint8_t lineH = ft_util_font_height(fontTable, text->font);
    const uint16_t visibleLines = widget->dimensions.h / lineH;
    const uint16_t totalLines = list_size(lines);
    const int16_t x = widget->absoluteCoord.x;
    int16_t y = widget->absoluteCoord.y;

    ListContainer *container = list_front(lines);
    if(totalLines > visibleLines)
      for(uint16_t i = 0; i < (totalLines-visibleLines-text->cursor); ++i)
        container = list_next(container);

    ft_cmd_dl_set(cmds, ft_dl_save_context());

    FT_Color currentColor;
    currentColor.value = ((LineInfo*)list_item(container))->color.value;
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(currentColor.red, currentColor.green, currentColor.blue));
    const uint16_t visibleRange = totalLines < visibleLines ? totalLines : visibleLines;
    for(uint16_t i = 0; i < visibleRange; ++i) {
      LineInfo *info = (LineInfo*)list_item(container);
      if(info->color.value != currentColor.value) {
        currentColor.value = info->color.value;
        ft_cmd_dl_set(cmds, ft_dl_color_rgb(currentColor.red, currentColor.green, currentColor.blue));
      }
      ft_cmd_text(cmds, x, y, text->font, 0, str_cdata(&info->line));
      y += lineH;
      container = list_next(container);
    }

    if(totalLines > visibleLines)
      for(uint16_t i = 0; i < text->cursor; ++i)
        container = list_next(container);

    if(totalLines > visibleLines) {
      const int16_t scrollX = x + widget->dimensions.w - SCROLL_W;
      const int16_t scrollY = widget->absoluteCoord.y + SCROLL_MARGIN;
      const int16_t scrollH = widget->dimensions.h - SCROLL_MARGIN * 2;

      if(widget->touch.tag == 0)
        widget->touch.tag = ft_screen_tag(widget->screen);
      ft_cmd_track(cmds, scrollX, widget->absoluteCoord.y, SCROLL_W, widget->dimensions.h, widget->touch.tag);
      ft_cmd_dl_set(cmds, ft_dl_tag(widget->touch.tag));

      uint32_t options = FT_OPT_3D;
      if(text->touched == bTrue)
        options = FT_OPT_FLAT;
      const uint32_t value = totalLines - visibleLines - text->cursor;
      const uint32_t size = visibleLines;
      ft_cmd_scrollbar(cmds, scrollX, scrollY, SCROLL_W, scrollH, options, value, size, totalLines);
    }

    ft_cmd_dl_set(cmds, ft_dl_restore_context());
  }
}

static void clear(FT_Widget *widget)
{
  FT_WidgetText *context = (FT_WidgetText*)widget;
  List *lines = &context->lines;
  while(list_empty(lines) == bFalse) {
    LineInfo info;
    list_pop_front(lines, &info);
    str_clear(&info.line);
  }
}

FT_WidgetText* ft_widget_text_create(FT_Widget *owner)
{
  FT_WidgetText *widget = (FT_WidgetText*)ft_widget_create(owner, sizeof(FT_WidgetText));
  widget->parent.touch.type = ftWidgetTouchTrack;
  FT_Slots *slots = &widget->parent.touch.slot.common;
  slots->press = press;
  slots->release = relese;
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->lines = make_list(sizeof(LineInfo));
  widget->maxLines = 3;
  widget->cursor = 0;
  widget->touched = bFalse;
  widget->cut = ftTextNoCut;
  return widget;
}

void ft_widget_text_set_font(FT_WidgetText *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_text_set_max_lines(FT_WidgetText *widget, uint16_t maxLines)
{
  if(widget->maxLines != maxLines) {
    ft_widget_semphr_take(&widget->parent);
    widget->maxLines = maxLines;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

static char const* text_process(char const *text, String *str)
{
  char const *begin = text;
  for(; *text != '\0' && *text != '\r' && *text != '\n'; ++text);
  if((text-begin) > 0)
    *str = make_str_arr(begin, text);
  else
    *str = make_str_null();

  char const *nextText = text + 1;
  if(*text == '\r' && *(nextText) == '\n')
    ++nextText;
  return *text != '\0' && *nextText != '\0' ? nextText : NULL;
}

void ft_widget_text_append_text(FT_WidgetText *widget, char const *text, uint32_t color)
{
  ft_widget_semphr_take(&widget->parent);

  Bool modified = bFalse;
  while(text) {
    LineInfo info;
    info.color.value = color;
    text = text_process(text, &info.line);

    if(str_empty(&info.line) == bFalse) {
      list_push_back(&widget->lines, &info);
      if(list_size(&widget->lines) > widget->maxLines) {
        LineInfo removeInfo;
        list_pop_front(&widget->lines, &removeInfo);
        str_clear(&removeInfo.line);
      }
      FT_Font const *fontTable = ft_widget_font_table(&widget->parent);
      const uint16_t visibleLines = widget->parent.dimensions.h / ft_util_font_height(fontTable, widget->font);
      const uint16_t totalLines = list_size(&widget->lines);
      if(widget->cursor != 0 && widget->cursor < (totalLines-visibleLines))
        ++widget->cursor;
      modified = bTrue;
    }
  }

  if(modified == bTrue)
    ft_widget_set_modified(&widget->parent);

  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_text_clear_text(FT_WidgetText *widget)
{
  ft_widget_semphr_take(&widget->parent);

  clear(&widget->parent);
  ft_widget_set_modified(&widget->parent);

  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_text_set_cut(FT_WidgetText *widget, FT_TextCut cut)
{
  if(widget->cut != cut) {
    ft_widget_semphr_take(&widget->parent);
    widget->cut = cut;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}
