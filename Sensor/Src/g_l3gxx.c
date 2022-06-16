#include "g_l3gxx.h"
#include "i2c.h"
#include "spi.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"

L3GXX l3gd20h;
L3GXX l3g4250d;

void g_l3gxx_wr_word(L3GXX* l3g, uint8_t addr, uint8_t data);
uint8_t g_l3gxx_rd_word(L3GXX* l3g, uint8_t addr);
void g_l3gxx_wr(L3GXX* l3g, uint8_t addr, uint8_t* data, size_t size);
void g_l3gxx_rd(L3GXX* l3g, uint8_t addr, uint8_t* data, size_t size);

void g_l3gd20h_init()
{
  l3gd20h.comm.type = commSPI;
  l3gd20h.comm.spiIF = &spiIF_L3GD20H;
  l3gd20h.valid = bFalse;
  l3gd20h.whoami = L3GD20H_ID;
  l3gd20h.fullScale = l3gxx245dps;
}

void g_l3g4250d_init()
{
  l3g4250d.comm.type = commI2C;
  l3g4250d.comm.i2cIF = &i2cIF_I3G4250D;
  l3g4250d.valid = bFalse;
  l3g4250d.whoami = L3G4250D_ID;
  l3g4250d.fullScale = l3gxx245dps;
}

Bool g_l3gxx_is_ready(L3GXX* l3g)
{
  return comm_is_ready(&l3g->comm, L3GXX_WHO_AM_I, l3g->whoami);
}

void g_l3gxx_config(L3GXX* l3g)
{
  if (g_l3gxx_is_ready(l3g) != bTrue) return;
  l3g->valid = bTrue;

  L3GXX_Ctrl1Rg reg1 = {
      .dr = 3,
      .bw = 3,
      .pd = 0,
      .zEn = 1,
      .yEn = 1,
      .xEn = 1
  };
  L3GXX_Ctrl2Rg reg2 = {
      .extREn = 0,
      .lvlEn = 0,
      .hpm = 0,
      .hpcf = 0
  };
  L3GXX_Ctrl3Rg reg3 = {
      .int1Ig = 0,
      .int1Boot = 0,
      .hLactive = 0,
      .ppOd = 0,
      .int2DRdy = 0,
      .int2Fth = 0,
      .int2ORun = 0,
      .int2Empty = 0
  };
  L3GXX_Ctrl4Rg reg4 = {
      .bdu = 0,
      .ble = 0,
      .fs = l3g->fullScale,
      .imPEn = 0,
      .st = 0
  };
  L3GXX_Ctrl5Rg reg5 = {
      .boot = 0,
      .fifoEn = 0,
      .stopOfFth = 0,
      .hPEn = 0,
      .igSel = 0,
      .outSel = 0
  };

  g_l3gxx_wr_word(l3g, L3GXX_CTRL2, *(uint8_t*)&reg2);
  g_l3gxx_wr_word(l3g, L3GXX_CTRL3, *(uint8_t*)&reg3);
  g_l3gxx_wr_word(l3g, L3GXX_CTRL4, *(uint8_t*)&reg4);
  g_l3gxx_wr_word(l3g, L3GXX_REFERENCE, 0);
  g_l3gxx_wr_word(l3g, L3GXX_CTRL5, *(uint8_t*)&reg5);
  g_l3gxx_wr_word(l3g, L3GXX_CTRL1, *(uint8_t*)&reg1);

  g_l3gxx_out_mngt(l3g, bTrue);
}

void g_l3gxx_wr_word(L3GXX* l3g, uint8_t addr, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  g_l3gxx_wr(l3g, addr, txData, 1);
}

uint8_t g_l3gxx_rd_word(L3GXX* l3g, uint8_t addr)
{
  uint8_t data;
  g_l3gxx_rd(l3g, addr, &data, 1);
  return data;
}

void g_l3gxx_wr(L3GXX* l3g, uint8_t addr, uint8_t* data, size_t size)
{
  comm_send(&l3g->comm, addr, data, size);
}

void g_l3gxx_rd(L3GXX* l3g, uint8_t addr, uint8_t* data, size_t size)
{
  comm_recv(&l3g->comm, addr, data, size);
}

void g_l3gxx_refresh_xyz(L3GXX* l3g)
{
  if (l3g->valid != bTrue) return;
  double fullScaleVal = 0;
  switch (l3g->fullScale) {
    case l3gxx245dps: fullScaleVal = 245; break;
    case l3gxx500dps: fullScaleVal = 500; break;
    case l3gxx2000dps: fullScaleVal = 2000; break;
  }
  uint8_t data[6];
  g_l3gxx_rd(l3g, L3GXX_OUT_X_L, &data[0], 1);
  g_l3gxx_rd(l3g, L3GXX_OUT_X_H, &data[1], 1);
  g_l3gxx_rd(l3g, L3GXX_OUT_Y_L, &data[2], 1);
  g_l3gxx_rd(l3g, L3GXX_OUT_Y_H, &data[3], 1);
  g_l3gxx_rd(l3g, L3GXX_OUT_Z_L, &data[4], 1);
  g_l3gxx_rd(l3g, L3GXX_OUT_Z_H, &data[5], 1);
# ifdef DEVICE_TYPE_ATB
  sens.gyr.raw.x = (int16_t)(data[1] << 8 | data[0]);
  sens.gyr.raw.y = (int16_t)(data[3] << 8 | data[2]);
  sens.gyr.raw.z = (int16_t)(data[5] << 8 | data[4]);
# endif
# ifdef DEVICE_TYPE_ATV
  sens.gyr.raw.x = -(int16_t)(data[3] << 8 | data[2]);
  sens.gyr.raw.y = (int16_t)(data[1] << 8 | data[0]);
  sens.gyr.raw.z = (int16_t)(data[5] << 8 | data[4]);
# endif
  sens.gyr.ripe.x = sens.gyr.raw.x * fullScaleVal / 0x7FFFL;
  sens.gyr.ripe.y = sens.gyr.raw.y * fullScaleVal / 0x7FFFL;
  sens.gyr.ripe.z = sens.gyr.raw.z * fullScaleVal / 0x7FFFL;
}

void g_l3gxx_refresh_temp(L3GXX* l3g)
{
  if (l3g->valid != bTrue) return;
  sens.gyr.raw.temp = (int8_t)g_l3gxx_rd_word(l3g, L3GXX_OUT_TEMP);
  sens.gyr.ripe.temp = sens.gyr.raw.temp * -1 + 25;
}

void g_l3gxx_out_mngt(L3GXX* l3g, Bool en)
{
  if (l3g->valid != bTrue) return;
  uint8_t temp = g_l3gxx_rd_word(l3g, L3GXX_CTRL1);
  L3GXX_Ctrl1Rg reg1 = *(L3GXX_Ctrl1Rg*)&temp;
  reg1.pd = en;
  g_l3gxx_wr_word(l3g, L3GXX_CTRL1, *(uint8_t*)&reg1);
}
