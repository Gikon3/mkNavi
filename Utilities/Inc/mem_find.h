#ifndef INC_MEM_FIND_H_
#define INC_MEM_FIND_H_

#include "stm32f4xx_hal.h"

char* mem_find(char const* str, size_t lenStr,
               char const* subStr, size_t lenSubStr);
char* mem_find_circ(char const* buffer,  size_t sizeBuf,
                    char const* str, size_t lenStr,
                    char const* subStr, size_t lenSubStr);

#endif /* INC_FIND_H_ */
