#ifndef FRAMEWORK_INC_FT_KEYBOARD_H_
#define FRAMEWORK_INC_FT_KEYBOARD_H_

#include "util_types.h"
#include "ft_core_impl.h"
#include "ft_screen_impl.h"
#include "ft_keyboard_impl.h"

void ft_keyboard_press(FT_Keyboard *keyboard, uint16_t tag);
void ft_keyboard_long_press(FT_Keyboard *keyboard, uint16_t tag);
void ft_keyboard_release(FT_Keyboard *keyboard, uint16_t tag);
void ft_keyboard_reset(FT_Keyboard *keyboard, uint16_t tag);
void ft_keyboard_compile(FT_Keyboard *keyboard, Vector *cmds);
void ft_keyboard_init(FT_Keyboard *keyboard, FT_Screen *screen);
void ft_keyboard_destroy(FT_Keyboard *keyboard);
void ft_keyboard_attach(FT_Keyboard *keyboard, struct _FT_WidgetLineEdit *lineEdit);
void ft_keyboard_detach(FT_Keyboard *keyboard);

#endif /* FRAMEWORK_INC_FT_KEYBOARD_H_ */
