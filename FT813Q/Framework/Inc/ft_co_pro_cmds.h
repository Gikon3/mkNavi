#ifndef FRAMEWORK_INC_FT_CO_PRO_COMMANDS_H_
#define FRAMEWORK_INC_FT_CO_PRO_COMMANDS_H_

#include "stm32f4xx.h"
#include "util_vector.h"
#include "ft_co_pro_cmds_options.h"

// Подствление команды списка отображения
void ft_cmd_dl_set(Vector* context, uint32_t dl);

// Начало и конец списка отображения
void ft_cmd_dlstart(Vector* context);
void ft_cmd_dlswap(Vector* context);

// Рисование графических объектов
void ft_cmd_setfont(Vector* context, uint32_t font, uint32_t ptr);
void ft_cmd_setfont2(Vector* context, uint32_t font, uint32_t ptr, uint32_t firstChar);
void ft_cmd_setscratch(Vector* context, uint32_t handle);
void ft_cmd_text(Vector* context, int16_t x, int16_t y, int16_t font,
                 uint16_t options, char const* s);
void ft_cmd_button(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t font, uint16_t options, char const* s);
void ft_cmd_clock(Vector* context, int16_t x, int16_t y, int16_t r, uint16_t options,
                  uint16_t h, uint16_t m, uint16_t s, uint16_t ms);
void ft_cmd_bgcolor(Vector* context, uint32_t c);
void ft_cmd_fgcolor(Vector* context, uint32_t c);
void ft_cmd_gradcolor(Vector* context, uint32_t c);
void ft_cmd_gauge(Vector* context, int16_t x, int16_t y, int16_t r, uint16_t options,
                  uint16_t major, uint16_t minor, uint16_t value, uint16_t range);
void ft_cmd_gradient(Vector* context, int16_t x0, int16_t y0, uint32_t rgb0,
                     int16_t x1, int16_t y1, uint32_t rgb1);
void ft_cmd_keys(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                 int16_t font, uint16_t options, char const* s);
void ft_cmd_pregress(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t options, uint16_t value, uint16_t range);
void ft_cmd_scrollbar(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                      uint16_t options, uint16_t value, uint16_t size, uint16_t range);
void ft_cmd_slider(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   uint16_t options, uint16_t value, uint16_t range);
void ft_cmd_dial(Vector* context, int16_t x, int16_t y, int16_t r,
                 uint16_t options, uint16_t value);
void ft_cmd_toggle(Vector* context, int16_t x, int16_t y, int16_t w, int16_t font,
                   uint16_t options, uint16_t state, char const* s);
void ft_cmd_setbase(Vector* context, uint32_t b);
void ft_cmd_number(Vector* context, int16_t x, int16_t y, int16_t font,
                   uint16_t options, int32_t n);

// Работа с памятью
void ft_cmd_memcrc(Vector* context, uint32_t ptr, uint32_t num, uint32_t result);
void ft_cmd_memzero(Vector* context, uint32_t ptr, uint32_t num);
void ft_cmd_memset(Vector* context, uint32_t ptr, uint32_t value, uint32_t num);
void ft_cmd_memwrite(Vector* context, uint32_t ptr, uint32_t size, uint8_t const* data);
void ft_cmd_memcpy(Vector* context, uint32_t dst, uint32_t src, uint32_t num);
void ft_cmd_append(Vector* context, uint32_t ptr, uint32_t num);

// Загрузка данных изображения в RAM_G (FT81X)
void ft_cmd_inflane(Vector* context, uint32_t ptr, uint8_t const *data, size_t size);
void ft_cmd_loadimage(Vector* context, uint32_t ptr, uint32_t options);

// Установка матрицы преобразования растрового изображения
void ft_cmd_loadidentity(Vector* context);
void ft_cmd_translate(Vector* context, int32_t tx, int32_t ty);
void ft_cmd_scale(Vector* context, int32_t sx, int32_t sy);
void ft_cmd_rotate(Vector* context, int32_t a);
void ft_cmd_setmatrix(Vector* context);
void ft_cmd_getmatrix(Vector* context, int32_t a, int32_t b, int32_t c,
                      int32_t d, int32_t e, int32_t f);

// Другое
void ft_cmd_coldstart(Vector* context);
void ft_cmd_interrupt(Vector* context, uint32_t ms);
void ft_cmd_regread(Vector* context, uint32_t ptr, uint32_t result);
void ft_cmd_calibrate(Vector* context, uint32_t result);
void ft_cmd_romfont(Vector* context, uint32_t font, uint32_t romslot);
void ft_cmd_setrotate(Vector* context, uint32_t r);
void ft_cmd_setbitmap(Vector* context, uint32_t addr, uint16_t fmt, uint16_t w, uint16_t h);
void ft_cmd_spinner(Vector* context, int16_t x, int16_t y, uint16_t style, uint16_t scale);
void ft_cmd_stop(Vector* context);
void ft_cmd_screensaver(Vector* context);
void ft_cmd_sketch(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   uint32_t ptr, uint16_t format);
void ft_cmd_csketch(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                    uint32_t ptr, uint16_t format, uint16_t freq);
void ft_cmd_snapshot(Vector* context, uint32_t ptr);
void ft_cmd_snapshot2(Vector* context, uint32_t fmt, uint32_t ptr,
                      int16_t x, int16_t y, int16_t w, int16_t h);
void ft_cmd_logo(Vector* context);
void ft_cmd_getptr(Vector* context, uint32_t result);
void ft_cmd_getprops(Vector* context, uint32_t ptr, uint32_t width, uint32_t height);

// Видео
void ft_cmd_mediafifo(Vector* context, uint32_t ptr, uint32_t size);
void ft_cmd_playvideo(Vector* context, uint32_t opts);
void ft_cmd_videostart(Vector* context);
void ft_cmd_videoframe(Vector* context, uint32_t dst, uint32_t ptr);

// Касание
void ft_cmd_track(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag);

#endif /* FRAMEWORK_INC_FT_CO_PRO_COMMANDS_H_ */
