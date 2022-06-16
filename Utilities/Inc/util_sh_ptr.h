#ifndef INC_UTIL_SH_PTR_H_
#define INC_UTIL_SH_PTR_H_

#include "stm32f4xx_hal.h"

typedef struct
{
  size_t  count;
  size_t  itemSize;
} ShPtr;

ShPtr *make_sh_ptr(void *item, size_t itemSize);
void *sh_ptr_get(ShPtr *ptr);
void sh_ptr_incr(ShPtr *ptr);
void sh_ptr_decr(ShPtr *ptr);

#endif /* INC_UTIL_SH_PTR_H_ */
