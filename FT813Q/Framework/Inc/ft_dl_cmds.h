#ifndef FRAMEWORK_INC_FT_DL_COMMANDS_H_
#define FRAMEWORK_INC_FT_DL_COMMANDS_H_

#include "stm32f4xx.h"
#include "util_types.h"
#include "ft_dl_cmds_impl.h"

static inline uint32_t ft_dl_alpha_func(FT_Function func, uint8_t ref) { return 0x09 << 24 | func << 10 | ref; }
static inline uint32_t ft_dl_begin(FT_Primitive prim) { return 0x1F << 24 | prim; }
static inline uint32_t ft_dl_bitmap_handle(uint16_t handle) { return 0x05 << 24 | (handle & 0x1F); }
static inline uint32_t ft_dl_bitmap_layout(FT_Format format, uint16_t linestride, uint16_t height) {
  return 0x07 << 24 | format << 19 | (linestride & 0x3FF) << 9 | (height & 0x1FF);
}
static inline uint32_t ft_dl_bitmap_layout_h(uint8_t linestride, uint8_t height) { return 0x28 << 24 | (linestride & 0x3) << 2 | (height & 0x3); }
static inline uint32_t ft_dl_bitmap_size(FT_Filter filter, FT_Wrap wrapX, FT_Wrap wrapY, uint16_t width, uint16_t height) {
  return 0x08 << 24 | filter << 20 | wrapX << 19 | wrapY << 18 | (width & 0x1F) | (height & 0x1FF);
}
static inline uint32_t ft_dl_bitmap_size_h(uint8_t width, uint8_t height) { return 0x29 << 24 | (width & 0x3) << 2 | (height & 0x3); }
static inline uint32_t ft_dl_bitmap_source(uint32_t addr) { return 0x01 << 24 | (addr & 0x1FFFFF); }
static inline uint32_t ft_dl_bitmap_transform_a(uint32_t a) { return 0x15 << 24 | (a & 0x1FFFF); }
static inline uint32_t ft_dl_bitmap_transform_b(uint32_t b) { return 0x16 << 24 | (b & 0x1FFFF); }
static inline uint32_t ft_dl_bitmap_transform_c(uint32_t c) { return 0x17 << 24 | (c & 0xFFFFFF); }
static inline uint32_t ft_dl_bitmap_transform_d(uint32_t d) { return 0x18 << 24 | (d & 0x1FFFF); }
static inline uint32_t ft_dl_bitmap_transform_e(uint32_t e) { return 0x19 << 24 | (e & 0x1FFFF); }
static inline uint32_t ft_dl_bitmap_transform_f(uint32_t f) { return 0x1A << 24 | (f & 0xFFFFFF); }
static inline uint32_t ft_dl_blend_func(FT_Blend src, FT_Blend dst) { return 0x0B << 24 | src << 3 | dst; }
static inline uint32_t ft_dl_call(uint16_t dest) { return 0x1D << 24 | dest; }
static inline uint32_t ft_dl_cell(uint8_t cell) { return 0x06 << 24 | (cell & 0x7F); }
static inline uint32_t ft_dl_clear(Bool c, Bool s, Bool t) { return 0x26 << 24 | c << 2 | s << 1 | t; }
static inline uint32_t ft_dl_clear_color_a(uint8_t alpha) { return 0x0F << 24 | alpha; }
static inline uint32_t ft_dl_clear_color_rgb(uint8_t red, uint8_t blue, uint8_t green) { return 0x02 << 24 | red << 16 | blue << 8 | green; }
static inline uint32_t ft_dl_clear_stencil(uint8_t s) { return 0x11 << 24 | s; }
static inline uint32_t ft_dl_clear_tag(uint8_t t) { return 0x12 << 24 | t; }
static inline uint32_t ft_dl_color_a(uint8_t alpha) { return 0x10 << 24 | alpha; }
static inline uint32_t ft_dl_color_mask(Bool r, Bool g, Bool b, Bool a) { return 0x20 << 24 | r << 3 | g << 2 | b << 1 | a; }
static inline uint32_t ft_dl_color_rgb(uint8_t red, uint8_t blue, uint8_t green) { return 0x04 << 24 | red << 16 | blue << 8 | green; }
static inline uint32_t ft_dl_display() { return 0; }
static inline uint32_t ft_dl_end() { return 0x21 << 24; }
static inline uint32_t ft_dl_jump(uint16_t dest) { return 0x1E << 24 | dest; }
static inline uint32_t ft_dl_line_width(uint16_t width) { return 0x0E << 24 | (width & 0xFFF); }
static inline uint32_t ft_dl_macro(FT_MacroReg m) { return 0x25 << 24 | m; }
static inline uint32_t ft_dl_nop() { return 0x2D << 24; }
static inline uint32_t ft_dl_palette_source(uint32_t addr) { return 0x2A << 24 | (addr & 0x3FFFFF); }
static inline uint32_t ft_dl_point_size(uint16_t size) { return 0x0D << 24 | (size & 0x1FFF); }
static inline uint32_t ft_dl_restore_context() { return 0x23 << 24; }
static inline uint32_t ft_dl_return() { return 0x24 << 24; }
static inline uint32_t ft_dl_save_context() { return 0x22 << 24; }
static inline uint32_t ft_dl_scissor_size(uint16_t width, uint16_t height) { return 0x1C << 24 | (width & 0xFFF) << 12 | (height & 0xFFF); }
static inline uint32_t ft_dl_scissor_xy(uint16_t x, uint16_t y) { return 0x1B << 24 | (x & 0x7FF) << 11 | (y & 0x7FF); }
static inline uint32_t ft_dl_stencil_func(FT_Function func, uint8_t ref, uint8_t mask) { return 0x0A << 24 | func << 16 | ref << 8 | mask; }
static inline uint32_t ft_dl_stencil_mask(uint8_t mask) { return 0x13 << 24 | mask; }
static inline uint32_t ft_dl_stencil_op(FT_Action sfail, FT_Action spass) { return 0x0C << 24 | sfail << 3 | spass; }
static inline uint32_t ft_dl_tag(uint8_t s) { return 0x03 << 24 | s; }
static inline uint32_t ft_dl_tag_mask(Bool mask) { return 0x14 << 24 | mask; }
static inline uint32_t ft_dl_vertex2f(uint16_t x, uint16_t y) { return 0x1 << 30 | (x & 0x7FFF) << 15 | (y & 0x7FFF); }
static inline uint32_t ft_dl_vertex2ii(uint16_t x, uint16_t y, uint8_t handle, uint8_t cell) {
  return 0x2 << 30 | (x & 0x1FF) << 21 | (y & 0x1FF) << 12 | (handle & 0x1F) << 7 | (cell & 0x7F);
}
static inline uint32_t ft_dl_vertex_format(uint8_t frac) { return 0x27 << 24 | (frac & 0x7); }
static inline uint32_t ft_dl_vertex_translate_x(uint32_t x) { return 0x2B << 24 | (x & 0x1FFFF); }
static inline uint32_t ft_dl_vertex_translate_y(uint32_t y) { return 0x2C << 24 | (y & 0x1FFFF); }

#endif /* FRAMEWORK_INC_FT_DL_COMMANDS_H_ */
