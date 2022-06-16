#include "util_dlist.h"
#include "util_heap.h"
#include <string.h>

static DListContainer *create_container(DList *dlist, void const *item, DListContainer *prev, DListContainer *next)
{
  DListContainer *cont = util_malloc(sizeof(DListContainer)+dlist->itemSize);
  if(!cont) {
    dlist->stat = utilMallocFail;
    return cont;
  }
  cont->prev = prev;
  cont->next = next;
  memcpy(dlist_item(cont), item, dlist->itemSize);
  return cont;
}

DList make_dlist(size_t itemSize)
{
  DList dlist = {
      .remove = (util_remove_t)dlist_clear,
      .head = NULL,
      .tail = NULL,
      .size = 0,
      .stat = utilOK,
      .itemSize = itemSize
  };
  return dlist;
}

DListContainer *dlist_front(DList *dlist)
{
  return dlist->head;
}

DListContainer *dlist_back(DList *dlist)
{
  return dlist->tail;
}

Bool dlist_empty(DList const *dlist)
{
  return dlist->size == 0 ? bTrue : bFalse;
}

size_t dlist_size(DList const *dlist)
{
  return dlist->size;
}

void *dlist_item(DListContainer *cont)
{
  if(cont)
    return cont + 1;
  return NULL;
}

DListContainer *dlist_prev(DListContainer const *cont)
{
  return cont->prev;
}

DListContainer *dlist_next(DListContainer const *cont)
{
  return cont->next;
}

void dlist_clear(DList *dlist)
{
  DListContainer *cont = dlist->head;
  while(cont) {
    DListContainer *freeCont = cont;
    cont = cont->next;
    util_free(freeCont);
  }
  dlist->head = NULL;
  dlist->tail = NULL;
  dlist->size = 0;
  dlist->stat = utilOK;
}

DListContainer *dlist_insert(DList *dlist, DListContainer *cont, void const *item)
{
  if(!cont || !item) {
    dlist->stat = utilNullPtrInArg;
    return NULL;
  }
  if(cont == dlist->head)
    return dlist_push_front(dlist, item);

  DListContainer *insertCont = create_container(dlist, item, cont->prev, cont);
  if(insertCont) {
    insertCont->prev->next = insertCont;
    cont->prev = insertCont;
    ++dlist->size;
    dlist->stat = utilOK;
  }
  return insertCont;
}

void dlist_erase(DList *dlist, DListContainer *cont, void *item)
{
  if(!cont) {
    dlist->stat = utilNullPtrInArg;
    return;
  }
  if(cont == dlist->head) {
    dlist_pop_front(dlist, item);
    return;
  }
  if(cont == dlist->tail) {
    dlist_pop_back(dlist, item);
    return;
  }

  cont->prev->next = cont->next;
  cont->next->prev = cont->prev;
  --dlist->size;
  if(item)
    memcpy(item, dlist_item(cont), dlist->itemSize);
  util_free(cont);
  dlist->stat = utilOK;
}

DListContainer *dlist_push_back(DList *dlist, void const *item)
{
  if(!item) {
    dlist->stat = utilNullPtrInArg;
    return NULL;
  }
  DListContainer *insertCont = create_container(dlist, item, dlist->tail, NULL);
  if(insertCont) {
    if(dlist->size == 0)
      dlist->head = insertCont;
    else
      dlist->tail->next = insertCont;
    dlist->tail = insertCont;
    ++dlist->size;
    dlist->stat = utilOK;
  }
  return insertCont;
}

void dlist_pop_back(DList *dlist, void *item)
{
  if(dlist->size == 0) {
    dlist->stat = utilEmpty;
    return;
  }
  DListContainer *freeCont = dlist->tail;
  if(dlist->size > 1)
    freeCont->prev->next = NULL;
  else
    dlist->head = NULL;
  dlist->tail = freeCont->prev;
  --dlist->size;
  if(item)
    memcpy(item, dlist_item(freeCont), dlist->itemSize);
  util_free(freeCont);
  dlist->stat = utilOK;
}

DListContainer *dlist_push_front(DList *dlist, void const *item)
{
  if(!item) {
    dlist->stat = utilNullPtrInArg;
    return NULL;
  }
  DListContainer *insertCont = create_container(dlist, item, NULL, dlist->head);
  if(insertCont) {
    if(dlist->size == 0)
      dlist->tail = insertCont;
    else
      dlist->head->prev = insertCont;
    dlist->head = insertCont;
    ++dlist->size;
    dlist->stat = utilOK;
  }
  return insertCont;
}

void dlist_pop_front(DList *dlist, void *item)
{
  if(dlist->size == 0) {
    dlist->stat = utilEmpty;
    return;
  }
  DListContainer *freeCont = dlist->head;
  if(dlist->size > 1)
    freeCont->next->prev = NULL;
  else
    dlist->tail = NULL;
  dlist->head = freeCont->next;
  --dlist->size;
  if(item)
    memcpy(item, dlist_item(freeCont), dlist->itemSize);
  util_free(freeCont);
  dlist->stat = utilOK;
}
