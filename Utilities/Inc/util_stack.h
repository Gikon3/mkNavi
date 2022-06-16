#ifndef INC_UTIL_STACK_H_
#define INC_UTIL_STACK_H_

#include "stm32f4xx.h"
#include "util_types.h"

struct _Stack;
typedef struct _Stack Stack;

Stack make_stack(size_t itemSize);
void stack_clear(Stack *stack);
Bool stack_empty(Stack const *stack);
size_t stack_size(Stack const *stack);
void* stack_top(Stack *stack, void *item);
void stack_push(Stack *stack, void const *item);
void stack_pop(Stack *stack, void *item);

#endif /* INC_UTIL_STACK_H_ */
