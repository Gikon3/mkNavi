#ifndef FRAMEWORK_INC_FT_SCREEN_H_
#define FRAMEWORK_INC_FT_SCREEN_H_

#include "ft_core_impl.h"
#include "ft_screen_impl.h"
#include "ft_widget_impl.h"

FT_Screen* ft_screen_create(FT_Core *core);
void ft_screen_destroy(FT_Screen *screen);
FT_Widget* ft_screen_widget(FT_Screen *screen);
void ft_screen_load(FT_Screen *screen, Vector *cmds);
void ft_screen_draw(FT_Screen *screen);
uint16_t ft_screen_tag(FT_Screen *screen);
void ft_screen_clear_tag(FT_Screen *screen, uint16_t tag);
int8_t ft_screen_bitmap_handle(FT_Screen *screen);
void ft_screen_bitmap_handle_clear(FT_Screen *screen, int8_t handle);

#endif /* FRAMEWORK_INC_FT_SCREEN_H_ */
