#include "m_lis3mdl.h"
#include "spi.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"

LIS3MDL lis3mdl;

void m_lis3mdl_wr_word(LIS3MDL* lis, uint8_t addr, uint8_t data);
uint8_t m_lis3mdl_rd_word(LIS3MDL* lis, uint8_t addr);
void m_lis3mdl_wr(LIS3MDL* lis, uint8_t addr, uint8_t* data, size_t size);
void m_lis3mdl_rd(LIS3MDL* lis, uint8_t addr, uint8_t* data, size_t size);

void m_lis3mdl_init()
{
  lis3mdl.comm.type = commSPI;
  lis3mdl.comm.spiIF = &spiIF_LIS3MDL;
  lis3mdl.valid = bFalse;
  lis3mdl.whoami = LIS3MDL_ID;
  lis3mdl.fullScale = lis3mdl4gauss;
}

Bool m_lis3mdl_is_ready(LIS3MDL* lis)
{
  return comm_is_ready(&lis->comm, LIS3MDL_WHO_AM_I, lis->whoami);
}

void m_lis3mdl_config(LIS3MDL* lis)
{
  if (m_lis3mdl_is_ready(lis) != bTrue) return;
  lis->valid = bTrue;

  LIS3MDL_Ctrl1Rg reg1 = {
      .tempEn = 0,
      .om = 0,
      .dOut = 0,
      .fastOdr = 1,
      .st = 0
  };
  LIS3MDL_Ctrl2Rg reg2 = {
      .fs = lis->fullScale,
      .reboot = 0,
      .softRst = 0
  };
  LIS3MDL_Ctrl3Rg reg3 = {
      .lp = 0,
      .sim = 0,
      .md = 3
  };
  LIS3MDL_Ctrl4Rg reg4 = {
      .omz = 0,
      .ble = 0
  };
  LIS3MDL_Ctrl5Rg reg5 = {
      .fastRead = 0,
      .bdu = 0
  };

  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG1, *(uint8_t*)&reg1);
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG2, *(uint8_t*)&reg2);
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG3, *(uint8_t*)&reg3);
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG4, *(uint8_t*)&reg4);
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG5, *(uint8_t*)&reg5);

  m_lis3mdl_out_xyz_mngt(lis, bTrue);
  m_lis3mdl_out_temp_mngt(lis, bTrue);
}

void m_lis3mdl_wr_word(LIS3MDL* lis, uint8_t addr, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  m_lis3mdl_wr(lis, addr, txData, 1);
}

uint8_t m_lis3mdl_rd_word(LIS3MDL* lis, uint8_t addr)
{
  uint8_t data;
  m_lis3mdl_rd(lis, addr, &data, 1);
  return data;
}

void m_lis3mdl_wr(LIS3MDL* lis, uint8_t addr, uint8_t* data, size_t size)
{
  comm_send(&lis->comm, addr, data, size);
}

void m_lis3mdl_rd(LIS3MDL* lis, uint8_t addr, uint8_t* data, size_t size)
{
  comm_recv(&lis->comm, addr, data, size);
}

void m_lis3mdl_refresh_xyz(LIS3MDL* lis)
{
  if (lis->valid != bTrue) return;
  const float offset[] = {-2040.43, 1408.21, 265.62};
  /* compensation matrix */
//  const float comp[3][3] = {
//      {3.122E-4, 5.756E-5, -1.031E-6},
//      {0.0,      3.201E-4, -7.366E-7},
//      {0.0,      0.0,       1.185}
//  };
  double sensitivity = 0;    /* sensitivity */
  switch (lis->fullScale) {
    case lis3mdl4gauss: sensitivity = 6842; break;
    case lis3mdl8gauss: sensitivity = 3421; break;
    case lis3mdl12gauss: sensitivity = 2281; break;
    case lis3mdl16gauss: sensitivity = 1711; break;
  }
  uint8_t data[6];
  m_lis3mdl_rd(lis, LIS3MDL_OUT_X_L, data, sizeof(data));
  sens.mag.raw.x = (int16_t)(data[1] << 8 | data[0]) - offset[0];
  sens.mag.raw.y = (int16_t)(data[3] << 8 | data[2]) - offset[1];
  sens.mag.raw.z = (int16_t)(data[5] << 8 | data[4]) - offset[2];
//  xRawVal = x * comp[0][0] + y * comp[0][1] + z * comp[0][2];
//  yRawVal = x * comp[1][0] + y * comp[1][1] + z * comp[1][2];
//  zRawVal = x * comp[2][0] + y * comp[2][1] + z * comp[2][2];
  sens.mag.ripe.x = sens.mag.raw.x / sensitivity;
  sens.mag.ripe.y = sens.mag.raw.y / sensitivity;
  sens.mag.ripe.z = sens.mag.raw.z / sensitivity;
}

void m_lis3mdl_refresh_temp(LIS3MDL* lis)
{
  if (lis->valid != bTrue) return;
  uint8_t data[2];
  m_lis3mdl_rd(lis, LIS3MDL_TEMP_OUT_L, data, sizeof(data));
  sens.mag.raw.temp = (int16_t)(data[1] << 8 | data[0]);
  sens.mag.ripe.temp = sens.mag.raw.temp / 8.0 + 25.0;
}

void m_lis3mdl_out_xyz_mngt(LIS3MDL* lis, Bool en)
{
  if (lis->valid != bTrue) return;
  uint8_t temp = m_lis3mdl_rd_word(lis, LIS3MDL_CTRL_REG3);
  LIS3MDL_Ctrl3Rg reg3 = *(LIS3MDL_Ctrl3Rg*)&temp;
  reg3.md = en == bTrue ? 0 : 3;
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG3, *(uint8_t*)&reg3);
}

void m_lis3mdl_out_temp_mngt(LIS3MDL* lis, Bool en)
{
  if (lis->valid != bTrue) return;
  uint8_t temp = m_lis3mdl_rd_word(lis, LIS3MDL_CTRL_REG1);
  LIS3MDL_Ctrl1Rg reg1 = *(LIS3MDL_Ctrl1Rg*)&temp;
  reg1.tempEn = en;
  m_lis3mdl_wr_word(lis, LIS3MDL_CTRL_REG1, *(uint8_t*)&reg1);
}
