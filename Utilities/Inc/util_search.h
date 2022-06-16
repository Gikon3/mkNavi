#ifndef INC_UTIL_SEARCH_H_
#define INC_UTIL_SEARCH_H_

#include "stm32f4xx.h"
#include "util_string.h"

struct _RingBufferItem;
typedef void (*data_processing_t)(String const *str, void *argument);
typedef data_processing_t (*arbiter_t)(struct _RingBufferItem const *item,
    uint8_t const *beginMessage, uint8_t const *endMessage);

typedef struct _RingBufferItem
{
  uint8_t const *beginBuffer;
  uint8_t const *endBuffer;
  size_t        sizeBuffer;
  uint8_t const *pWrite;
  uint8_t const *pRead;
  uint8_t const *pFind;
  uint8_t const *beginPattern;
  uint8_t const *endPattern;
  arbiter_t     arbiter;
  void          *argument;
} RingBufferItem;

uint8_t const* buffer_find(uint8_t const *beginBuffer, size_t sizeBuffer, uint8_t const *pattern, size_t sizePattern);
uint8_t const* ring_buffer_find(uint8_t const *beginBuffer, uint8_t const *endBuffer,
    uint8_t const *beginPattern, uint8_t const *endPattern,
    uint8_t const *beginFind, size_t findSize);
void ring_buffer_item_init(RingBufferItem *item, uint8_t const *buffer, size_t sizeBuffer,
    uint8_t const *pattern, size_t sizePattern, arbiter_t arbiter, void *argument);
void ring_buffer_increase_write_point(RingBufferItem *item, size_t size);
void ring_buffer_process(RingBufferItem *item);
void util_ring_buffer_copy(uint8_t const *beginBuffer, uint8_t const *endBuffer,
    uint8_t const *beginCopy, size_t sizeCopy, void *destination);

#endif /* INC_UTIL_SEARCH_H_ */
