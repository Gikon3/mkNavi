#include "b_bmp280.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"

BMP230 bmp230;

void b_bmp230_wr(BMP230* bmp, uint8_t address, uint8_t* data, uint16_t size);
void b_bmp230_rd(BMP230* bmp, uint8_t address, uint8_t* data, uint16_t size);
void b_bmp230_wr_word(BMP230* bmp, uint8_t address, uint8_t data);
uint8_t b_bmp230_rd_word(BMP230* bmp, uint8_t address);

static void get_calib_param(BMP230* bmp);
//static int32_t comp_temp_32bit(BMP230* bmp, int32_t val);
static double comp_temp_double(BMP230* bmp, int32_t val);
//static uint32_t comp_pres_32bit(BMP230* bmp, uint32_t val);
//static uint32_t comp_pres_64bit(BMP230* bmp, uint32_t val);
static double comp_pres_double(BMP230* bmp, uint32_t val);

void b_bmp230_init()
{
  bmp230.comm.type = commI2C;
  bmp230.comm.i2cIF = &i2cIF_BMP280;
  bmp230.valid = bFalse;
  bmp230.whoami = BMP230_ID;
}

Bool b_bmp230_is_ready(BMP230* bmp)
{
  return comm_is_ready(&bmp->comm, BMP230_WHO_AM_I, bmp->whoami);
}

void b_bmp230_config(BMP230* bmp)
{
  if (b_bmp230_is_ready(bmp) != bTrue) return;
  bmp->valid = bTrue;

  BMP230_Conf regConf = {
      .tsb = 0,
      .filter = 4,
      .spi3w = 0
  };
  BMP230_CtrlMeas regCtrlMeas = {
      .osrst = 2,
      .osrsp = 5,
      .mode = 0
  };

  b_bmp230_wr_word(bmp, BMP230_CONF, *(uint8_t*)&regConf);
  b_bmp230_wr_word(bmp, BMP230_CTRL_MEAS, *(uint8_t*)&regCtrlMeas);

  get_calib_param(bmp);

  b_bmp230_out_mngt(bmp, bTrue);
}

void b_bmp230_wr(BMP230* bmp, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_send(&bmp->comm, address, data, size);
}

void b_bmp230_rd(BMP230* bmp, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_recv(&bmp->comm, address, data, size);
}

void b_bmp230_wr_word(BMP230* bmp, uint8_t address, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  b_bmp230_wr(bmp, address, txData, 1);
}

uint8_t b_bmp230_rd_word(BMP230* bmp, uint8_t address)
{
  uint8_t rxData;
  b_bmp230_rd(bmp, address, &rxData, 1);
  return rxData;
}

void b_bmp230_refresh(BMP230* bmp)
{
  if (bmp->valid != bTrue) return;
  uint8_t data[6];
  b_bmp230_rd(bmp, BMP230_PRESS_MSB, data, sizeof(data));
  sens.bar.raw.val = (uint32_t)data[0] << 12 | (uint32_t)data[1] << 4 | data[2] >> 4;
  sens.bar.raw.temp = (uint32_t)data[3] << 12 | (uint32_t)data[4] << 4 | data[5] >> 4;
  sens.bar.ripe.temp = comp_temp_double(bmp, sens.bar.raw.temp);
  sens.bar.ripe.val = comp_pres_double(bmp, sens.bar.raw.val);
}

void b_bmp230_out_mngt(BMP230* bmp, Bool en)
{
  if (bmp->valid != bTrue) return;
  uint8_t temp = b_bmp230_rd_word(bmp, BMP230_CTRL_MEAS);
  BMP230_CtrlMeas regCtrlMeas = *(BMP230_CtrlMeas*)&temp;
  regCtrlMeas.mode = en == bTrue ? 3 : 0;
  b_bmp230_wr_word(bmp, BMP230_CTRL_MEAS, *(uint8_t*)&regCtrlMeas);
}

static void get_calib_param(BMP230* bmp)
{
  if (b_bmp230_is_ready(bmp) != bTrue) return;

  uint8_t calib[24];
  b_bmp230_rd(bmp, BMP230_CALIB, calib, sizeof(calib));
  bmp->calib.digT1 = (uint16_t)calib[1] << 8 | calib[0];
  bmp->calib.digT2 = (int16_t)calib[3] << 8 | calib[2];
  bmp->calib.digT3 = (int16_t)calib[5] << 8 | calib[4];
  bmp->calib.digP1 = (uint16_t)calib[7] << 8 | calib[6];
  bmp->calib.digP2 = (int16_t)calib[9] << 8 | calib[8];
  bmp->calib.digP3 = (int16_t)calib[11] << 8 | calib[10];
  bmp->calib.digP4 = (int16_t)calib[13] << 8 | calib[12];
  bmp->calib.digP5 = (int16_t)calib[15] << 8 | calib[14];
  bmp->calib.digP6 = (int16_t)calib[17] << 8 | calib[16];
  bmp->calib.digP7 = (int16_t)calib[19] << 8 | calib[18];
  bmp->calib.digP8 = (int16_t)calib[21] << 8 | calib[20];
  bmp->calib.digP9 = (int16_t)calib[23] << 8 | calib[22];
}

//static int32_t comp_temp_32bit(BMP230* bmp, int32_t val)
//{
//  if (b_bmp230_is_ready(bmp) != bTrue) return -128;
//
//  int32_t var1 = (((val / 8 - ((int32_t)bmp->calib.digT1 << 1))) * (int32_t)bmp->calib.digT2) / 2048;
//  int32_t var2 = ((((val / 16 - (int32_t)bmp->calib.digT1) *
//      (val / 16 - (int32_t)bmp->calib.digT1)) / 4096) * (int32_t)bmp->calib.digT3) / 16384;
//  bmp->calib.tFine = var1 + var2;
//  return (bmp->calib.tFine * 5 + 128) / 256;
//}
static double comp_temp_double(BMP230* bmp, int32_t val)
{
  if (b_bmp230_is_ready(bmp) != bTrue) return -128.0;

  double var1 = ((double)val / 16384.0 - (double)bmp->calib.digT1 / 1024.0) * (double)bmp->calib.digT2;
  double var2 = (((double)val / 131072.0 - (double)bmp->calib.digT1 / 8192.0) *
               ((double)val / 131072.0 - (double)bmp->calib.digT1 / 8192.0)) * (double)bmp->calib.digT3;
  bmp->calib.tFine = (int32_t)(var1 + var2);
  return (var1 + var2) / 5120.0;
}

//static uint32_t comp_pres_32bit(BMP230* bmp, uint32_t val)
//{
//  if (b_bmp230_is_ready(bmp) != bTrue) return 0;
//
//  int32_t var1 = (int32_t)bmp->calib.tFine / 2 - (int32_t)64000;
//  int32_t var2 = (((var1 / 4) * (var1 / 4)) / 2048) * (int32_t)bmp->calib.digP6;
//  var2 = var2 + ((var1 * (int32_t)bmp->calib.digP5) * 2);
//  var2 = (var2 / 4) + ((int32_t)bmp->calib.digP4 * 65536);
//  var1 = (((bmp->calib.digP3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
//      ((((int32_t)bmp->calib.digP2) * var1) / 2)) / 262144;
//  var1 = ((32768 + var1) * (int32_t)bmp->calib.digP1) / 32768;
//
//  if (var1 != 0) {
//    uint32_t press = (uint32_t)(((int32_t)(1048576 - val) - (var2 / 4096)) * 3125);
//    if (press < 0x80000000) press = (press << 1) / (uint32_t)var1;
//    else press = (press / (uint32_t)var1) * 2;
//    var1 = ((int32_t)bmp->calib.digP9 * ((int32_t)(((press / 8) * (press / 8)) / 8192))) / 4096;
//    var2 = ((int32_t)(press / 4) * ((int32_t)bmp->calib.digP8)) / 8192;
//    press = (uint32_t)((int32_t) press + ((var1 + var2 + bmp->calib.digP7) / 16));
//    return press;
//  }
//
//  return 0;
//}
//static uint32_t comp_pres_64bit(BMP230* bmp, uint32_t val)
//{
//  if (b_bmp230_is_ready(bmp) != bTrue) return 0;
//
//  int64_t var1 = ((int64_t)bmp->calib.tFine) - 128000;
//  int64_t var2 = var1 * var1 * (int64_t)bmp->calib.digP6;
//  var2 = var2 + ((var1 * (int64_t)bmp->calib.digP5) * 131072);
//  var2 = var2 + (((int64_t)bmp->calib.digP4) * 34359738368);
//  var1 = ((var1 * var1 * (int64_t)bmp->calib.digP3) / 256) + ((var1 * (int64_t)bmp->calib.digP2) * 4096);
//  var1 = (((int64_t)0x800000000000 + var1) * ((int64_t)bmp->calib.digP1)) / 8589934592;
//
//  if (var1 != 0) {
//    int64_t p = 1048576 - val;
//    p = (((((p * 2147483648U)) - var2) * 3125) / var1);
//    var1 = (((int64_t)bmp->calib.digP9) * (p / 8192) * (p / 8192)) / 33554432;
//    var2 = (((int64_t)bmp->calib.digP8) * p) / 524288;
//    p = ((p + var1 + var2) / 256) + (((int64_t)bmp->calib.digP7) * 16);
//    return (uint32_t)p;
//  }
//
//  return 0;
//}
static double comp_pres_double(BMP230* bmp, uint32_t val)
{
  if (b_bmp230_is_ready(bmp) != bTrue) return 0;

  double var1 = ((double)bmp->calib.tFine / 2.0) - 64000.0;
  double var2 = var1 * var1 * ((double)bmp->calib.digP6) / 32768.0;
  var2 = var2 + var1 * ((double)bmp->calib.digP5) * 2.0;
  var2 = (var2 / 4.0) + (((double)bmp->calib.digP4) * 65536.0);
  var1 = (((double)bmp->calib.digP3) * var1 * var1 / 524288.0 + ((double)bmp->calib.digP2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)bmp->calib.digP1);

  if (var1 < 0 || var1 > 0) {
    double press = 1048576.0 - (double)val;
    press = (press - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)bmp->calib.digP9) * press * press / 2147483648.0;
    var2 = press * ((double)bmp->calib.digP8) / 32768.0;
    press = press + (var1 + var2 + ((double)bmp->calib.digP7)) / 16.0;
    return press;
  }

  return 0.0;
}
