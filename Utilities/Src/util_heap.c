#include "util_heap.h"

#include "cmsis_os.h"

void* (*util_malloc)(size_t) = pvPortMalloc;
void (*util_free)(void*) = vPortFree;
