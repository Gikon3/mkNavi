#include <string.h>
#include "util_types.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_heap.h"

typedef struct
{
  uint32_t  addr;
  size_t    size;
} FT_HeapCell;

void ft_heap_init(FT_Heap *heap, FT_Host *host)
{
  heap->host = host;
  heap->free = make_dlist(sizeof(FT_HeapCell));
  FT_HeapCell cell = {
      .addr = FT_HEAP_START,
      .size = FT_HEAP_SIZE
  };
  dlist_push_back(&heap->free, &cell);
  heap->busy = make_dlist(sizeof(FT_HeapCell));
}

void ft_heap_deinit(FT_Heap *heap)
{
  dlist_clear(&heap->free);
  dlist_clear(&heap->busy);
}

FT_HeapStat ft_heap_clip(FT_Heap *heap, size_t size, uint32_t *addr)
{
  // Выравнивание выделяемой памяти по sizeof(uint32_t)
  const size_t needSpace = (size + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);

  // Поиск свободного места
  DListContainer *freeContainer = dlist_front(&heap->free);
  while(freeContainer && ((FT_HeapCell *)dlist_item(freeContainer))->size < needSpace)
    freeContainer = dlist_next(freeContainer);
  FT_HeapCell *freeCell = dlist_item(freeContainer);
  if(!freeContainer || freeCell->size < needSpace) {
    *addr = 0;
    return ftHeapNotEnoughSpace;
  }

  const uint32_t idAddr = freeCell->addr;
  freeCell->size -= needSpace;
  if(freeCell->size > 0)
    freeCell->addr += needSpace;
  else
    dlist_erase(&heap->free, freeContainer, NULL);

  // Резервирование найденного участка с сортировкой по адресу
  DListContainer *busyContainer = dlist_front(&heap->busy);
  while(busyContainer && ((FT_HeapCell *)dlist_item(busyContainer))->addr < idAddr)
    busyContainer = dlist_next(busyContainer);

  const FT_HeapCell insertCell = {
      .addr = idAddr,
      .size = needSpace
  };
  if(busyContainer)
    dlist_insert(&heap->busy, busyContainer, &insertCell);
  else
    dlist_push_back(&heap->busy, &insertCell);

  *addr = idAddr;
  return ftHeapOk;
}

FT_HeapStat ft_heap_append_own(FT_Heap *heap, Vector *data, FT_HeapParam *param)
{
  // Выравнивание выделяемой памяти по sizeof(uint32_t)
  const size_t needSpace = (preparing_to_cpy_from_dl(heap->host, data) + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);

  const FT_HeapStat status = ft_heap_clip(heap, needSpace, &param->addr);
  if(status != ftHeapOk) {
    vect_clear(data);
    param->addr = 0;
    param->size = 0;
    return status;
  }
  param->size = needSpace;

  // Запись в память
  ft_hal_cpy_from_dl(heap->host, param->addr, param->size);

  return ftHeapOk;
}

FT_HeapStat ft_heap_append(FT_Heap *heap, void const *data, size_t size, FT_HeapParam *param)
{
  Vector newData = make_vect_arr(data, size, sizeof(uint8_t));
  return ft_heap_append_own(heap, &newData, param);
}

FT_HeapStat ft_heap_remove(FT_Heap *heap, uint32_t addr)
{
  // Поиск занятой ячейки с заданным ID
  DListContainer *busyContainer = dlist_front(&heap->busy);
  while(busyContainer && ((FT_HeapCell *)dlist_item(busyContainer))->addr != addr)
    busyContainer = dlist_next(busyContainer);
  if(!busyContainer)
    return ftHeapInvalidID;
  FT_HeapCell *busyCell = dlist_item(busyContainer);

  // Поиск окна, где должна оказаться ячейка после освобождения
  DListContainer *freeContainer = dlist_front(&heap->free);
  while(freeContainer && ((FT_HeapCell *)dlist_item(freeContainer))->addr < addr)
    freeContainer = dlist_next(freeContainer);
  FT_HeapCell *freeCell = dlist_item(freeContainer);

  if(freeCell && (busyCell->addr+busyCell->size) == freeCell->addr) {
    // Соединение освобожденной ячейки со свободной, если они соседние
    freeCell->addr = busyCell->addr;
    freeCell->size += busyCell->size;

    // Объединение с предыдущей свободной ячейкой
    DListContainer *prevContainer = dlist_prev(freeContainer);
    FT_HeapCell *prevCell = dlist_item(prevContainer);
    if(prevCell && (prevCell->addr+prevCell->size) == freeCell->addr) {
      prevCell->size += freeCell->size;
      dlist_erase(&heap->free, prevContainer, NULL);
    }
  }
  else if(freeContainer) {
    // Создание новой свободной ячейки, если нет по соседству свободной
    const FT_HeapCell insertCell = {
        .addr = addr,
        .size = busyCell->size
    };
    dlist_insert(&heap->free, freeContainer, &insertCell);
  }
  else {
    // Создание новой свободной ячейки в конце или объединение с последней свободной ячейкой
    FT_HeapCell *lastCell = dlist_item(dlist_back(&heap->free));
    if((lastCell && (lastCell->addr+lastCell->size) != addr) || dlist_empty(&heap->free) == bTrue) {
      const FT_HeapCell insertCell = {
          .addr = addr,
          .size = busyCell->size
      };
      dlist_push_back(&heap->free, &insertCell);
    }
    else
      lastCell->size += busyCell->size;
  }
  dlist_erase(&heap->busy, busyContainer, NULL);

  return ftHeapOk;
}
