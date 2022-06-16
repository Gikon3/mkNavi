#ifndef WIDGETS_INC_FT_WIDGET_TAB_H_
#define WIDGETS_INC_FT_WIDGET_TAB_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_utils_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

typedef struct
{
  FT_WIDGET_HEADER
  uint8_t touched;
} FT_WidgetTab;

FT_WidgetTab* ft_widget_tab_create(FT_Widget *owner);
void ft_widget_tab_delete_slot_connect(FT_WidgetTab *widget, FT_Widget const *body, FT_WidgetUserSlot slot, void *arg);
void ft_widget_tab_set_font(FT_WidgetTab *widget, int16_t font);
void ft_widget_tab_set_cut(FT_WidgetTab *widget, FT_TextCut cut);
void ft_widget_tab_set_color_active_tab(FT_WidgetTab *widget, FT_Color color);
void ft_widget_tab_set_color_inactive_tab(FT_WidgetTab *widget, FT_Color color);
void ft_widget_tab_set_color_active_tab_name(FT_WidgetTab *widget, FT_Color color);
void ft_widget_tab_set_color_inactive_tab_name(FT_WidgetTab *widget, FT_Color color);
void ft_widget_tab_set_max_tabs(FT_WidgetTab *widget, uint8_t maxTabs);
void ft_widget_tab_set_closeable_tabs(FT_WidgetTab *widget, Bool closeableTabs);
void ft_widget_tab_set_tab_name(FT_WidgetTab *window, FT_Widget *body, char const *newName);
FT_Widget* ft_widget_tab_new_tab(FT_WidgetTab *widget, char const *tabName);
void ft_widget_tab_delete_tab(FT_WidgetTab *widget, FT_Widget *body);

#endif /* WIDGETS_INC_FT_WIDGET_TAB_H_ */
