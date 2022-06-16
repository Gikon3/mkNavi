#ifndef FRAMEWORK_INC_FT_HEAP_H_
#define FRAMEWORK_INC_FT_HEAP_H_

#include "stm32f4xx_hal.h"
#include "util_vector.h"
#include "ft_mem_map.h"
#include "ft_host_impl.h"
#include "ft_heap_impl.h"

#define FT_HEAP_START   (FT_RAM_G + 4)
#define FT_HEAP_SIZE    (1024 * 1024 - 4)   /* 1024 KB - 4 B */

typedef enum {
  ftHeapOk,
  ftHeapNotEnoughSpace,
  ftHeapInvalidID
} FT_HeapStat;

void ft_heap_init(FT_Heap *heap, FT_Host *host);
void ft_heap_deinit(FT_Heap *heap);
FT_HeapStat ft_heap_clip(FT_Heap *heap, size_t size, uint32_t *addr);
FT_HeapStat ft_heap_append_own(FT_Heap *heap, Vector *data, FT_HeapParam *param);
FT_HeapStat ft_heap_append(FT_Heap *heap, void const *data, size_t size, FT_HeapParam *param);
FT_HeapStat ft_heap_remove(FT_Heap *heap, uint32_t addr);

#endif /* FRAMEWORK_INC_FT_MEM_H_ */
