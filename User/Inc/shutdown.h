#ifndef INC_SHUTDOWN_H_
#define INC_SHUTDOWN_H_

#include "stm32f4xx_hal.h"

typedef enum {
  shdownNone,
  shdownPend,
  shdownTrue
} ShutdownState;

void shutdown();

#endif /* INC_SHUTDOWN_H_ */
