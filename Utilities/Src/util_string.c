#include "util_string.h"
#include <string.h>
#include "cmsis_os.h"
#include "util_heap.h"

String make_str(char const *str)
{
  const size_t len = strlen(str);
  const size_t allocSize = len + 1;
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = util_malloc(allocSize),
      .len = len,
      .alloc = allocSize
  };
  strcpy(ret.data, str);
  return ret;
}

String make_str_null()
{
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = NULL,
      .len = 0,
      .alloc = 0
  };
  return ret;
}

String make_str_len(size_t len)
{
  const size_t allocSize = len + 1;
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = util_malloc(allocSize),
      .len = 0,
      .alloc = allocSize
  };
  ret.data[0] = '\0';
  return ret;
}

String make_str_arr(char const *begin, char const *end)
{
  const size_t len = end - begin;
  const size_t allocSize = len + 1;
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = util_malloc(allocSize),
      .len = len,
      .alloc = allocSize
  };
  memcpy(ret.data, begin, ret.len);
  ret.data[ret.len] = '\0';
  return ret;
}

String make_str_circ(char const *buffer, size_t size, char const *begin, char const *end)
{
  const size_t len = (end > begin) ? end - begin: size - (begin - end);
  const size_t allocSize = len + 1;
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = util_malloc(allocSize),
      .len = len,
      .alloc = allocSize
  };
  if(end > begin)
    memcpy(ret.data, begin, ret.len);
  else {
    const size_t tailSize = buffer + size - begin;
    const size_t headSize = end - buffer;
    memcpy(ret.data, begin, tailSize);
    memcpy(ret.data + tailSize, buffer, headSize);
  }
  ret.data[ret.len] = '\0';
  return ret;
}

char *str_data(String *str)
{
  return str->data;
}

char const *str_cdata(String const *str)
{
  return str->data;
}

Bool str_empty(String const *str)
{
  return str->len == 0 ? bTrue : bFalse;
}

size_t str_length(String const *str)
{
  return str->len;
}

size_t str_max_lenght(String const *str)
{
  return str->alloc - 1;
}

String str_copy(String const *str)
{
  String ret = {
      .remove = (util_remove_t)str_clear,
      .data = str->data ? util_malloc(str->alloc) : NULL,
      .len = str->len,
      .alloc = str->alloc
  };
  if(ret.data)
    memcpy(ret.data, str->data, str->len+1);
  return ret;
}

void str_resize(String *str, size_t newLen)
{
  if(str->len == newLen)
    return;
  if(!str->data) {
    *str = make_str_len(newLen);
    return;
  }

  const size_t allocSize = newLen + 1;
  String newStr = {
      .remove = (util_remove_t)str_clear,
      .data = util_malloc(allocSize),
      .len = newLen > str->len ? str->len : newLen,
      .alloc = allocSize
  };
  memcpy(newStr.data, str->data, newStr.len);
  newStr.data[newStr.len] = '\0';
  util_free(str->data);
  *str = newStr;
}

void str_clear(String *str)
{
  util_free(str->data);
  str->data = NULL;
  str->alloc = 0;
  str->len = 0;
}

static size_t new_alloc_size(String const *str, size_t needAllocLen)
{
  size_t allocLen = str->alloc == 0 ? 7 : str->alloc > 1 ? str->alloc - 1 : str->alloc;
  while(needAllocLen > allocLen)
    allocLen *= 2;
  return allocLen + 1;
}

char const* str_insert_char(String *str, size_t index, char ch)
{
  if(index > str->len)
    return NULL;
  size_t newAllocSize = str->alloc;
  if(str->alloc == 0 || (str->alloc-1) == str->len)
    newAllocSize = new_alloc_size(str, str->len+1);

  char *data = str->data;
  if(newAllocSize != str->alloc) {
    data = util_malloc(newAllocSize);
    memcpy(data, str->data, index);
  }
  memmove(&data[index+1], &str->data[index], str->len+1-index);
  data[index] = ch;
  str->data = data;
  ++str->len;
  str->alloc = newAllocSize;
  return &data[index];
}

char str_erase_char(String *str, size_t index)
{
  if(index >= str->len)
    return '\0';
  const char ch = str->data[index];
  memcpy(&str->data[index], &str->data[index+1], str->len-index);
  --str->len;
  return ch;
}

static inline void resize_for_append(String *str, size_t needAllocLen)
{
  size_t allocLen = str->alloc > 1 ? str->alloc - 1 : str->alloc;
  while(needAllocLen > allocLen)
    allocLen *= 2;
  if(str->alloc != (allocLen + 1))
    str_resize(str, allocLen);
}

void str_append(String *str, char const *src)
{
  const size_t srcLen = strlen(src);
  if(srcLen == 0)
    return;
  if(!str->data) {
    *str = make_str(src);
    return;
  }
  resize_for_append(str, str->len+srcLen);
  memcpy(&str->data[str->len], src, srcLen+1);
  str->len += srcLen;
}

void str_append_str(String *str, String const *src)
{
  if(src->len == 0 || !src->data)
    return;
  if(!str->data) {
    *str = str_copy(src);
    return;
  }
  resize_for_append(str, str->len+src->len);
  memcpy(&str->data[str->len], src->data, src->len+1);
  str->len += src->len;
}

void str_append_str_own(String *str, String *src)
{
  if(src->len == 0 || !src->data) {
    str_clear(src);
    return;
  }
  if(!str->data) {
    *str = *src;
    *src = make_str_null();
    return;
  }
  str_append_str(str, src);
  str_clear(src);
}

void str_append_arr(String *str, char const *begin, char const *end)
{
  const size_t srcLen = end - begin;
  if(srcLen == 0)
    return;
  if(!str->data) {
    *str = make_str_arr(begin, end);
    return;
  }
  resize_for_append(str, str->len+srcLen);
  memcpy(&str->data[str->len], begin, srcLen);
  str->len += srcLen;
  str->data[str->len] = '\0';
}

void str_append_char(String *str, char src)
{
  if(!str->data) {
    *str = make_str_len(1);
    str->data[0] = src;
    str->data[1] = '\0';
    return;
  }
  const size_t srcLen = 1;
  resize_for_append(str, str->len+srcLen);
  str->data[str->len] = src;
  str->len += srcLen;
  str->data[str->len] = '\0';
}

char str_pop_back(String *str)
{
  if(!str->data || str->len == 0)
    return '\0';
  --str->len;
  char ret = str->data[str->len];
  str->data[str->len] = '\0';
  return ret;
}
