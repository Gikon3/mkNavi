#include <string.h>
#include "util_heap.h"
#include "util_stack.h"

struct _Stack
{
  void    *data;
  size_t  itemSize;
  size_t  size;
  size_t  capacity;
};

Stack make_stack(size_t itemSize)
{
  Stack stack = {
      .data = NULL,
      .itemSize = itemSize,
      .size = 0,
      .capacity = 0,
  };
  return stack;
}

void stack_clear(Stack *stack)
{
  util_free(stack->data);
}

Bool stack_empty(Stack const *stack)
{
  return stack->size == 0 ? bTrue : bFalse;
}

size_t stack_size(Stack const *stack)
{
  return stack->size;
}

void* stack_top(Stack *stack, void *item)
{
  uint8_t *pTop = stack->size > 0 ? (uint8_t*)stack->data + stack->itemSize * (stack->size - 1) : NULL;
  if(item && pTop)
    memcpy(item, pTop, stack->itemSize);
  return pTop;
}

static void resize(Stack *stack)
{
  void *data = util_malloc(stack->capacity * stack->itemSize * 2);
  memcpy(data, stack->data, stack->size * stack->itemSize);
  util_free(stack->data);
  stack->data = data;
}

void stack_push(Stack *stack, void const *item)
{
  if(item) {
    if(stack->size == stack->capacity)
      resize(stack);
    memcpy(stack->data + stack->size * stack->itemSize, item, stack->itemSize);
    ++stack->size;
  }
}

void stack_pop(Stack *stack, void *item)
{
  if(stack->size > 0) {
    --stack->size;
    if(item)
      memcpy(item, stack->data + stack->size * stack->itemSize, stack->itemSize);
  }
}
