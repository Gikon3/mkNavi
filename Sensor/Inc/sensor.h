#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "stm32f4xx_hal.h"

typedef struct
{
  struct
  {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t temp;
  } raw;
  struct
  {
    float   x;
    float   y;
    float   z;
    float   temp;
  } ripe;
} SensXYZ;
typedef struct
{
  struct
  {
    int32_t val;
    int32_t temp;
  } raw;
  struct
  {
    float   val;
    float   temp;
  } ripe;
} SensVal;

typedef struct
{
  SensXYZ   acc;
  SensXYZ   gyr;
  SensXYZ   mag;
  SensVal   bar;
} Sensor;

extern Sensor sens;

#endif /* INC_SENSOR_H_ */
