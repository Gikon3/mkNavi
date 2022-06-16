#ifndef INC_M_LIS3MDL_H_
#define INC_M_LIS3MDL_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"
#include "comm_point.h"

#define LIS3MDL_WHO_AM_I      0x0F
#define LIS3MDL_CTRL_REG1     0x20
#define LIS3MDL_CTRL_REG2     0x21
#define LIS3MDL_CTRL_REG3     0x22
#define LIS3MDL_CTRL_REG4     0x23
#define LIS3MDL_CTRL_REG5     0x24
#define LIS3MDL_STATUS_REG    0x27
#define LIS3MDL_OUT_X_L       0x28
#define LIS3MDL_OUT_X_H       0x29
#define LIS3MDL_OUT_Y_L       0x2A
#define LIS3MDL_OUT_Y_H       0x2B
#define LIS3MDL_OUT_Z_L       0x2C
#define LIS3MDL_OUT_Z_H       0x2D
#define LIS3MDL_TEMP_OUT_L    0x2E
#define LIS3MDL_TEMP_OUT_H    0x2F
#define LIS3MDL_INT_CFG       0x30
#define LIS3MDL_INT_SRC       0x31
#define LIS3MDL_INT_THS_L     0x32
#define LIS3MDL_INT_THS_H     0x33

#define LIS3MDL_ID            0x3D

typedef enum {
  lis3mdl4gauss,
  lis3mdl8gauss,
  lis3mdl12gauss,
  lis3mdl16gauss
} LIS3MDL_FullScale;

typedef struct
{
  CommPoint         comm;
  Bool              valid;
  uint8_t           whoami;
  LIS3MDL_FullScale fullScale;
} LIS3MDL;

typedef struct
{
  uint8_t st      : 1;
  uint8_t fastOdr : 1;
  uint8_t dOut    : 3;
  uint8_t om      : 2;
  uint8_t tempEn  : 1;
} LIS3MDL_Ctrl1Rg;
typedef struct
{
  uint8_t         : 2;
  uint8_t softRst : 1;
  uint8_t reboot  : 1;
  uint8_t         : 1;
  uint8_t fs      : 2;
  uint8_t         : 1;
} LIS3MDL_Ctrl2Rg;
typedef struct
{
  uint8_t md    : 2;
  uint8_t sim   : 1;
  uint8_t       : 2;
  uint8_t lp    : 1;
  uint8_t       : 2;
} LIS3MDL_Ctrl3Rg;
typedef struct
{
  uint8_t     : 1;
  uint8_t ble : 1;
  uint8_t omz : 2;
  uint8_t     : 4;
} LIS3MDL_Ctrl4Rg;
typedef struct
{
  uint8_t           : 6;
  uint8_t bdu       : 1;
  uint8_t fastRead  : 1;
} LIS3MDL_Ctrl5Rg;
typedef struct
{
  uint8_t xda   : 1;
  uint8_t yda   : 1;
  uint8_t zda   : 1;
  uint8_t zyxda : 1;
  uint8_t xor   : 1;
  uint8_t yor   : 1;
  uint8_t zor   : 1;
  uint8_t zyxor : 1;
} LIS3MDL_StatRg;
typedef struct
{
  uint8_t ien   : 1;
  uint8_t lir   : 1;
  uint8_t iea   : 1;
  uint8_t _high : 1;  /* must be set to 1 */
  uint8_t       : 1;
  uint8_t zien  : 1;
  uint8_t yien  : 1;
  uint8_t xien  : 1;
} LIS3MDL_IntCfgRg;
typedef struct
{
  uint8_t interrupt : 1;
  uint8_t mroi      : 1;
  uint8_t nthZ      : 1;
  uint8_t nthY      : 1;
  uint8_t nthX      : 1;
  uint8_t pthZ      : 1;
  uint8_t pthY      : 1;
  uint8_t pthX      : 1;
} LIS3MDL_IntSrcRg;

extern LIS3MDL lis3mdl;

void m_lis3mdl_init();
void m_lis3mdl_config(LIS3MDL* lis);
Bool m_lis3mdl_is_ready(LIS3MDL* lis);
void m_lis3mdl_refresh_xyz(LIS3MDL* lis);
void m_lis3mdl_refresh_temp(LIS3MDL* lis);
void m_lis3mdl_out_xyz_mngt(LIS3MDL* lis, Bool en);
void m_lis3mdl_out_temp_mngt(LIS3MDL* lis, Bool en);

#endif /* INC_M_LIS3MDL_H_ */
