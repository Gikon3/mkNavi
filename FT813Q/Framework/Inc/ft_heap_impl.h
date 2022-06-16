#ifndef FRAMEWORK_INC_FT_HEAP_IMPL_H_
#define FRAMEWORK_INC_FT_HEAP_IMPL_H_

#include "stm32f4xx.h"
#include "util_dlist.h"

struct _FT_Host;

typedef struct _FT_Heap
{
  struct _FT_Host *host;
  DList           free;
  DList           busy;
} FT_Heap;

typedef struct
{
  uint32_t  addr;
  size_t    size;
} FT_HeapParam;

#endif /* FRAMEWORK_INC_FT_HEAP_IMPL_H_ */
