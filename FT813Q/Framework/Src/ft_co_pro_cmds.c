#include "ft_co_pro_cmds.h"

static inline void push_in_vector(Vector* context, uint32_t const* cmds, size_t size)
{
  for(size_t i = 0; i < size; ++i)
    vect_push_back(context, &cmds[i]);
}

static void add_array_to_end(Vector* context, uint8_t const* data, size_t size)
{
  size_t byteOrdinal = 0;
  for(byteOrdinal = 0; byteOrdinal < size; byteOrdinal += 4) {
    const uint32_t word = (uint32_t)data[byteOrdinal+3] << 24 | (uint32_t)data[byteOrdinal+2] << 16 |
        (uint32_t)data[byteOrdinal+1] << 8 | (uint32_t)data[byteOrdinal];
    vect_push_back(context, &word);
  }
  if(byteOrdinal != size) {
    uint32_t lastWord = 0;
    for(size_t i = 0; i < byteOrdinal - size; ++i)
      lastWord |= data[byteOrdinal - 4 + i] << (i * sizeof(data[0]));
    vect_push_back(context, &lastWord);
  }
}

static void add_str_to_end(Vector* context, char const* str)
{
  char const* s = str;
  uint32_t word = 0;
  if(!str || *str == '\0') {
    vect_push_back(context, &word);
    return;
  }

  size_t it = 0;
  for(it = 0; *s != '\0'; ++it) {
    if(it % 4 == 0 && it > 0) {
      vect_push_back(context, &word);
      word = 0;
    }
    word |= (uint32_t)*s++ << ((it % 4) * 8);
  }
  vect_push_back(context, &word);
  word = 0;
  if(it % 4 == 0)
    vect_push_back(context, &word);
}

// Подствление команды списка отображения
void ft_cmd_dl_set(Vector* context, uint32_t dl)
{
  vect_push_back(context, &dl);
}

// Начало и конец списка отображения
void ft_cmd_dlstart(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF00;
  vect_push_back(context, &cmd);
}

void ft_cmd_dlswap(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF01;
  vect_push_back(context, &cmd);
}

// Рисование графических объектов
void ft_cmd_setfont(Vector* context, uint32_t font, uint32_t ptr)
{
  const uint32_t cmds[] = {
      0xFFFFFF2B,
      font,
      ptr
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_setfont2(Vector* context, uint32_t font, uint32_t ptr, uint32_t firstChar)
{
  const uint32_t cmds[] = {
      0xFFFFFF3B,
      font,
      ptr,
      firstChar
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_setscratch(Vector* context, uint32_t handle)
{
  const uint32_t cmds[] = {
      0xFFFFFF3C,
      handle
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_text(Vector* context, int16_t x, int16_t y, int16_t font,
                 uint16_t options, char const* s)
{
  const uint32_t cmds[] = {
      0xFFFFFF0C,
      y << 16 | x,
      options << 16 | font
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  add_str_to_end(context, s);
}

void ft_cmd_button(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t font, uint16_t options, char const* s)
{
  const uint32_t cmds[] = {
      0xFFFFFF0D,
      y << 16 | x,
      h << 16 | w,
      options << 16 | font
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  add_str_to_end(context, s);
}

void ft_cmd_clock(Vector* context, int16_t x, int16_t y, int16_t r, uint16_t options,
                  uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
  const uint32_t cmds[] = {
      0xFFFFFF14,
      y << 16 | x,
      options << 16 | r,
      m << 16 | h,
      ms << 16 | s
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_bgcolor(Vector* context, uint32_t c)
{
  const uint32_t cmds[] = {
      0xFFFFFF09,
      c >> 8
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_fgcolor(Vector* context, uint32_t c)
{
  const uint32_t cmds[] = {
      0xFFFFFF0A,
      c >> 8
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_gradcolor(Vector* context, uint32_t c)
{
  const uint32_t cmds[] = {
      0xFFFFFF34,
      c
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_gauge(Vector* context, int16_t x, int16_t y, int16_t r, uint16_t options,
                  uint16_t major, uint16_t minor, uint16_t value, uint16_t range)
{
  const uint32_t cmds[] = {
      0xFFFFFF13,
      y << 16 | x,
      options << 16 | r,
      minor << 16 | major,
      range << 16 | value
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_gradient(Vector* context, int16_t x0, int16_t y0, uint32_t rgb0,
                     int16_t x1, int16_t y1, uint32_t rgb1)
{
  const uint32_t cmds[] = {
      0xFFFFFF0B,
      y0 << 16 | x0,
      rgb0,
      y1 << 16 | x1,
      rgb1
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_keys(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                 int16_t font, uint16_t options, char const* s)
{
  const uint32_t cmds[] = {
      0xFFFFFF0E,
      y << 16 | x,
      h << 16 | w,
      options << 16 | font
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  add_str_to_end(context, s);
}

void ft_cmd_pregress(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t options, uint16_t value, uint16_t range)
{
  const uint32_t cmds[] = {
      0xFFFFFF0F,
      y << 16 | x,
      h << 16 | w,
      value << 16 | options,
      range
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_scrollbar(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                      uint16_t options, uint16_t value, uint16_t size, uint16_t range)
{
  const uint32_t cmds[] = {
      0xFFFFFF11,
      y << 16 | x,
      h << 16 | w,
      value << 16 | options,
      range << 16 | size
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_slider(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   uint16_t options, uint16_t value, uint16_t range)
{
  const uint32_t cmds[] = {
      0xFFFFFF10,
      y << 16 | x,
      h << 16 | w,
      value << 16 | options,
      range
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_dial(Vector* context, int16_t x, int16_t y, int16_t r,
                 uint16_t options, uint16_t value)
{
  const uint32_t cmds[] = {
      0xFFFFFF2D,
      y << 16 | x,
      options << 16 | r,
      value
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_toggle(Vector* context, int16_t x, int16_t y, int16_t w, int16_t font,
                   uint16_t options, uint16_t state, char const* s)
{
  const uint32_t cmds[] = {
      0xFFFFFF12,
      y << 16 | x,
      font << 16 | w,
      state << 16 | options
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  add_str_to_end(context, s);
}

void ft_cmd_setbase(Vector* context, uint32_t b)
{
  const uint32_t cmds[] = {
      0xFFFFFF38,
      b
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_number(Vector* context, int16_t x, int16_t y, int16_t font,
                   uint16_t options, int32_t n)
{
  const uint32_t cmds[] = {
      0xFFFFFF2E,
      y << 16 | x,
      options << 16 | font,
      n
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Работа с памятью
void ft_cmd_memcrc(Vector* context, uint32_t ptr, uint32_t num, uint32_t result)
{
  const uint32_t cmds[] = {
      0xFFFFFF18,
      ptr,
      num,
      result
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_memzero(Vector* context, uint32_t ptr, uint32_t num)
{
  const uint32_t cmds[] = {
      0xFFFFFF1C,
      ptr,
      num
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_memset(Vector* context, uint32_t ptr, uint32_t value, uint32_t num)
{
  const uint32_t cmds[] = {
      0xFFFFFF1B,
      ptr,
      value,
      num
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_memwrite(Vector* context, uint32_t ptr, uint32_t size, uint8_t const* data)
{
  const uint32_t cmds[] = {
      0xFFFFFF1A,
      ptr,
      size
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  if(data)
    add_array_to_end(context, data, size);
}

void ft_cmd_memcpy(Vector* context, uint32_t dst, uint32_t src, uint32_t num)
{
  const uint32_t cmds[] = {
      0xFFFFFF1D,
      dst,
      src,
      num
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_append(Vector* context, uint32_t ptr, uint32_t num)
{
  const uint32_t cmds[] = {
      0xFFFFFF1E,
      ptr,
      num
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Загрузка данных изображения в RAM_G (FT81X)
void ft_cmd_inflane(Vector* context, uint32_t ptr, uint8_t const *data, size_t size)
{
  const uint32_t cmds[] = {
      0xFFFFFF22,
      ptr
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
  if(data)
    add_array_to_end(context, data, size);
}

void ft_cmd_loadimage(Vector* context, uint32_t ptr, uint32_t options)
{
  const uint32_t cmds[] = {
      0xFFFFFF24,
      ptr,
      options
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Установка матрицы преобразования растрового изображения
void ft_cmd_loadidentity(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF26;
  vect_push_back(context, &cmd);
}

void ft_cmd_translate(Vector* context, int32_t tx, int32_t ty)
{
  const uint32_t cmds[] = {
      0xFFFFFF27,
      tx,
      ty
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_scale(Vector* context, int32_t sx, int32_t sy)
{
  const uint32_t cmds[] = {
      0xFFFFFF28,
      sx,
      sy
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_rotate(Vector* context, int32_t a)
{
  const uint32_t cmds[] = {
      0xFFFFFF29,
      a
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_setmatrix(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF2A;
  vect_push_back(context, &cmd);
}

void ft_cmd_getmatrix(Vector* context, int32_t a, int32_t b, int32_t c,
                      int32_t d, int32_t e, int32_t f)
{
  const uint32_t cmds[] = {
      0xFFFFFF33,
      a,
      b,
      c,
      d,
      e,
      f
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Другое
void ft_cmd_coldstart(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF32;
  vect_push_back(context, &cmd);
}

void ft_cmd_interrupt(Vector* context, uint32_t ms)
{
  const uint32_t cmds[] = {
      0xFFFFFF02,
      ms
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_regread(Vector* context, uint32_t ptr, uint32_t result)
{
  const uint32_t cmds[] = {
      0xFFFFFF19,
      ptr,
      result
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_calibrate(Vector* context, uint32_t result)
{
  const uint32_t cmds[] = {
      0xFFFFFF15,
      result
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_romfont(Vector* context, uint32_t font, uint32_t romslot)
{
  const uint32_t cmds[] = {
      0xFFFFFF3F,
      font,
      romslot
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_setrotate(Vector* context, uint32_t r)
{
  const uint32_t cmds[] = {
      0xFFFFFF36,
      r
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_setbitmap(Vector* context, uint32_t addr, uint16_t fmt, uint16_t w, uint16_t h)
{
  const uint32_t cmds[] = {
      0xFFFFFF43,
      addr,
      w << 16 | fmt,
      h
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_spinner(Vector* context, int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
  const uint32_t cmds[] = {
      0xFFFFFF16,
      y << 16 | x,
      scale << 16 | style
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_stop(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF17;
  vect_push_back(context, &cmd);
}

void ft_cmd_screensaver(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF2F;
  vect_push_back(context, &cmd);
}

void ft_cmd_sketch(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                   uint32_t ptr, uint16_t format)
{
  const uint32_t cmds[] = {
      0xFFFFFF30,
      y << 16 | x,
      h << 16 | w,
      ptr,
      format
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_csketch(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h,
                    uint32_t ptr, uint16_t format, uint16_t freq)
{
  const uint32_t cmds[] = {
      0xFFFFFF35,
      y << 16 | x,
      h << 16 | w,
      ptr,
      freq << 16 | format
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_snapshot(Vector* context, uint32_t ptr)
{
  const uint32_t cmds[] = {
      0xFFFFFF1F,
      ptr
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_snapshot2(Vector* context, uint32_t fmt, uint32_t ptr,
                      int16_t x, int16_t y, int16_t w, int16_t h)
{
  const uint32_t cmds[] = {
      0xFFFFFF37,
      fmt,
      ptr,
      y << 16 | x,
      h << 16 | w
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_logo(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF31;
  vect_push_back(context, &cmd);
}

void ft_cmd_getptr(Vector* context, uint32_t result)
{
  const uint32_t cmds[] = {
      0xFFFFFF23,
      result
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_getprops(Vector* context, uint32_t ptr, uint32_t width, uint32_t height)
{
  const uint32_t cmds[] = {
      0xFFFFFF25,
      ptr,
      width,
      height
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Видео
void ft_cmd_mediafifo(Vector* context, uint32_t ptr, uint32_t size)
{
  const uint32_t cmds[] = {
      0xFFFFFF39,
      ptr,
      size
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_playvideo(Vector* context, uint32_t opts)
{
  const uint32_t cmds[] = {
      0xFFFFFF3A,
      opts
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

void ft_cmd_videostart(Vector* context)
{
  const uint32_t cmd = 0xFFFFFF40;
  vect_push_back(context, &cmd);
}

void ft_cmd_videoframe(Vector* context, uint32_t dst, uint32_t ptr)
{
  const uint32_t cmds[] = {
      0xFFFFFF41,
      dst,
      ptr
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}

// Касание
void ft_cmd_track(Vector* context, int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
  const uint32_t cmds[] = {
      0xFFFFFF2C,
      y << 16 | x,
      h << 16 | w,
      tag
  };
  push_in_vector(context, cmds, sizeof(cmds)/sizeof(cmds[0]));
}
