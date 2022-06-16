#ifndef FRAMEWORK_SRC_FT_WIDGET_DEFAULT_H_
#define FRAMEWORK_SRC_FT_WIDGET_DEFAULT_H_

#include "ft_widget_impl.h"

#define FT_WIDGET_DEFAULT_FONT  26

FT_Widget *ft_widget_create(FT_Widget *owner, size_t size);
void ft_widget_destroy(FT_Widget *widget);
void ft_widget_semphr_take(FT_Widget *widget);
void ft_widget_semphr_give(FT_Widget *widget);
void ft_widget_calculate_dimensions(FT_Widget *widget);
void ft_widget_calculate_absolute_coord(FT_Widget *widget);
void ft_widget_calculate_absolute_coord_recursive(FT_Widget *widget);
void ft_widget_set_modified(FT_Widget *widget);
void ft_widget_set_dependent_param(FT_Widget *widget, FT_Widget *owner);
void ft_widget_insert_color_cmds(FT_Widget *widget, Vector *cmds);
void ft_widget_insert_touch_cmds(FT_Widget *widget, Vector *cmds);
void ft_widget_insert_touch_color_cmds(FT_Widget *widget, Vector *cmds);
FT_Font const* ft_widget_font_table(FT_Widget *widget);
void ft_widget_compile(FT_Widget *widget, Vector *cmds);
void ft_widget_set_color(FT_Widget *widget, uint8_t red, uint8_t green, uint8_t blue, Bool defaultFl);
void ft_widget_set_alpha(FT_Widget *widget, uint8_t alpha, Bool defaultFl);
void ft_widget_set_x(FT_Widget *widget, int16_t x);
void ft_widget_set_y(FT_Widget *widget, int16_t y);
void ft_widget_set_coord(FT_Widget *widget, int16_t x, int16_t y);
void ft_widget_set_w(FT_Widget *widget, int16_t w);
void ft_widget_set_h(FT_Widget *widget, int16_t h);
void ft_widget_set_dimensions(FT_Widget *widget, int16_t w, int16_t h);
void ft_widget_set_touch_type(FT_Widget *widget, FT_WidgetTouchType type);
void ft_widget_slots_connect(FT_Widget *widget, FT_WidgetUserSlot press, FT_WidgetUserSlot long_press,
                             FT_WidgetUserSlot release, FT_WidgetUserSlot reset, void *arg);
void ft_widget_set_heap_en(FT_Widget *widget, Bool enable);
void ft_widget_set_heap_en_recursive(FT_Widget *widget, Bool enable);

#endif /* FRAMEWORK_SRC_FT_WIDGET_DEFAULT_H_ */
