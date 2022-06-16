#ifndef INC_UTIL_HEAP_H_
#define INC_UTIL_HEAP_H_

#include "stm32f4xx_hal.h"

extern void* (*util_malloc)(size_t);
extern void (*util_free)(void*);

#endif /* INC_UTIL_HEAP_H_ */
