#ifndef INC_COMPASS_H_
#define INC_COMPASS_H_

#include "stm32f4xx.h"

typedef struct
{
  double roll;
  double pitch;
  double yaw;
} Orientation;

Orientation const* compass();

#endif /* INC_COMPASS_H_ */
