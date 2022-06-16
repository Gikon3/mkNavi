#include "bat_ctrl.h"
#include "i2c.h"
#include "powi.h"

static I2C_IF* i2cIf;
static ChrgStat chrgStat;
static PgStat pgStat;
static VbusStat vbusStat;
static IcoStat icoStat;
static int16_t ibus;  /* in mA */
static int16_t ichrg; /* in mA */
static int16_t vbus;  /* in mV */
static int16_t vbat;  /* in mV */
static int16_t vsys;  /* in mV */
static float ts;      /* in % */
static float tdie;    /* in C */

static void bat_init()
{
  i2cIf = &i2cIF_Bat;
}

HAL_StatusTypeDef bat_is_ready()
{
  uint8_t isReady;
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeIsReady;
  node.address = 0;
  node.data = &isReady;
  node.dataSize = sizeof(isReady);
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
  return isReady;
}

void bat_config()
{
  bat_init();
  if (bat_is_ready() != HAL_OK) {
    return;
  }

  BatReg06 reg06 = {
      .enOtg = 0,
      .autoIndetEn = 1,
      .treg = 3,
      .enChg = 1,
      .batlowv = 1,
      .vrechg = 2
  };

  bat_wr_word(BAT_REG00, 0xA0);   /* 8400 mV */
  bat_wr_word(BAT_REG03, 0x0A);   /* 1500 mA */
  bat_wr_word(BAT_REG06, *(uint8_t*)&reg06);
  bat_wr_word(BAT_REG15, 0x30);
}

void bat_wr(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeTx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = NULL;
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
}

void bat_rd(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeRx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
}

void bat_wr_word(uint8_t address, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  bat_wr(address, txData, 1);
}

uint8_t bat_rd_word(uint8_t address)
{
  uint8_t rxData;
  bat_rd(address, &rxData, 1);
  return rxData;
}

void bat_refresh_chrg_stat()
{
  uint8_t data[2];
  bat_rd(BAT_REG0B, data, sizeof(data));
  chrgStat = data[0] & 0x7;
  pgStat = (data[1] >> 7) & 0x1;
  vbusStat = (data[1] >> 4) & 0x7;
  icoStat = (data[1] >> 1) & 0x3;
}

ChrgStat bat_get_chrg_stat()
{
  return chrgStat;
}

PgStat bat_get_pg_stat()
{
  return pgStat;
}

VbusStat bat_get_vbus_stat()
{
  return vbusStat;
}

IcoStat bat_get_ico_stat()
{
  return icoStat;
}

static int16_t decode_adc(uint8_t const* data, size_t size)
{
  static const size_t byteSize = 8;
  int16_t val = 0;
  for (size_t add = 0, decrSize = size - 1; add < size; ++add, --decrSize) {
    for (size_t i = 0; i < byteSize; ++i) {
      uint8_t en = (data[decrSize] >> i) & 1;
      if (en) {
        val += powi(2, i + byteSize * add);
      }
    }
  }
  return val;
}

void bat_refresh_ibus()
{
  uint8_t data[2];
  bat_rd(BAT_REG17, data, sizeof(data));
  ibus = decode_adc(data, sizeof(data));
}

void bat_refresh_ichrg()
{
  uint8_t data[2];
  bat_rd(BAT_REG19, data, sizeof(data));
  ichrg = decode_adc(data, sizeof(data));
}

void bat_refresh_vbus()
{
  uint8_t data[2];
  bat_rd(BAT_REG1B, data, sizeof(data));
  vbus = decode_adc(data, sizeof(data));
}

void bat_refresh_vbat()
{
  uint8_t data[2];
  bat_rd(BAT_REG1D, data, sizeof(data));
  vbat = decode_adc(data, sizeof(data));
}

void bat_refresh_vsys()
{
  uint8_t data[2];
  bat_rd(BAT_REG1F, data, sizeof(data));
  vsys = decode_adc(data, sizeof(data));
}

void bat_refresh_ts()
{
  uint8_t data[2];
  bat_rd(BAT_REG21, data, sizeof(data));

  static const size_t byteSize = 8;
  static const float table[] = { 0.098, 0.195, 0.391, 0.781, 1.563,
      3.125, 6.25, 12.50, 25.0, 50.0 };
  float val = 0;
  for (size_t add = 0, decrSize = sizeof(data) - 1; add < sizeof(data); ++add, --decrSize) {
    for (size_t i = 0; i < byteSize && (i + byteSize * add) < (sizeof(table) / sizeof(table[0])); ++i) {
      uint8_t en = (data[decrSize] >> i) & 1;
      if (en) {
        val += table[i + byteSize * add];
      }
    }
  }
  ts = val;
}

void bat_refresh_tdie()
{
  uint8_t data[2];
  bat_rd(BAT_REG23, data, sizeof(data));

  static const size_t byteSize = 8;
  float val = 0;
  for (size_t add = 0, decrSize = sizeof(data) - 1; add < sizeof(data); ++add, --decrSize) {
    for (uint8_t i = 0; i < byteSize; ++i) {
      uint8_t en = (data[decrSize] >> i) & 1;
      if (en) {
        if (!i && !add) {
          val += 0.5;
        }
        else {
          val += powi(2, i + byteSize * add - 1);
        }
      }
    }
  }
  tdie = val;
}

int16_t bat_get_ibus()
{
  return ibus;
}

int16_t bat_get_ichrg()
{
  return ichrg;
}

int16_t bat_get_vbus()
{
  return vbus;
}

int16_t bat_get_vbat()
{
  return vbat;
}

int16_t bat_get_vsys()
{
  return vsys;
}

float bat_get_ts()
{
  return ts;
}

float bat_get_tdie()
{
  return tdie;
}

void bat_out_en()
{
  uint8_t temp = bat_rd_word(BAT_REG15);
  bat_wr_word(BAT_REG15, temp | 0x80);
}

void bat_out_dis()
{
  uint8_t temp = bat_rd_word(BAT_REG15);
  bat_wr_word(BAT_REG15, temp & 0x7F);
}
