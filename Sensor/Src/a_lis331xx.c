#include "a_lis331xx.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"
#include "g_l3gxx.h"

LIS331XX  lis331hh;
LIS331XX  lis331dlh;

void a_lis331xx_wr(LIS331XX* lis, uint8_t address, uint8_t* data, uint16_t size);
void a_lis331xx_rd(LIS331XX* lis, uint8_t address, uint8_t* data, uint16_t size);
void a_lis331xx_wr_word(LIS331XX* lis, uint8_t address, uint8_t data);
uint8_t a_lis331xx_rd_word(LIS331XX* lis, uint8_t address);

void a_lis331hh_init()
{
  lis331hh.comm.type = commI2C;
  lis331hh.comm.i2cIF = &i2cIF_LIS331HH;
  lis331hh.valid = bFalse;
  lis331hh.whoami = LIS331HH_ID;
  lis331hh.fullScale = 0; // +-6g
}

void a_lis331dlh_init()
{
  lis331dlh.comm.type = commI2C;
  lis331dlh.comm.i2cIF = &i2cIF_LIS331DLH;
  lis331dlh.valid = bFalse;
  lis331dlh.whoami = LIS331DLH_ID;
  lis331dlh.fullScale = 0; // +-2g
}

Bool a_lis331xx_is_ready(LIS331XX* lis)
{
  return comm_is_ready(&lis->comm, LIS331XX_WHO_AM_I, lis->whoami);
}

void a_lis331xx_config(LIS331XX* lis)
{
  // TODO какой-то костыль на определение набора датчиков
  if (lis == &lis331dlh && l3gd20h.valid == bTrue) return;
  if (a_lis331xx_is_ready(lis) != bTrue) return;
  lis->valid = bTrue;

  LIS331HH_Ctrl1Rg reg1 = {
      .pm = 0,
      .dr = 3,
      .zen = 1,
      .yen = 1,
      .xen = 1
  };
  LIS331HH_Ctrl2Rg reg2 = {
      .boot = 0,
      .hpm = 0,
      .fds = 0,
      .hpen2 = 0,
      .hpen1 = 0,
      .hpcf = 0
  };
  LIS331HH_Ctrl3Rg reg3 = {
      .ihl = 1,
      .ppOd = 0,
      .lir2 = 0,
      .i2Cfg = 0,
      .lir1 = 0,
      .i1Cfg = 0
  };
  LIS331HH_Ctrl4Rg reg4 = {
      .bdu = 0,
      .ble = 0,
      .fs = lis->fullScale,
      .stSign = 0,
      .st = 0,
      .sim = 0
  };

  a_lis331xx_wr_word(lis, LIS331XX_CTRL_REG1, *(uint8_t*)&reg1);
  a_lis331xx_wr_word(lis, LIS331XX_CTRL_REG2, *(uint8_t*)&reg2);
  a_lis331xx_wr_word(lis, LIS331XX_CTRL_REG3, *(uint8_t*)&reg3);
  a_lis331xx_wr_word(lis, LIS331XX_CTRL_REG4, *(uint8_t*)&reg4);
  a_lis331xx_wr_word(lis, LIS331XX_REFERENCE, 0);
  a_lis331xx_rd_word(lis, LIS331XX_HP_FILTER_RESET);

  a_lis331xx_out_mngt(lis, bTrue);
}

void a_lis331xx_wr(LIS331XX* lis, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_send(&lis->comm, 0x80 | address, data, size);
}

void a_lis331xx_rd(LIS331XX* lis, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_recv(&lis->comm, 0x80 | address, data, size);
}

void a_lis331xx_wr_word(LIS331XX* lis, uint8_t address, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  a_lis331xx_wr(lis, address, txData, 1);
}

uint8_t a_lis331xx_rd_word(LIS331XX* lis, uint8_t address)
{
  uint8_t rxData;
  a_lis331xx_rd(lis, address, &rxData, 1);
  return rxData;
}

void a_lis331xx_refresh_xyz(LIS331XX* lis)
{
  if (lis->valid != bTrue) return;
  double fullScaleVal = 0;
  switch (lis->fullScale) {
    case 0: fullScaleVal = lis == &lis331dlh ? 2 : 6; break;
    case 1: fullScaleVal = lis == &lis331dlh ? 4 : 12; break;
    case 3: fullScaleVal = lis == &lis331dlh ? 8 : 24; break;
  }
  uint8_t data[6];
  a_lis331xx_rd(lis, LIS331XX_OUT_X_L, data, sizeof(data));
# ifdef DEVICE_TYPE_ATB
  sens.acc.raw.x = (int16_t)(data[1] << 8 | data[0]);
  sens.acc.raw.y = (int16_t)(data[3] << 8 | data[2]);
  sens.acc.raw.z = (int16_t)(data[5] << 8 | data[4]);
# endif
# ifdef DEVICE_TYPE_ATV
  sens.acc.raw.x = -(int16_t)(data[3] << 8 | data[2]);
  sens.acc.raw.y = (int16_t)(data[1] << 8 | data[0]);
  sens.acc.raw.z = (int16_t)(data[5] << 8 | data[4]);
# endif
  sens.acc.ripe.x = sens.acc.raw.x * fullScaleVal / 0x7FFFL;
  sens.acc.ripe.y = sens.acc.raw.y * fullScaleVal / 0x7FFFL;
  sens.acc.ripe.z = sens.acc.raw.z * fullScaleVal / 0x7FFFL;
}

void a_lis331xx_out_mngt(LIS331XX* lis, Bool en)
{
  if (lis->valid != bTrue) return;
  uint8_t temp = a_lis331xx_rd_word(lis, LIS331XX_CTRL_REG1);
  LIS331HH_Ctrl1Rg reg1 = *(LIS331HH_Ctrl1Rg*)&temp;
  reg1.pm = en;
  a_lis331xx_wr_word(lis, LIS331XX_CTRL_REG1, *(uint8_t*)&reg1);
}
