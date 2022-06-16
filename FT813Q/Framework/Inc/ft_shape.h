#ifndef FRAMEWORK_INC_FT_DL_CMDS_COMPOSITE_H_
#define FRAMEWORK_INC_FT_DL_CMDS_COMPOSITE_H_

#include "stm32f4xx.h"
#include "util_vector.h"

void ft_shape_point(Vector *cmds, int16_t x, int16_t y);
void ft_shape_line(Vector *cmds, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void ft_shape_rectangle(Vector *cmds, int16_t x, int16_t y, int16_t w, int16_t h);
void ft_shape_rectangle_outline(Vector *cmds, int16_t x, int16_t y, int16_t w, int16_t h);

#endif /* FRAMEWORK_INC_FT_DL_CMDS_COMPOSITE_H_ */
