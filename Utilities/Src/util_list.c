#include "util_list.h"
#include "util_heap.h"
#include <string.h>

static ListContainer *create_container(List *list, void const *item, ListContainer *next)
{
  ListContainer *cont = util_malloc(sizeof(ListContainer)+list->itemSize);
  if(!cont) {
    list->stat = utilMallocFail;
    return cont;
  }
  cont->next = next;
  memcpy(list_item(cont), item, list->itemSize);
  return cont;
}

List make_list(size_t itemSize)
{
  List list = {
      .remove = (util_remove_t)list_clear,
      .head = NULL,
      .tail = NULL,
      .size = 0,
      .stat = utilOK,
      .itemSize = itemSize
  };
  return list;
}

ListContainer *list_front(List *list)
{
  return list->head;
}

ListContainer *list_back(List *list)
{
  return list->tail;
}

Bool list_empty(List const *list)
{
  return list->size == 0 ? bTrue : bFalse;
}

size_t list_size(List const *list)
{
  return list->size;
}

void *list_item(ListContainer *cont)
{
  return cont + 1;
}

ListContainer *list_next(ListContainer const *cont)
{
  return cont->next;
}

void list_clear(List *list)
{
  ListContainer *cont = list->head;
  while(cont) {
    ListContainer *freeCont = cont;
    cont = cont->next;
    util_free(freeCont);
  }
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  list->stat = utilOK;
}

ListContainer *list_push_back(List *list, void const *item)
{
  if(!item) {
    list->stat = utilNullPtrInArg;
    return NULL;
  }
  ListContainer *insertCont = create_container(list, item, NULL);
  if(insertCont) {
    if(list->size == 0)
      list->head = insertCont;
    else
      list->tail->next = insertCont;
    list->tail = insertCont;
    ++list->size;
    list->stat = utilOK;
  }
  return insertCont;
}

ListContainer *list_push_front(List *list, void const *item)
{
  if(!item) {
    list->stat = utilNullPtrInArg;
    return NULL;
  }
  ListContainer *insertCont = create_container(list, item, list->head);
  if(insertCont) {
    if(list->size == 0)
      list->tail = insertCont;
    list->head = insertCont;
    ++list->size;
    list->stat = utilOK;
  }
  return insertCont;
}

void list_pop_front(List *list, void *item)
{
  if(list->size == 0) {
    list->stat = utilEmpty;
    return;
  }
  ListContainer *freeCont = list->head;
  list->head = freeCont->next;
  --list->size;
  if(list->size == 0)
    list->tail = NULL;
  if(item)
    memcpy(item, list_item(freeCont), list->itemSize);
  util_free(freeCont);
  list->stat = utilOK;
}
