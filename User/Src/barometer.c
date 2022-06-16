#include "barometer.h"
#include "i2c.h"
#include <math.h>

float baro_altitude(float press, float temp)
{
  const float M = 0.029;    /* kg/mole  */
  const float g = 9.81;     /* m/s^2    */
  const float R = 8.31;     /* J*mole/K */
  const float P0 = 101325;  /* Pa       */
  float T = 273.15 + temp;     /* K        */
  /*        R * T * ln(p / P0) */
  /* h = - ------------------- */
  /*              M * g        */
  return -R * T * log(press/P0) / M / g;
}
