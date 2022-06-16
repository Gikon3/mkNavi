#include <string.h>
#include "ft_utils.h"
#include "cmsis_os.h"

#define FONT_OFFSET       16  /* Номер шрифта попадает на начало таблицы */
#define FONT_BEGIN_OFFSET 10  /* Отсекает шрифты 16 - 25 */

void* (*ft_util_malloc)(size_t) = pvPortMalloc;
void (*ft_util_free)(void*) = vPortFree;

uint8_t ft_util_font_height(FT_Font const *fontTable, int16_t font)
{
  const int16_t id = font - FONT_OFFSET;
  return fontTable->height[id];
}

static char* cut_front(FT_Font const *fontTable, int16_t font, String const *str, int32_t wLimit, int32_t hLimit)
{
  const int16_t id = font - FONT_OFFSET;
  int32_t charNumber = 0;
  int32_t wAccum = 0;
  if(font >= 26) {
    uint8_t const *width = fontTable->width[id-FONT_BEGIN_OFFSET];
    for(charNumber = str->len - 1; charNumber >= 0; --charNumber) {
      wAccum += width[str->data[charNumber]-' '];
      if(wAccum > wLimit)
        break;
    }
  }
  else {
    const uint8_t maxWidth = fontTable->widthMax[id];
    for(charNumber = str->len - 1; charNumber >= 0; --charNumber) {
      wAccum += maxWidth;
      if(wAccum > wLimit)
        break;
    }
  }

  const size_t len = str->len - charNumber + 1;
  char *cutStr = ft_util_malloc(len+1);
  strcpy(cutStr, &str->data[charNumber+1]);
  return cutStr;
}

static char* cut_back(FT_Font const *fontTable, int16_t font, String const *str, int32_t wLimit, int32_t hLimit)
{
  const int16_t id = font - FONT_OFFSET;
  int32_t wAccum = 0;
  char *charPoint = str->data;
  size_t len = str->len;
  if(font >= 26) {
    uint8_t const *width = fontTable->width[id-FONT_BEGIN_OFFSET];
    for(; *charPoint != '\0'; ++charPoint) {
      wAccum += width[*charPoint-' '];
      if(wAccum > wLimit)
        break;
      --len;
    }
  }
  else {
    const uint8_t maxWidth = fontTable->widthMax[id];
    for(; *charPoint != '\0'; ++charPoint) {
      wAccum += maxWidth;
      if(wAccum > wLimit)
        break;
      --len;
    }
  }

  const size_t cutLen = str->len - len;
  char *cutStr = ft_util_malloc(cutLen+1);
  strncpy(cutStr, str->data, cutLen);
  cutStr[cutLen] = '\0';
  return cutStr;
}

char* ft_util_text_cut(FT_Font const *fontTable, int16_t font, String const *str, FT_TextCut cut, int32_t wLimit, int32_t hLimit)
{
  if(fontTable->height[font-FONT_OFFSET] > hLimit)
    return NULL;

  switch(cut) {
    case ftTextCutFront: return cut_front(fontTable, font, str, wLimit, hLimit);
    case ftTextCutBack: return cut_back(fontTable, font, str, wLimit, hLimit);
    default: return NULL;
  }
}

char* ft_util_text_cut_edges(FT_Font const *fontTable, int16_t font, char const *str, uint16_t *startPos, uint16_t pos,
    uint32_t *endPos, uint32_t wLimit, uint32_t hLimit)
{
  if(*startPos > pos)
    *startPos = pos;
  const int16_t id = font - FONT_OFFSET;
  uint32_t wAccum = 0;
  uint32_t endPosLocal = *startPos;
  if(font >= 26) {
    uint8_t const *width = fontTable->width[id-FONT_BEGIN_OFFSET];
    for(; str[endPosLocal] != '\0'; ++endPosLocal) {
      wAccum += width[str[endPosLocal]-' '];
      if(wAccum > wLimit) {
        wAccum -= width[str[*startPos]-' '];
        if(endPosLocal > pos) {
          --endPosLocal;
          *startPos = endPosLocal == pos ? *startPos + 1 : *startPos;
          break;
        }
        ++*startPos;
      }
    }
  }
  else {
    const uint8_t maxWidth = fontTable->widthMax[id];
    for(; str[endPosLocal] != '\0'; ++endPosLocal) {
      wAccum += maxWidth;
      if(wAccum > wLimit) {
        wAccum -= maxWidth;
        if(endPosLocal > pos) {
          --endPosLocal;
          *startPos = endPosLocal == pos ? *startPos + 1 : *startPos;
          break;
        }
        ++*startPos;
      }
    }
  }

  if(endPos)
    *endPos = endPosLocal;
  endPosLocal = str[endPosLocal] == '\0' ? endPosLocal - 1 : endPosLocal;
  const size_t cutLen = endPosLocal - *startPos + 1;
  char *cutStr = ft_util_malloc(cutLen+1);
  memcpy(cutStr, &str[*startPos], cutLen);
  cutStr[cutLen] = '\0';
  return cutStr;
}

uint32_t ft_util_text_width(FT_Font const *fontTable, int16_t font, char const *str, uint32_t position)
{
  const int16_t id = font - FONT_OFFSET;
  uint32_t wAccum = 0;
  uint32_t count = 0;
  char const *charPoint = str;
  if(font >= 26) {
    uint8_t const *width = fontTable->width[id-FONT_BEGIN_OFFSET];
    for(; *charPoint != '\0'; ++charPoint) {
      if(count == position)
        break;
      wAccum += width[*charPoint-' '];
      ++count;
    }
  }
  else {
    const uint8_t maxWidth = fontTable->widthMax[id];
    for(; *charPoint != '\0'; ++charPoint) {
      if(count == position)
        break;
      wAccum += maxWidth;
      ++count;
    }
  }

  return wAccum;
}

uint32_t ft_util_text_find_pos(FT_Font const *fontTable, int16_t font, char const *str, uint32_t w)
{
  const int16_t id = font - FONT_OFFSET;
  uint32_t count = 0;
  uint32_t wAccum = 0;
  if(font >= 26) {
    uint8_t const *width = fontTable->width[id-FONT_BEGIN_OFFSET];
    for(; *str != '\0' && wAccum < w; ++str) {
      if((wAccum+width[*str-' ']/2) > w)
        break;
      wAccum += width[*str-' '];
      ++count;
    }
  }
  else {
    const uint8_t maxWidth = fontTable->widthMax[id];
    for(; *str != '\0' && w < wAccum; ++str) {
      if((wAccum+maxWidth/2) > w)
        break;
      wAccum += maxWidth;
      ++count;
    }
  }

  return count;
}
