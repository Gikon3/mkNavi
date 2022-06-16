#ifndef INC_VECTOR_DATA_H_
#define INC_VECTOR_DATA_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef struct
{
  util_remove_t remove;
  void          *data;
  size_t        size;
  UtilStat      stat;
  size_t        itemSize;
  size_t        allocSize;
} Vector;

Vector make_vect(size_t size, size_t itemSize);
Vector make_vect_null(size_t itemSize);
Vector make_vect_arr(void const *data, size_t size, size_t itemSize);
void *vect_at(Vector *vect, size_t index, void *item);
void *vect_front(Vector *vect, void *item);
void *vect_back(Vector *vect, void *item);
void *vect_data(Vector const *vect);
Bool vect_empty(Vector const *vect);
size_t vect_size(Vector const *vect);
size_t vect_max_size(Vector const *vect);
size_t vect_item_size(Vector const *vect);
void vect_clear(Vector *vect);
void *vect_release(Vector *vect);
void vect_resize(Vector *vect, size_t size);
void *vect_insert(Vector *vect, size_t index, void const *item);
void vect_erase(Vector *vect, size_t index, void *item);
void *vect_push_back(Vector *vect, void const *item);
void *vect_push_back_arr(Vector *vect, void const *data, size_t size);
void vect_pop_back(Vector *vect, void *item);

#endif /* INC_VECTOR_DATA_H_ */
