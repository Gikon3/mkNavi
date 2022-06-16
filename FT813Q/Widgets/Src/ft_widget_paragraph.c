#include <string.h>
#include "util_vector.h"
#include "util_string.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_paragraph.h"

static uint8_t line_height(FT_Widget *widget, int16_t font)
{
  FT_Font const *fontTable = ft_widget_font_table(widget);
  return fontTable->height[font-16];
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetParagraph *paragraph = (FT_WidgetParagraph*)widget;
  if(paragraph->lines == 0)
    return;

  const uint8_t lineH = line_height(widget, paragraph->font);
  const uint16_t visibleLines = widget->dimensions.h / lineH;
  const int16_t x = widget->absoluteCoord.x;
  int16_t y = widget->absoluteCoord.y;

  ft_cmd_dl_set(cmds, ft_dl_save_context());

  ft_widget_insert_color_cmds(widget, cmds);
  const uint16_t range = visibleLines < paragraph->lines ? visibleLines : paragraph->lines;
  char const *str = paragraph->text;
  for(uint16_t i = 0; i < range; ++i) {
    ft_cmd_text(cmds, x, y, paragraph->font, 0, str);
    str += strlen(str) + 1;
    y += lineH;
  }

  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void clear(FT_Widget *widget)
{
  FT_WidgetParagraph *context = (FT_WidgetParagraph*)widget;
  ft_util_free(context->text);
}

FT_WidgetParagraph* ft_widget_paragraph_create(FT_Widget *owner)
{
  FT_WidgetParagraph *widget = (FT_WidgetParagraph*)ft_widget_create(owner, sizeof(FT_WidgetParagraph));
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->text = NULL;
  widget->lines = 0;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  return widget;
}

void ft_widget_paragraph_set_font(FT_WidgetParagraph *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

static void text_process(FT_WidgetParagraph *widget, char const *text)
{
  const size_t len = strlen(text);
  widget->text = ft_util_malloc(len+1);
  char *widgetText = widget->text;

  widget->lines = 0;
  for(; *text != '\0'; ++text, ++widgetText) {
    if(*text == '\n') {
      *widgetText = '\0';
      ++widget->lines;
      continue;
    }
    *widgetText = *text;
  }
  *widgetText = '\0';
  ++widget->lines;
}

void ft_widget_paragraph_set_text(FT_WidgetParagraph *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  ft_util_free(widget->text);
  text_process(widget, text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}
