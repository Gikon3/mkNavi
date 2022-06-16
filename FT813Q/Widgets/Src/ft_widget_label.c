#include "util_vector.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_label.h"

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetLabel *label = (FT_WidgetLabel*)widget;
  if(str_length(&label->text) > 0) {
    ft_cmd_dl_set(cmds, ft_dl_save_context());
    ft_widget_insert_touch_color_cmds(widget, cmds);
    FT_Font const *fontTable = ft_widget_font_table(widget);
    char *cutText = ft_util_text_cut(fontTable, label->font, &label->text, label->cut,
        widget->dimensions.w, widget->dimensions.h);
    ft_cmd_text(cmds, widget->absoluteCoord.x, widget->absoluteCoord.y, label->font, label->options,
        label->cut == ftTextNoCut ? str_data(&label->text) : cutText);
    ft_util_free(cutText);
    ft_cmd_dl_set(cmds, ft_dl_restore_context());
  }
}

static void clear(FT_Widget *widget)
{
  FT_WidgetLabel *context = (FT_WidgetLabel*)widget;
  str_clear(&context->text);
}

FT_WidgetLabel* ft_widget_label_create(FT_Widget *owner)
{
  FT_WidgetLabel *widget = (FT_WidgetLabel*)ft_widget_create(owner, sizeof(FT_WidgetLabel));
  widget->parent.action.compile = compile;
  widget->parent.action.clear = clear;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->text = make_str_null();
  widget->options = 0;
  widget->cut = ftTextNoCut;
  return widget;
}

void ft_widget_label_set_font(FT_WidgetLabel *widget, int16_t font)
{
  if(widget->font != font) {
    ft_widget_semphr_take(&widget->parent);
    widget->font = font;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_label_set_text(FT_WidgetLabel *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_clear(&widget->text);
  widget->text = make_str(text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_label_append_text(FT_WidgetLabel *widget, char const *text)
{
  ft_widget_semphr_take(&widget->parent);
  str_append(&widget->text, text);
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_label_set_options(FT_WidgetLabel *widget, uint16_t options)
{
  if(widget->options != options) {
    ft_widget_semphr_take(&widget->parent);
    widget->options = options;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}

void ft_widget_label_set_cut(FT_WidgetLabel *widget, FT_TextCut cut)
{
  if(widget->cut != cut) {
    ft_widget_semphr_take(&widget->parent);
    widget->cut = cut;
    ft_widget_set_modified(&widget->parent);
    ft_widget_semphr_give(&widget->parent);
  }
}
