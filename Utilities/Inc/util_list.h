#ifndef INC_LIST_DATA_H_
#define INC_LIST_DATA_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef struct _ListContainer
{
  struct _ListContainer *next;
} ListContainer;

typedef struct
{
  util_remove_t remove;
  ListContainer *head;
  ListContainer *tail;
  size_t        size;
  UtilStat      stat;
  size_t        itemSize;
} List;

List make_list(size_t itemSize);
ListContainer *list_front(List *list);
ListContainer *list_back(List *list);
Bool list_empty(List const *list);
size_t list_size(List const *list);
void *list_item(ListContainer *cont);
ListContainer *list_next(ListContainer const *cont);
void list_clear(List *list);
ListContainer *list_push_back(List *list, void const *item);
ListContainer *list_push_front(List *list, void const *item);
void list_pop_front(List *list, void *item);

#endif /* INC_LIST_DATA_H_ */
