#ifndef FRAMEWORK_INC_FT_IMAGE_IMPL_H_
#define FRAMEWORK_INC_FT_IMAGE_IMPL_H_

#include "stm32f4xx.h"
#include "ft_utils_impl.h"
#include "ft_dl_cmds_impl.h"

typedef struct
{
  uint8_t const *bin;
  size_t        size;
  FT_Format     format;
  uint16_t      stride;
  FT_Dimensions resolution;
} FT_Picture;

typedef enum {
  ftImageNone,
  ftImageLoad,
  ftImageTake
} FT_ImageState;

typedef struct
{
  FT_Widget         *parent;
  FT_Picture const  *picture;
  uint32_t          addr;
  int8_t            handle;
  FT_ImageState     state;
  FT_Filter         filter;
  struct
  {
    FT_Wrap         x;
    FT_Wrap         y;
  } wrap;
  FT_Color          color;
  FT_Coord          coord;
} FT_Image;

#endif /* FRAMEWORK_INC_FT_IMAGE_IMPL_H_ */
