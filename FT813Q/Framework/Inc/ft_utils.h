#ifndef FRAMEWORK_INC_FT_UTILS_H_
#define FRAMEWORK_INC_FT_UTILS_H_

#include "util_string.h"
#include "ft_utils_impl.h"

extern void* (*ft_util_malloc)(size_t);
extern void (*ft_util_free)(void*);
uint8_t ft_util_font_height(FT_Font const *fontTable, int16_t font);
char* ft_util_text_cut(FT_Font const *fontTable, int16_t font, String const *str, FT_TextCut cut, int32_t wLimit, int32_t hLimit);
char* ft_util_text_cut_edges(FT_Font const *fontTable, int16_t font, char const *str, uint16_t *startPos, uint16_t pos,
    uint32_t *endPos, uint32_t wLimit, uint32_t hLimit);
uint32_t ft_util_text_width(FT_Font const *fontTable, int16_t font, char const *str, uint32_t position);
uint32_t ft_util_text_find_pos(FT_Font const *fontTable, int16_t font, char const *str, uint32_t w);

#endif /* FRAMEWORK_INC_FT_UTILS_H_ */
