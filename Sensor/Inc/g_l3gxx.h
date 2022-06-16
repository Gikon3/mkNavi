#ifndef INC_G_L3GXX_H_
#define INC_G_L3GXX_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"
#include "comm_point.h"

#define L3GXX_WHO_AM_I      0x0F
#define L3GXX_CTRL1         0x20
#define L3GXX_CTRL2         0x21
#define L3GXX_CTRL3         0x22
#define L3GXX_CTRL4         0x23
#define L3GXX_CTRL5         0x24
#define L3GXX_REFERENCE     0x25
#define L3GXX_OUT_TEMP      0x26
#define L3GXX_STATUS        0x27
#define L3GXX_OUT_X_L       0x28
#define L3GXX_OUT_X_H       0x29
#define L3GXX_OUT_Y_L       0x2A
#define L3GXX_OUT_Y_H       0x2B
#define L3GXX_OUT_Z_L       0x2C
#define L3GXX_OUT_Z_H       0x2D
#define L3GXX_FIFO_CTRL     0x2E
#define L3GXX_FIFO_SRC      0x2F
#define L3GXX_IG_CFG        0x30
#define L3GXX_IG_SRC        0x31
#define L3GXX_IG_THS_XH     0x32
#define L3GXX_IG_THS_XL     0x33
#define L3GXX_IG_THS_YH     0x34
#define L3GXX_IG_THS_YL     0x35
#define L3GXX_IG_THS_ZH     0x36
#define L3GXX_IG_THS_ZL     0x37
#define L3GXX_IG_DURATION   0x38
#define L3GXX_LOW_ODR       0x39

#define L3GD20H_ID          0xD7
#define L3G4250D_ID         0xD3

typedef enum {
  l3gxx245dps = 0,
  l3gxx500dps = 1,
  l3gxx2000dps = 2
} L3GXX_FullScale;

typedef struct
{
  CommPoint       comm;
  Bool            valid;
  uint8_t         whoami;
  L3GXX_FullScale fullScale;
} L3GXX;

typedef struct
{
  uint8_t xEn : 1;
  uint8_t yEn : 1;
  uint8_t zEn : 1;
  uint8_t pd  : 1;
  uint8_t bw  : 2;
  uint8_t dr  : 2;
} L3GXX_Ctrl1Rg;
typedef struct
{
  uint8_t hpcf    : 4;
  uint8_t hpm     : 2;
  uint8_t lvlEn   : 1;  // Только в L3GD20H
  uint8_t extREn  : 1;  // Только в L3GD20H
} L3GXX_Ctrl2Rg;
typedef struct
{
  uint8_t int2Empty : 1;
  uint8_t int2ORun  : 1;
  uint8_t int2Fth   : 1;  // I2_WTM в L3G4250D
  uint8_t int2DRdy  : 1;
  uint8_t ppOd      : 1;
  uint8_t hLactive  : 1;
  uint8_t int1Boot  : 1;
  uint8_t int1Ig    : 1;
} L3GXX_Ctrl3Rg;
typedef struct
{
  uint8_t sim   : 1;
  uint8_t st    : 2;
  uint8_t imPEn : 1;      // Только в L3GD20H
  uint8_t fs    : 2;
  uint8_t ble   : 1;
  uint8_t bdu   : 1;      // Только в L3GD20H
} L3GXX_Ctrl4Rg;
typedef struct
{
  uint8_t outSel    : 2;
  uint8_t igSel     : 2;
  uint8_t hPEn      : 1;
  uint8_t stopOfFth : 1;  // Только в L3GD20H
  uint8_t fifoEn    : 1;
  uint8_t boot      : 1;
} L3GXX_Ctrl5Rg;
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
} L3GXX_StatRg;
typedef struct
{
  uint8_t fth : 5;
  uint8_t fm  : 3;
} L3GXX_FifoCtrlRg;
typedef struct
{
  uint8_t fss   : 5;
  uint8_t empty : 1;
  uint8_t ovrn  : 1;
  uint8_t fth   : 1;
} L3GXX_FifoSrcRg;
typedef struct
{
  uint8_t xlie  : 1;
  uint8_t xhie  : 1;
  uint8_t ylie  : 1;
  uint8_t yhie  : 1;
  uint8_t zlie  : 1;
  uint8_t zhie  : 1;
  uint8_t lir   : 1;
  uint8_t andOr : 1;
} L3GXX_IgCfgRg;
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
} L3GXX_IgSrcRg;
typedef struct
{
  uint8_t d     : 7;
  uint8_t wait  : 1;
} L3GXX_IgDurRg;
typedef struct
{
  uint8_t lowOdr  : 1;
  uint8_t         : 1;
  uint8_t swRes   : 1;
  uint8_t i2cDis  : 1;
  uint8_t         : 1;
  uint8_t drdyHl  : 1;
  uint8_t         : 2;
} L3GXX_LowOdrRg;

extern L3GXX l3gd20h;
extern L3GXX l3g4250d;

void g_l3gd20h_init();
void g_l3g4250d_init();
void g_l3gxx_config(L3GXX* l3g);
Bool g_l3gxx_is_ready(L3GXX* l3g);
void g_l3gxx_refresh_xyz(L3GXX* l3g);
void g_l3gxx_refresh_temp(L3GXX* l3g);
void g_l3gxx_out_mngt(L3GXX* l3g, Bool en);

#endif /* INC_G_L3GD20H_H_ */
