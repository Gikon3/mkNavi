#ifndef FRAMEWORK_INC_FT_UTILS_IMPL_H_
#define FRAMEWORK_INC_FT_UTILS_IMPL_H_

#include "stm32f4xx.h"

typedef union
{
  struct
  {
    uint8_t alpha;
    uint8_t blue;
    uint8_t green;
    uint8_t red;
  };
  uint32_t  value;
} FT_Color;

typedef struct
{
  int16_t x;
  int16_t y;
} FT_Coord;

typedef struct
{
  int16_t w;
  int16_t h;
} FT_Dimensions;

typedef enum {
  ftTextNoCut,
  ftTextCutFront,
  ftTextCutBack,
  ftTextCutEdges
} FT_TextCut;

typedef struct
{
  uint8_t widthMax[19];   //< В пикселях
  uint8_t height[19];     //< В пикселях
  uint8_t width[9][95];   //< В пикселях; для шрифтов 26 - 34
} FT_Font;

#endif /* FRAMEWORK_INC_FT_UTILS_IMPL_H_ */
