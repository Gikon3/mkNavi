#include "util_vector.h"
#include <string.h>
#include "util_heap.h"

Vector make_vect(size_t size, size_t itemSize)
{
  const size_t allocSize = size * itemSize;
  void *const data = util_malloc(allocSize);
  Vector ret = {
      .remove = (util_remove_t)vect_clear,
      .data = data,
      .size = 0,
      .stat = data ? utilOK : utilMallocFail,
      .itemSize = itemSize,
      .allocSize = allocSize
  };
  return ret;
}

Vector make_vect_null(size_t itemSize)
{
  Vector ret = {
      .remove = (util_remove_t)vect_clear,
      .data = NULL,
      .size = 0,
      .stat = utilOK,
      .itemSize = itemSize,
      .allocSize = 0
  };
  return ret;
}

Vector make_vect_arr(void const *data, size_t size, size_t itemSize)
{
  Vector ret = make_vect(size, itemSize);
  if(ret.data) {
    memcpy(ret.data, data, size*ret.itemSize);
    ret.size = size;
  }
  return ret;
}

void *vect_at(Vector *vect, size_t index, void *item)
{
  if(vect->size == 0) {
    vect->stat = utilEmpty;
    return NULL;
  }
  if(index >= vect->size) {
    vect->stat = utilInvalidIndex;
    return NULL;
  }

  const size_t pos = index * vect->itemSize;
  if(item)
    memcpy(item, &((uint8_t *)vect->data)[pos], vect->itemSize);
  vect->stat = utilOK;
  return &((uint8_t *)vect->data)[pos];
}

void *vect_front(Vector *vect, void *item)
{
  if(vect->size == 0) {
    vect->stat = utilEmpty;
    return NULL;
  }

  if(item)
    memcpy(item, vect->data, vect->itemSize);
  vect->stat = utilOK;
  return vect->data;
}

void *vect_back(Vector *vect, void *item)
{
  if(vect->size == 0) {
    vect->stat = utilEmpty;
    return NULL;
  }

  const size_t pos = (vect->size - 1) * vect->itemSize;
  if(item)
    memcpy(item, &((uint8_t *)vect->data)[pos], vect->itemSize);
  vect->stat = utilOK;
  return &((uint8_t *)vect->data)[pos];
}

void *vect_data(Vector const *vect)
{
  return vect->data;
}

Bool vect_empty(Vector const *vect)
{
  return vect->size == 0 ? bTrue : bFalse;
}

size_t vect_size(Vector const *vect)
{
  return vect->size;
}

size_t vect_max_size(Vector const *vect)
{
  return vect->allocSize / vect->itemSize;
}

size_t vect_item_size(Vector const *vect)
{
  return vect->itemSize;
}

void vect_clear(Vector *vect)
{
  util_free(vect->data);
  vect->data = NULL;
  vect->size = 0;
  vect->stat = utilOK;
  vect->allocSize = 0;
}

void *vect_release(Vector *vect)
{
  void *data = vect->data;
  vect->data = NULL;
  vect->size = 0;
  vect->stat = utilOK;
  vect->allocSize = 0;
  return data;
}

void vect_resize(Vector *vect, size_t size)
{
  if(size == 0)
    return;
  const size_t allocSize = size * vect->itemSize;
  void *data = util_malloc(allocSize);
  if(!data) {
    vect->stat = utilMallocFail;
    return;
  }
  const size_t newSize = size < vect->size ? size : vect->size;
  memcpy(data, vect->data, newSize*vect->itemSize);
  util_free(vect->data);
  vect->data = data;
  vect->size = newSize;
  vect->stat = utilOK;
  vect->allocSize = allocSize;
}

static inline size_t new_number_alloc_items(Vector *vect, size_t needAllocItems)
{
  size_t newAllocItems = vect->allocSize / vect->itemSize;
  if(newAllocItems == 0)
    newAllocItems = 1;
  while(needAllocItems > newAllocItems)
    newAllocItems *= 2;
  return newAllocItems;
}

void *vect_insert(Vector *vect, size_t index, void const *item)
{
  if(index == vect->size)
      return vect_push_back(vect, item);
  if(index > vect->size) {
    vect->stat = utilInvalidIndex;
    return NULL;
  }
  if(!item) {
    vect->stat = utilNullPtrInArg;
    return NULL;
  }

  const size_t allocItems = vect->allocSize / vect->itemSize;
  const size_t needAllocItems = vect->size + 1;
  const Bool realloc = allocItems < needAllocItems ? bTrue : bFalse;
  size_t newAllocItems = 0;
  uint8_t *data = vect->data;
  if(realloc == bTrue) {
    newAllocItems = new_number_alloc_items(vect, needAllocItems);
    data = util_malloc(newAllocItems*vect->itemSize);
    if(!data) {
      vect->stat = utilMallocFail;
      return NULL;
    }
  }

  const size_t cpySize0 = index * vect->itemSize;
  const size_t cpySize1 = vect->itemSize;
  const size_t cpySize2 = (vect->size - index) * vect->itemSize;
  memmove(&data[cpySize0+cpySize1], &((uint8_t *)vect->data)[cpySize0], cpySize2);
  memcpy(&data[cpySize0], item, cpySize1);
  if(realloc == bTrue) {
    memcpy(&data[0], vect->data, cpySize0);
    util_free(vect->data);
    vect->data = data;
    vect->allocSize = newAllocItems * vect->itemSize;
  }
  ++vect->size;
  vect->stat = utilOK;
  return &((uint8_t *)vect->data)[cpySize0];
}

void vect_erase(Vector *vect, size_t index, void *item)
{
  if(index == (vect->size-1))
    return vect_pop_back(vect, item);
  if(index >= vect->size) {
    vect->stat = utilInvalidIndex;
    return;
  }

  const size_t pos = index * vect->itemSize;
  if(item)
    memcpy(item, &((uint8_t*)vect->data)[pos], vect->itemSize);
  --vect->size;
  const size_t cpyPos = pos + vect->itemSize;
  const size_t cpySize = (vect->size - index) * vect->itemSize;
  memcpy(&((uint8_t *)vect->data)[pos], &((uint8_t*)vect->data)[cpyPos], cpySize);
  vect->stat = utilOK;
}

void *vect_push_back(Vector *vect, void const *item)
{
  if(!item) {
    vect->stat = utilInvalidIndex;
    return NULL;
  }

  const size_t allocItems = vect->allocSize / vect->itemSize;
  const size_t needAllocItems = vect->size + 1;
  if(allocItems < needAllocItems) {
    const size_t newAllocItems = new_number_alloc_items(vect, needAllocItems);
    vect_resize(vect, newAllocItems);
    if(vect->stat != utilOK)
      return NULL;
  }

  const size_t pos = vect->size * vect->itemSize;
  memcpy(&((uint8_t *)vect->data)[pos], item, vect->itemSize);
  ++vect->size;
  vect->stat = utilOK;
  return &((uint8_t *)vect->data)[pos];
}

void *vect_push_back_arr(Vector *vect, void const *data, size_t size)
{
  if(!data) {
    vect->stat = utilInvalidIndex;
    return NULL;
  }

  const size_t allocItems = vect->allocSize / vect->itemSize;
  const size_t needAllocItems = vect->size + size;
  if(allocItems < needAllocItems) {
    const size_t newAllocItems = new_number_alloc_items(vect, needAllocItems);
    vect_resize(vect, newAllocItems);
    if(vect->stat != utilOK)
      return NULL;
  }

  const size_t pos = vect->size * vect->itemSize;
  memcpy(&((uint8_t*)vect->data)[pos], data, size);
  vect->size += size;
  vect->stat = utilOK;
  return &((uint8_t*)vect->data)[pos];
}

void vect_pop_back(Vector *vect, void *item)
{
  if(vect->size == 0) {
    vect->stat = utilEmpty;
    return;
  }

  --vect->size;
  if(item) {
    const size_t pos = vect->size * vect->itemSize;
    memcpy(item, &((uint8_t *)vect->data)[pos], vect->itemSize);
  }
  vect->stat = utilOK;
}
