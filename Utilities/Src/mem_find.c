#include "mem_find.h"

char* mem_find(char const* str, size_t lenStr, char const* subStr, size_t lenSubStr)
{
  for (size_t i = 0, j = 0; i < lenStr; ++i) {
    if ((i + lenSubStr) > lenStr) return NULL;
    for (j = 0; j < lenSubStr; ++j) {
      if (str[i + j] != subStr[j]) break;
    }
    if (j == lenSubStr) return (char*)&str[i + lenSubStr];
  }
  return NULL;
}

char* mem_find_circ(char const* buffer, size_t sizeBuf,
                    char const* str, size_t lenStr,
                    char const* subStr, size_t lenSubStr)
{
  size_t index = str - buffer;
  for (size_t i = 0, j = 0; i < lenStr; ++i) {
    if ((i + lenSubStr) > lenStr) return NULL;
    for (j = 0; j < lenSubStr; ++j) {
      const size_t nextIndex = index + j;
      const size_t findIndex = (nextIndex < sizeBuf) ? nextIndex : nextIndex - sizeBuf;
      if (buffer[findIndex] != subStr[j]) break;
    }
    if (j == lenSubStr) {
      const size_t endIndex = index + lenSubStr;
      if (endIndex < sizeBuf) return (char*)&buffer[endIndex];
      return (char*)&buffer[endIndex - sizeBuf];
    }
    ++index;
    if (index >= sizeBuf) index = 0;
  }
  return NULL;
}
