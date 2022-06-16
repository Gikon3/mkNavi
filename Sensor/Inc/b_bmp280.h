#ifndef INC_B_BMP280_H_
#define INC_B_BMP280_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"
#include "comm_point.h"

#define BMP230_CALIB      0x88
#define BMP230_WHO_AM_I   0xD0
#define BMP230_RESET      0xE0
#define BMP230_STAT       0xF3
#define BMP230_CTRL_MEAS  0xF4
#define BMP230_CONF       0xF5
#define BMP230_PRESS_MSB  0xF7
#define BMP230_PRESS_LSB  0xF8
#define BMP230_PRESS_XLSB 0xF9
#define BMP230_TEMP_MSB   0xFA
#define BMP230_TEMP_LSB   0xFB
#define BMP230_TEMP_XLSB  0xFC

#define BMP230_ID         0x58

typedef struct
{
  CommPoint         comm;
  Bool              valid;
  uint8_t           whoami;
  struct
  {
    uint16_t  digT1;
    int16_t   digT2;
    int16_t   digT3;
    uint16_t  digP1;
    int16_t   digP2;
    int16_t   digP3;
    int16_t   digP4;
    int16_t   digP5;
    int16_t   digP6;
    int16_t   digP7;
    int16_t   digP8;
    int16_t   digP9;
    int32_t   tFine;
  } calib;
} BMP230;

typedef struct
{
  uint8_t spi3w   : 1;
  uint8_t         : 1;
  uint8_t filter  : 3;
  uint8_t tsb     : 3;
} BMP230_Conf;
typedef struct
{
  uint8_t mode  : 2;
  uint8_t osrsp : 3;
  uint8_t osrst : 3;
} BMP230_CtrlMeas;
typedef struct
{
  uint8_t imUpdate  : 1;
  uint8_t           : 2;
  uint8_t meas      : 1;
  uint8_t           : 4;
} BMP230_Stat;

extern BMP230 bmp230;

void b_bmp230_init();
Bool b_bmp230_is_ready(BMP230* bmp);
void b_bmp230_config(BMP230* bmp);
void b_bmp230_refresh(BMP230* bmp);
void b_bmp230_out_mngt(BMP230* bmp, Bool en);

#endif /* INC_B_BMP280_H_ */
