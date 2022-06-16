#ifndef INC_A_LIS331XX_H_
#define INC_A_LIS331XX_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"
#include "comm_point.h"

#define LIS331XX_WHO_AM_I           0x0F
#define LIS331XX_CTRL_REG1          0x20
#define LIS331XX_CTRL_REG2          0x21
#define LIS331XX_CTRL_REG3          0x22
#define LIS331XX_CTRL_REG4          0x23
#define LIS331XX_CTRL_REG5          0x24
#define LIS331XX_HP_FILTER_RESET    0x25
#define LIS331XX_REFERENCE          0x26
#define LIS331XX_STATUS_REG         0x27
#define LIS331XX_OUT_X_L            0x28
#define LIS331XX_OUT_X_H            0x29
#define LIS331XX_OUT_Y_L            0x2A
#define LIS331XX_OUT_Y_H            0x2B
#define LIS331XX_OUT_Z_L            0x2C
#define LIS331XX_OUT_Z_H            0x2D
#define LIS331XX_INT1_CFG           0x30
#define LIS331XX_INT1_SOURCE        0x31
#define LIS331XX_INT1_THS           0x32
#define LIS331XX_INT1_DURATION      0x33
#define LIS331XX_INT2_CFG           0x34
#define LIS331XX_INT2_SOURCE        0x35
#define LIS331XX_INT2_THS           0x36
#define LIS331XX_INT2_DURATION      0x37

#define LIS331HH_ID                 0x32
#define LIS331DLH_ID                0x32

typedef struct
{
  CommPoint   comm;
  Bool        valid;
  uint8_t     whoami;
  uint8_t     fullScale;
} LIS331XX;

typedef struct
{
  uint8_t xen : 1;
  uint8_t yen : 1;
  uint8_t zen : 1;
  uint8_t dr  : 2;
  uint8_t pm  : 3;
} LIS331HH_Ctrl1Rg;
typedef struct
{
  uint8_t hpcf  : 2;
  uint8_t hpen1 : 1;
  uint8_t hpen2 : 1;
  uint8_t fds   : 1;
  uint8_t hpm   : 2;
  uint8_t boot  : 1;
} LIS331HH_Ctrl2Rg;
typedef struct
{
  uint8_t i1Cfg : 2;
  uint8_t lir1  : 1;
  uint8_t i2Cfg : 2;
  uint8_t lir2  : 1;
  uint8_t ppOd  : 1;
  uint8_t ihl   : 1;
} LIS331HH_Ctrl3Rg;
typedef struct
{
  uint8_t sim     : 1;
  uint8_t st      : 1;
  uint8_t         : 1;
  uint8_t stSign  : 1;
  uint8_t fs      : 2;
  uint8_t ble     : 1;
  uint8_t bdu     : 1;
} LIS331HH_Ctrl4Rg;
typedef struct
{
  uint8_t turnOn  : 2;
  uint8_t         : 6;
} LIS331HH_Ctrl5Rg;
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
} LIS331HH_StatRg;
typedef struct
{
  uint8_t xlie  : 1;
  uint8_t xhie  : 1;
  uint8_t ylie  : 1;
  uint8_t yhie  : 1;
  uint8_t zlie  : 1;
  uint8_t zhie  : 1;
  uint8_t d6    : 1;
  uint8_t aoi   : 1;
} LIS331HH_IntCfgRg;
typedef struct
{
  uint8_t xl  : 1;
  uint8_t xh  : 1;
  uint8_t yl  : 1;
  uint8_t yh  : 1;
  uint8_t zl  : 1;
  uint8_t zh  : 1;
  uint8_t ia  : 1;
  uint8_t     : 1;
} LIS331HH_IntSrcRg;

extern LIS331XX lis331hh;
extern LIS331XX lis331dlh;

void a_lis331hh_init();
void a_lis331dlh_init();
Bool a_lis331xx_is_ready(LIS331XX* lis);
void a_lis331xx_config(LIS331XX* lis);
void a_lis331xx_refresh_xyz(LIS331XX* lis);
void a_lis331xx_out_mngt(LIS331XX* lis, Bool en);

#endif /* INC_A_LIS331XX_H_ */
