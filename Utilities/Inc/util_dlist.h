#ifndef INC_UTIL_DLIST_H_
#define INC_UTIL_DLIST_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef struct _DListContainer
{
  struct _DListContainer  *prev;
  struct _DListContainer  *next;
} DListContainer;

typedef struct
{
  util_remove_t   remove;
  DListContainer  *head;
  DListContainer  *tail;
  size_t          size;
  UtilStat        stat;
  size_t          itemSize;
} DList;

DList make_dlist(size_t itemSize);
DListContainer *dlist_front(DList *dlist);
DListContainer *dlist_back(DList *dlist);
Bool dlist_empty(DList const *dlist);
size_t dlist_size(DList const *dlist);
void *dlist_item(DListContainer *cont);
DListContainer *dlist_prev(DListContainer const *cont);
DListContainer *dlist_next(DListContainer const *cont);
void dlist_clear(DList *dlist);
DListContainer *dlist_insert(DList *dlist, DListContainer *cont, void const *item);
void dlist_erase(DList *dlist, DListContainer *cont, void *item);
DListContainer *dlist_push_back(DList *dlist, void const *item);
void dlist_pop_back(DList *dlist, void *item);
DListContainer *dlist_push_front(DList *dlist, void const *item);
void dlist_pop_front(DList *dlist, void *item);

#endif /* INC_UTIL_DLIST_H_ */
