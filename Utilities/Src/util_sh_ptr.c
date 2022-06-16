#include <string.h>
#include "util_sh_ptr.h"
#include "util_heap.h"
#include "util_types.h"

ShPtr *make_sh_ptr(void *item, size_t itemSize)
{
  ShPtr *ptr = util_malloc(sizeof(ShPtr)+itemSize);
  ptr->count = 1;
  ptr->itemSize = itemSize;
  memcpy(ptr+sizeof(ShPtr), item, itemSize);
  return ptr;
}

void *sh_ptr_get(ShPtr *ptr)
{
  if(!ptr)
    return NULL;
  return ptr + 1;
}

void sh_ptr_incr(ShPtr *ptr)
{
  if(!ptr)
    return;
  ++ptr->count;
}

void sh_ptr_decr(ShPtr *ptr)
{
  if(!ptr)
    return;
  --ptr->count;
  if(ptr->count == 0) {
    // Вызов деструктора объекта
    ((util_remove_t)(ptr + 1))(ptr + 1);
    util_free(ptr);
    ptr = NULL;
  }
}
