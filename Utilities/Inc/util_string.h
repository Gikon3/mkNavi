#ifndef INC_STRING_DATA_H_
#define INC_STRING_DATA_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef struct
{
  util_remove_t remove;
  char          *data;
  size_t        len;
  size_t        alloc;
} String;

String make_str(char const *str);
String make_str_null();
String make_str_len(size_t len);
String make_str_arr(char const *begin, char const *end);
String make_str_circ(char const *buffer, size_t size, char const *begin, char const *end);
char *str_data(String *str);
char const *str_cdata(String const *str);
Bool str_empty(String const *str);
size_t str_length(String const *str);
size_t str_max_lenght(String const *str);
String str_copy(String const *str);
void str_resize(String *str, size_t newLen);
void str_clear(String *str);
char const* str_insert_char(String *str, size_t index, char ch);
char str_erase_char(String *str, size_t index);
void str_append(String *str, char const *src);
void str_append_str(String *str, String const *src);
void str_append_str_own(String *str, String *src);
void str_append_arr(String *str, char const *begin, char const *end);
void str_append_char(String *str, char src);
char str_pop_back(String *str);

#endif /* INC_STRING_DATA_H_ */
