#include "util_types.h"
#include "util_heap.h"
#include "util_search.h"

uint8_t const* buffer_find(uint8_t const *beginBuffer, size_t sizeBuffer, uint8_t const *pattern, size_t sizePattern)
{
  uint8_t const *endPattern = pattern + sizePattern;
  for(size_t i = 0; i < sizeBuffer; ++i) {
    uint8_t const *pFind = beginBuffer++;
    uint8_t const *pPattern = pattern;
    size_t j = i;
    while(j < sizeBuffer && pPattern != endPattern) {
      if(*pFind++ != *pPattern++)
        break;
      ++j;
    }
    if((j - i) == sizePattern && pPattern == endPattern)
      return pFind;
  }
  return NULL;
}

uint8_t const* ring_buffer_find(uint8_t const *beginBuffer, uint8_t const *endBuffer,
    uint8_t const *beginPattern, uint8_t const *endPattern,
    uint8_t const *beginFind, size_t findSize)
{
  const size_t patternSize = endPattern - beginPattern;
  for(size_t i = 0; i < findSize; ++i) {
    uint8_t const *pFind = beginFind++;
    beginFind = beginFind != endBuffer ? beginFind : beginBuffer;
    uint8_t const *pPattern = beginPattern;
    size_t j = i;
    while(j < findSize && pPattern != endPattern) {
      if(pFind == endBuffer)
        pFind = beginBuffer;
      if(*pFind++ != *pPattern++)
        break;
      ++j;
    }
    if((j - i) == patternSize && pPattern == endPattern)
      return pFind != endBuffer ? pFind : beginBuffer;
  }
  return NULL;
}

void ring_buffer_item_init(RingBufferItem *item, uint8_t const *buffer, size_t sizeBuffer,
    uint8_t const *pattern, size_t sizePattern, arbiter_t arbiter, void *argument)
{
  item->beginBuffer = buffer;
  item->endBuffer = buffer + sizeBuffer;
  item->sizeBuffer = sizeBuffer;
  item->pWrite = buffer;
  item->pRead = buffer;
  item->pFind = buffer;
  item->beginPattern = pattern;
  item->endPattern = pattern + sizePattern;
  item->arbiter = arbiter;
  item->argument = argument;
}

void ring_buffer_increase_write_point(RingBufferItem *item, size_t size)
{
  uint8_t const *newPoint = item->pWrite + size;
  item->pWrite = newPoint < item->endBuffer ? newPoint : item->beginBuffer + (newPoint - item->endBuffer);
}

void ring_buffer_process(RingBufferItem *item)
{
  size_t findSize = item->pWrite >= item->pFind ? item->pWrite - item->pFind :
      (item->endBuffer - item->pFind) + (item->pWrite - item->beginBuffer);
  uint8_t const *pPatternEnd = ring_buffer_find(item->beginBuffer, item->endBuffer, item->beginPattern, item->endPattern,
      item->pFind, findSize);

  while(pPatternEnd) {
    findSize = item->pWrite >= item->pFind ? item->pWrite - item->pFind :
        (item->endBuffer - item->pFind) + (item->pWrite - item->beginBuffer);
    pPatternEnd = ring_buffer_find(item->beginBuffer, item->endBuffer, item->beginPattern, item->endPattern,
        item->pFind, findSize);

    data_processing_t data_processing = NULL;
    if(pPatternEnd) {
      data_processing = item->arbiter(item, item->pRead, pPatternEnd);
      item->pFind = pPatternEnd;
    }
    else {
      const size_t patternSize = item->endPattern - item->beginPattern;
      item->pFind += findSize >= patternSize ? findSize - patternSize : 0;
      if(item->pFind >= item->endBuffer)
        item->pFind = item->beginBuffer + (item->pFind - item->endBuffer);
    }

    if(data_processing) {
      String message = make_str_circ((char const*)item->beginBuffer, item->endBuffer-item->beginBuffer,
          (char const*)item->pRead, (char const*)pPatternEnd);
      data_processing(&message, item->argument);
      str_clear(&message);
      item->pRead = pPatternEnd;
    }
  }
}

void util_ring_buffer_copy(uint8_t const *beginBuffer, uint8_t const *endBuffer,
    uint8_t const *beginCopy, size_t sizeCopy, void *destination)
{
  while(beginCopy != endBuffer && sizeCopy > 0) {
    *(uint8_t*)destination++ = *beginCopy++;
    --sizeCopy;
  }
  if(sizeCopy == 0)
    return;
  beginCopy = beginBuffer;
  for(; sizeCopy > 0; --sizeCopy)
    *(uint8_t*)destination++ = *beginCopy++;
}
